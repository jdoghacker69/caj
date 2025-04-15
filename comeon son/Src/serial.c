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


void SerialOutputChar(uint8_t data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){
	}

	serial_port->UART->TDR = data;
}



void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {

	uint32_t counter = 0;
	while(*pt) {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}

	serial_port->completion_function(counter);
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

void USART1_EXTI25_IRQHandler(void) {

	// Check for overrun or frame errors
	if ((USART1->ISR & USART_ISR_FE_Msk) && (USART1->ISR & USART_ISR_ORE_Msk)) {
		return;
	}

	if (USART1->ISR & USART_ISR_RXNE) {
		while (SerialInputAvailable(&USART1_PORT)) {
		    uint8_t c = SerialReadChar(&USART1_PORT);
		    if (string_index < MAX_BUFFER_LENGTH - 1) {
		        received_string[string_index++] = c;
		        if (c == '\n' || c == '\r') {
		            received_string[string_index - 1] = '\0'; // null-terminate
		            input_received_flag = 1;
		        }
		    }
		    else {
		        string_index = 0;
		    }
		}
	}

		if (input_received_flag == 1) {
		    SerialOutputString((uint8_t *)received_string, &USART1_PORT);
		    //  Check if the command is "start" // if not - restart the while loop
		    if (is_start_command(received_string) == 1) {
		        uint8_t response[] = "\n--SECRET-- Command START received.\n";
		        SerialOutputString(response, &USART1_PORT);
		        ShapeDemo();
		        return;
		    }
		    // Reset
		    string_index = 0;
		    input_received_flag = 0;

		    for (uint32_t i = 0; i < MAX_BUFFER_LENGTH; i++) {
		        received_string[i] = 0;
		    }
		}
	}

void setupNVIC(void)
{
    __disable_irq();
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
    __enable_irq();
}



