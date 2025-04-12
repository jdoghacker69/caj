#include "serial.h"

#include "stm32f303xc.h"

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

	// Enable clocks
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->AHBENR  |= serial_port->MaskAHBENR;      // GPIO
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;     // USART on APB1 (if any)
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;     // USART on APB2 (if any)

	// Configure GPIO for TX/RX (assume pins already chosen in SerialPort struct)
	serial_port->GPIO->MODER   |= serial_port->SerialPinModeValue;
	serial_port->GPIO->OSPEEDR |= serial_port->SerialPinSpeedValue;
	serial_port->GPIO->AFR[0]  |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1]  |= serial_port->SerialPinAlternatePinValueHigh;

	// Set baud rate (assume PCLK = 8 MHz)
	uint16_t *brr = (uint16_t*)&serial_port->UART->BRR;
	switch (baudRate) {
		case BAUD_9600:   *brr = 0x341; break;
		case BAUD_19200:  *brr = 0x1A0; break;
		case BAUD_38400:  *brr = 0xD0;  break;
		case BAUD_57600:  *brr = 0x8B;  break;
		case BAUD_115200: *brr = 0x46;  break;
		default:          *brr = 0x46;  break;
	}

	// Enable USART: TE, RE, UE, RXNEIE
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;

	// Enable NVIC interrupt
	if (serial_port->UART == USART1) {
		NVIC_EnableIRQ(USART1_IRQn);
	}
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

        for (volatile uint32_t d = 0; d < 50000; d++) {
            __asm__("nop");
        }

		counter++;
		pt++;
	}

	serial_port->completion_function(counter);
}


extern volatile uint8_t start_command_received; //declaring it for this file
void USART1_IRQHandler(void)
{
    if (USART1->ISR & USART_ISR_RXNE) {
        char c = USART1->RDR;  // Read received character

        // Echo it back immediately
        SerialOutputByte(c);  // echo back function

        // Store in buffer
        if (cmd_idx < CMD_BUF_SIZE - 1) {
            cmd_buf[cmd_idx++] = c;
        }

        // Check for 'start\r\n'
        if (cmd_idx >= 5 &&
            cmd_buf[cmd_idx - 5] == 's' &&
            cmd_buf[cmd_idx - 4] == 't' &&
            cmd_buf[cmd_idx - 3] == 'a' &&
            cmd_buf[cmd_idx - 2] == 'r' &&
            cmd_buf[cmd_idx - 1] == 't') {
            start_command_received = 1;
            cmd_idx = 0;  // Reset buffer
        }
    }
}

void SerialOutputByte(uint8_t byte) {
    while (!(USART1->ISR & USART_ISR_TXE)) {
        // Wait until transmit data register is empty
    }
    USART1->TDR = byte;
}
