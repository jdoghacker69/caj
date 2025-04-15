/**
 * @file serial_port.h
 * @brief Serial communication module for STM32F3 using USART peripherals.
 *
 * This module provides an abstraction layer for serial communication using the USART interfaces
 * on the STM32F3 Discovery board. It defines a `SerialPort` structure to represent serial ports,
 * and provides functions for initialising a USART, sending and receiving characters, and
 * transmitting null-terminated strings.
 *
 * The user can select from a predefined set of baud rates and assign callback functions to be
 * invoked after serial initialisation completes. It also exposes a global instance for USART1.
 */

#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER


#include <stdint.h>
#include <stdbool.h>


// Struct to define a serial port configuration
struct _SerialPort;
typedef struct _SerialPort SerialPort;

// Instantiate USART1 as a serial port using the SerialPort struct
extern SerialPort USART1_PORT;

// Option for user the select the baud rate that gets input into 'SerialInitialise'
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};


// Initialises USART with GPIO and baud rate
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t) );
 
// Check if data is available in RX buffer
bool SerialInputAvailable(SerialPort *serial_port);

// Read a single character from RX register
uint8_t SerialReadChar(SerialPort *serial_port);

// Transmit a single character to the serial port (this version waits until the port is ready)
void SerialOutputChar(uint8_t, SerialPort *serial_port);

// Transmits a NULL TERMINATED string to the serial port
void SerialOutputString(uint8_t *pt, SerialPort *serial_port);
 

#endif
