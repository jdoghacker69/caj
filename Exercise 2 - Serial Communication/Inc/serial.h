#include <stdint.h>

#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER
#define MAX_BUFFER_LENGTH 128


#define DB_BUFFER_SIZE 256

// Defining the serial port struct, the definition is hidden in the
// c file as no one really needs to know this.
struct _SerialPort;
typedef struct _SerialPort SerialPort;




// make any number of instances of the serial port (they are extern because
//   they are fixed, unique values)
extern SerialPort USART1_PORT;
extern uint8_t start_flag;
// The user might want to select the baud rate
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

 
// SerialInitialise - initialise the serial port
// Input: baud rate as defined in the enum
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t) );

void setupNVIC(void);

void USART1_EXTI25_IRQHandler(void);

// SerialOutputString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void SerialOutputStringDB(uint8_t *pt, SerialPort *serial_port);
 
 
int SerialInputAvailable(SerialPort *serial_port);

uint8_t SerialReadChar(SerialPort *serial_port);

#endif
