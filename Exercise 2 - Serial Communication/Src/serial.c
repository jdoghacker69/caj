#include "serial.h"
#include "stm32f303xc.h"
#include <string.h>

// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef *GPIO;
	volatile uint32_t MaskAPB2ENR;	// mask to enable RCC APB2 bus registers
	volatile uint32_t MaskAPB1ENR;	// mask to enable RCC APB1 bus registers
	volatile uint32_t MaskAHBENR;	// mask to enable RCC AHB bus registers
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;
	void (*completion_function)(uint32_t);
};



// instantiate the serial port parameters
//   note: the complexity is hidden in the c file
SerialPort USART1_PORT = {USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	// bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN, // bit to enable for AHB bus
		0xA00,
		0xF00,
		0x770000,  // for USART1 PC10 and 11, this is in the AFR low register
		0x00, // no change to the high alternate function register
		0x00 // default function pointer is NULL
		};


// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set

void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {

	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;    // PC4/PC5
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // USART1

	// enable the GPIO which is on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// set pin mode to alternate function for the specific GPIO pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

	// enable high speed clock for specific GPIO pins
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to external pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	serial_port->UART->CR1 |= USART_CR1_RXNEIE; // RX not empty interrupt
	serial_port->UART->CR1 |= USART_CR1_TXEIE;  // TX empty interrupt (optional)

	// enable the device based on the bits defined in the serial port definition
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}


	// enable serial port for tx and rx
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void setupNVIC(void)
{
    __disable_irq();
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
    __enable_irq();
}

int SerialInputAvailable(SerialPort *serial_port) {
    return (serial_port->UART->ISR & USART_ISR_RXNE);
}

uint8_t SerialReadChar(SerialPort *serial_port) {
    return serial_port->UART->RDR;
}

//  Function to check if input is the command "start"
int is_start_command(const uint8_t *input) {
    return strcmp((const char *)input, "start") == 0;
}

volatile uint8_t input_received_flag = 0;
volatile uint8_t received_string[MAX_BUFFER_LENGTH];
volatile uint32_t string_index = 0;
uint8_t start_flag = 0;

volatile uint8_t db_buffer1[DB_BUFFER_SIZE];
volatile uint8_t db_buffer2[DB_BUFFER_SIZE];

// Pointer to the current fill buffer (where new data is appended)
volatile uint8_t *fill_buffer = db_buffer1;
// Pointer to the buffer that is currently being transmitted
volatile uint8_t *transmit_buffer = db_buffer2;

// Index into the fill buffer (write pointer)
volatile uint32_t fill_index = 0;
// Current read index into the transmit buffer
volatile uint32_t transmit_index = 0;
// Number of bytes waiting for transmission in the transmit buffer
volatile uint32_t transmit_length = 0;

// Flag indicating that a transmission is in progress
volatile uint8_t is_transmitting = 0;



void USART1_EXTI25_IRQHandler(void) {
    // === Error Check: Framing and Overrun ===
    if ((USART1->ISR & USART_ISR_FE_Msk) || (USART1->ISR & USART_ISR_ORE_Msk)) {
    	USART1->ICR = USART_ICR_FECF | USART_ICR_ORECF;
        return;
    }

    // === RXNE: Receive register not empty ===
    if (USART1->ISR & USART_ISR_RXNE) {
        while (SerialInputAvailable(&USART1_PORT)) {
            uint8_t c = SerialReadChar(&USART1_PORT);
            if (string_index < MAX_BUFFER_LENGTH - 1) {
                received_string[string_index++] = c;
                if (c == '\n' || c == '\r') {
                    received_string[string_index - 1] = '\0'; // null-terminate
                    input_received_flag = 1;
                }
            } else {
                string_index = 0; // Overflow fallback
            }
        }
    }

    // === TXE: Transmit register empty (double buffering logic) ===
    if (is_transmitting && transmit_length == 0) { // safety guard
        USART1->CR1 &= ~USART_CR1_TXEIE;
        is_transmitting = 0;
    }


    if ((USART1->ISR & USART_ISR_TXE) && is_transmitting) {
        if (transmit_index < transmit_length) {
            USART1->TDR = transmit_buffer[transmit_index++];
        } else {
            // End of transmission for current buffer
            USART1->CR1 &= ~USART_CR1_TXEIE;

            if (USART1_PORT.completion_function) {
                USART1_PORT.completion_function(transmit_length);
            }

            is_transmitting = 0;

            // If more data was filled during transmission, swap buffers and restart
            if (fill_index > 0) {
                uint8_t *temp = fill_buffer;
                fill_buffer = transmit_buffer;
                transmit_buffer = temp;

                transmit_length = fill_index;
                fill_index = 0;
                transmit_index = 0;

                is_transmitting = 1;
                USART1->CR1 |= USART_CR1_TXEIE;
            }
        }
    }

    // === Post-command processing ===
    if (input_received_flag == 1) {
        SerialOutputStringDB((uint8_t *)received_string, &USART1_PORT);  // <-- Use DB version here

        if (is_start_command(received_string) == 1) {
            uint8_t response[] = "\n--SECRET-- Command START received.\n";
            SerialOutputStringDB(response, &USART1_PORT);  // <-- Use DB version here
            start_flag = 1;
        }

        // Reset state
        string_index = 0;
        input_received_flag = 0;

        for (uint32_t i = 0; i < MAX_BUFFER_LENGTH; i++) {
            received_string[i] = 0;
        }
    }
}

void SerialOutputStringDB(uint8_t *pt, SerialPort *serial_port) {
    // Disable interrupts briefly if necessary for concurrency safety
    __disable_irq();

    // Append the string to the fill buffer until either the string ends or the fill buffer is nearly full
    while(*pt && fill_index < (DB_BUFFER_SIZE - 1)) {
        fill_buffer[fill_index++] = *pt++;
    }
    // Null terminate the string (if needed, though the TX routine could use transmit_length)
    fill_buffer[fill_index] = '\0';

    // If no transmission is active, swap the buffers and kick-off the sending process
    if (!is_transmitting) {
        // Swap the pointers: the current fill buffer becomes the transmit buffer.
        uint8_t *temp = fill_buffer;
        fill_buffer = transmit_buffer;
        transmit_buffer = temp;

        // Set the transmit length equal to the number of bytes in the (old) fill buffer.
        transmit_length = fill_index;

        // Reset the fill index (so the new fill buffer starts empty)
        fill_index = 0;
        transmit_index = 0;

        // Mark that we are transmitting and start the TX empty interrupt.
        is_transmitting = 1;
        serial_port->UART->CR1 |= USART_CR1_TXEIE;
    }
    __enable_irq();
}


