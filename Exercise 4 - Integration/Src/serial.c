#include "stm32f303xc.h"
#include "serial.h"

// Struct to define a serial port configuration
struct _SerialPort
{
	USART_TypeDef *UART;                   				// Pointer to USART peripheral
	GPIO_TypeDef *GPIO;                    				// Pointer to GPIO port
	volatile uint32_t MaskAPB2ENR;         				// Mask to enable RCC APB2 bus for USART
	volatile uint32_t MaskAPB1ENR;         				// Mask to enable RCC APB1 bus (unused for USART1)
	volatile uint32_t MaskAHBENR;          				// Mask to enable RCC AHB bus for GPIO
	volatile uint32_t SerialPinModeValue;  				// Value to configure GPIO pins for Alternate Function mode
	volatile uint32_t SerialPinSpeedValue; 				// Value to configure GPIO pin speed
	volatile uint32_t SerialPinAlternatePinValueLow;  	// AF config (AFRL)
	volatile uint32_t SerialPinAlternatePinValueHigh; 	// AF config (AFRH) (unused for PC10 and PC11)
	void (*completion_function)(uint32_t); 				// Callback after transmission is complete
};

// Instantiated USART1_PORT – GPIOC with TX/RX on PC10, PC11
SerialPort USART1_PORT =
{
		USART1,                    // UART peripheral
		GPIOC,                     // GPIO port
		RCC_APB2ENR_USART1EN,      // USART1 is on APB2
		0x00,                      // APB1 not used for USART1
		RCC_AHBENR_GPIOCEN,        // Enable GPIOC on AHB
		0xA00,                     // MODER value for PC10/11 alternate function
		0xF00,                     // OSPEEDR value for high speed
		0x770000,                  // AFR[0] (low) sets AF7 for PC10 and PC11
		0x00,                      // AFR[1] (high) not used for PC10/11 – value is unused
		0x00                       // Callback initially NULL
};


void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t))
{
	// Stops an infinite loop from happening, very vital line of code
	serial_port->completion_function = completion_function;

	// Enable clock power, system configuration clock and GPIOC (common to all UARTs)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// Enable GPIO port
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// Set GPIO mode and speed for USART pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// Configure alternate function for TX/RX pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// Enable USART clock on respective bus
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Set baud rate (assumes 8 MHz clock)
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate)
	{
		case BAUD_9600:
			*baud_rate_config = 0x341;  // 9600 at 8MHz
			break;
		case BAUD_19200:
			*baud_rate_config = 0x1A1;  // 19200 at 8MHz
			break;
		case BAUD_38400:
			*baud_rate_config = 0xD0;  // 38400 at 8MHz
			break;
		case BAUD_57600:
			*baud_rate_config = 0x8B;  // 57600 at 8MHz
			break;
		case BAUD_115200:
			*baud_rate_config = 0x46;  // 115200 at 8MHz
			break;
	}

	// Enable USART transmitter, receiver and USART peripheral
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}


bool SerialInputAvailable(SerialPort *serial_port)
{
    return (serial_port->UART->ISR & USART_ISR_RXNE);
}


uint8_t SerialReadChar(SerialPort *serial_port)
{
    return serial_port->UART->RDR;
}


void SerialOutputChar(uint8_t data, SerialPort *serial_port)
{
	while((serial_port->UART->ISR & USART_ISR_TXE) == 0)
	{
		// Wait until TX register is empty
	}

	// Transmit the data over the configured uart port
	serial_port->UART->TDR = data;
}


void SerialOutputString(uint8_t *pt, SerialPort *serial_port)
{
	// Start a counter for the number of bytes sent
	uint32_t counter = 0;

	while(*pt)
	{
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}

	// Call optional callback with number of bytes sent
	serial_port->completion_function(counter);
}



