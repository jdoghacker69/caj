# Eliza's Circuit Breakers/Cajun Circuits

## **MTRX2700 Microcontrollers in C**  

### **Group Members and Roles**
| Name  | Role | Responsibilities |
|--------|------|----------------|
| Jasper | Serial Communication Lead | Development of UART communication functions in Exercises 2 & 4; interrupt handling for reception/transmission |
|        | Integration Co-Lead | Combining modules into a final system; parser implementation for serial commands |
| Connor | Hardware Timers Lead | Design and implementation of timer callbacks, regular/one-shot timer operations |
|        | Integration Co-Lead | Managing concurrent events in integration task, timer control via serial input |
| Aurora | Digital I/O Lead | Button debounce handling, LED state control, modular access functions |
|        | Integration Co-Lead | Coordination of I/O and serial events, LED output based on command strings |

---

## **Project Overview**
This STM32-based embedded systems project consists of four modular exercises designed to demonstrate structured programming in C, real-time hardware interfacing, and event-driven software using interrupts and timers. Each module was written to be independent, configurable via header APIs and driven by interrupts and callbacks.

Modules:
1. Digital I/O: Encapsulates LED/button functionality using a debounced button and LED state controller.
2. UART Communication: Enables string-based transmission and reception using interrupts and memory buffers.
3. Hardware Timer Interface: Supports periodic and one-shot callbacks using configurable timer events.
4. Integration Task: Parses serial commands to control LEDs, echo messages, and manage timers in real time.

The following flowchart diagrams give an overview of the integration exercise that uses all modules as submodules:

[insert flowcharts here once made in draw.io]

---

## **Installation & Usage Instructions**
To run each of the modules, the **STM32CubeIDE** and a software to communicate to the boards must be installed. 

| Computer  | Communication Software |
|--------|------|
| Linux/macOS | CuteCom |
| Windows | PuTTY |

An STM32F3 Discovery Board must then be connected via USB, and the correct port terminal opened using the CuteCom/PuTTY interface. Make sure the following CuteCom/PuTTy variables are set to:

| Setting  | Value |
|--------|------|
| Baud rate | 115200 |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | none |

This git repository must be cloned onto your device. Open STM32CubeIDE and import the project folder as an existing STM32 project.
, and a workspace opened in STM32CubeIDE to run each of the separate modules. The testing procedure and expected output of each program is outlined in the following section.

---

## **Exercise 1 - LED control and button handling**

### Summary
This module handles user input via a button and controls the onboard LEDs. A function pointer allows users to register custom behaviour on button press. LED states are managed internally and can only be modified through getter/setter functions exposed in the header file.


### Usage
Once the code has been downloaded onto the board, the user can press the button and expect an LED to light up. Subsequent button presses will cause the same LED to switch off and the next LED to light up. The LEDs will light up one at a time in a clockwise direction around the board. However, the button can not be spammed, as there is a short timer that prevents the button press being recognised until its up, so the button can only update the LED once every 2 seconds. 

To adjust the starting LED go to the ‘diode.c’ file and change the 'led_state' variable to whichever hex number that represents the LED you want to be the first switched on. The default value is zero, so there will initially be no LED on but if the value was '0x02' for example, the second LED (red) will be on at startup. 

To change the time allowed between button presses go to the 'timer.c' file and adjust the auto-reload register (ARR) of the timer TIM2, 'TIM2->ARR', to your desired time buffer. It's default is 2000 ms.


### Functions and modularity
The diode module is isolated to the 'diode.c/h' files, making it easy to integrate into other projects. The following is a description on what each function does and how they're dependent on each other. Note that the 'button_callback' function is defined in the 'main.c' file using the 'my_button_handler' function. 

#### void my_button_handler(void)
- Button interrupt handler.
- This function is called by the 'EXTI0_IRQHandler' function when the button is pressed.
- It calls dio_chase_led() to shift the LED if the timer allows.

#### void dio_init(void (*button_callback)(void));
- Initialises GPIO for LEDs and button.
- Registers the provided button callback function
- Configures PE8–PE15 as output pins

#### void dio_set_leds(uint8_t pattern);
- Updates the output state of LEDs on PE8–PE15.
- Takes an 8-bit pattern, where bit 0 corresponds to PE8.

#### uint8_t dio_get_leds(void);
- Returns the current 8-bit LED pattern (PE8–PE15)

#### void dio_chase_led(void);
- Shifts the active LED one position to the left but only allows updates when the timer grants permission.
- Wraps around when it overflows.

#### void EXTI0_IRQHandler(void);
- EXTI0 interrupt handler.
- Called when button on PA0 is pressed.

The timer module is isolated to the 'timer.c/h' files, making it easy to integrate into other projects. The following is a description on what each function does and how they're dependent on each other.

#### void timer_init(void);
- Sets up interrupts and timer.
- Sets up EXTI0 for rising edge interrupts on PA0
- Initialises TIM2 for periodic updates

#### void timer2_init(void);
- The timer2 function initialises TIM2 with preset values (10kHz base, 200ms interval)

#### void timer2_set_callback(void (*callback)(void));
- The timer2_set_callback function registers a callback to be called on every TIM2 update event


### Testing
| Test Cases | Expected Output | Observed behaviour |
|------------|-----------------|--------------------|
|Button press every 2 seconds|The LED's lit up consectutively in a clockwise direction once every 2 seconds| ...|
|Reset button press|The program reset and the chase pattern began again at the initial LED| ... |
|Several button presses in quick succession|The timer works and the LED's update once every 2 seconds| ... |


### Notes

---

## **Exercise 2 - String transmission, reception, and port forwarding**

<details>
  <summary><strong>Summary</strong></summary>

This module demonstrates UART serial communication on an STM32 microcontroller using interrupts. The system continuously receives characters into a buffer until a terminating character (newline or carriage return) is encountered. Once a complete string is assembled, the code echoes it back via UART and triggers a user-defined callback with a pointer to the received string and the transmitted byte count. Additionally, a basic double-buffering scheme for non-blocking serial transmission is implemented, allowing new data to be queued during an ongoing transmission, additionally saving space and processing power.

</details>

<details>
  <summary><strong>Usage</strong></summary>

#### Valid Input
- **Strings:** Input strings must be terminated by a newline (`\n`) or carriage return (`\r`).
- **Example:** Sending `"start\r\n"` is recognized as a complete command and will trigger the special processing routine.

#### How to Use:
1. **Build and Flash the Code:**
   - Import the project (including `main.c`, `serial.c`, `serial.h`, and supporting files) into STM32CubeIDE.
   - Compile the project for the STM32F303 microcontroller.
   - Flash the code to the target board using ST-Link or your preferred programmer.

2. **Terminal Setup:**
   - Connect the STM32F303 Discovery Board via USB.
   - Open your terminal emulator - CuteCom for Linux/macOS and PuTTY for Windows using the following settings:

     | **Setting**  | **Value**  |
     |--------------|------------|
     | Baud Rate    | 115200     |
     | Data Bits    | 8          |
     | Stop Bits    | 1          |
     | Parity       | None       |

3. **Operation:**
   - The module echoes received strings over UART using a double-buffered transmit routine.
   - If the received string matches `"start"`, a special message is output and the example code shape demo is triggered.

</details>

<details>
  <summary><strong>Core Functions and Modularity</strong></summary>

#### Key Functions

##### `SerialInitialise()`
Initialises the USART peripheral and associated GPIOs.

> **Function Prototype:**  
> `void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t));`

> **Code : initialisation function:**
> ```c
> void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint32_t)) {
>     serial_port->completion_function = completion_function;
> 
>     // Enable essential clocks
>     RCC->APB1ENR |= RCC_APB1ENR_PWREN;
>     RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
>     RCC->AHBENR  |= RCC_AHBENR_GPIOCEN;
>     RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
> 
>     // Configure GPIO pins for alternate function mode
>     RCC->AHBENR |= serial_port->MaskAHBENR;
>     serial_port->GPIO->MODER = serial_port->SerialPinModeValue;
>     serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;
>     serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
>     serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;
> 
>     // Enable USART interrupts (RXNE and TXE)
>     serial_port->UART->CR1 |= USART_CR1_RXNEIE;
>     serial_port->UART->CR1 |= USART_CR1_TXEIE;
> 
>     // Enable additional peripheral clocks if needed
>     RCC->APB1ENR |= serial_port->MaskAPB1ENR;
>     RCC->APB2ENR |= serial_port->MaskAPB2ENR;
> 
>     // Configure baud rate (placeholder values; update as needed)
>     uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR;
>     switch (baudRate) {
>         case BAUD_115200:
>             *baud_rate_config = 0x46;  // 115200 at 8MHz
>             break;
>         // Other baud rates: NEED TO FIX THIS!
>         default:
>             *baud_rate_config = 0x46;
>     }
> 
>     // Enable transmitter, receiver, and USART
>     serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
> }
> ```

---

##### `USART1_EXTI25_IRQHandler()`
Handles UART interrupts for both RX and TX. This function:
- Reads received characters into a buffer until a terminating character is detected.
- Echoes the complete string back over UART.
- Checks for the special command `"start"` and sets a flag to trigger further processing.
- Manages double-buffered transmission.

> **Code Snippet (Reception & Command Processing):**
> ```c
> void USART1_EXTI25_IRQHandler(void) {
>     // Check for framing or overrun errors and clear them
>     if ((USART1->ISR & USART_ISR_FE_Msk) || (USART1->ISR & USART_ISR_ORE_Msk)) {
>         USART1->ICR = USART_ICR_FECF | USART_ICR_ORECF;
>         return;
>     }
> 
>     // RXNE: Process received characters
>     if (USART1->ISR & USART_ISR_RXNE) {
>         while (SerialInputAvailable(&USART1_PORT)) {
>             uint8_t c = SerialReadChar(&USART1_PORT);
>             if (string_index < MAX_BUFFER_LENGTH - 1) {
>                 received_string[string_index++] = c;
>                 if (c == '\n' || c == '\r') {
>                     received_string[string_index - 1] = '\0';
>                     input_received_flag = 1;
>                 }
>             } else {
>                 string_index = 0;
>             }
>         }
>     }
> 
>     // Process a complete string if available
>     if (input_received_flag == 1) {
>         SerialOutputStringDB((uint8_t *)received_string, &USART1_PORT);
>         if (is_start_command(received_string) == 1) {
>             uint8_t response[] = "\n--SECRET-- Command START received.\n";
>             SerialOutputStringDB(response, &USART1_PORT);
>             start_flag = 1;
>         }
>         string_index = 0;
>         input_received_flag = 0;
>         memset((void*)received_string, 0, MAX_BUFFER_LENGTH);
>     }
> 
>     // TX interrupt handling (double-buffered) is managed below.
> }
> ```

---

##### `SerialOutputStringDB()`
Implements the double-buffered transmit mechanism. It:
- Appends the string to a fill buffer.
- If no transmission is active, swaps the fill and transmit buffers.
- Initiates transmission by enabling the TXE interrupt.
- Ensures new data is queued without blocking.

> **Code Snippet:**
> ```c
> void SerialOutputStringDB(uint8_t *pt, SerialPort *serial_port) {
>     __disable_irq();
> 
>     // Append data into the fill buffer until termination or full buffer
>     while (*pt && fill_index < (DB_BUFFER_SIZE - 1)) {
>         fill_buffer[fill_index++] = *pt++;
>     }
>     fill_buffer[fill_index] = '\0'; // Null-terminate the fill buffer
> 
>     // If not transmitting, swap buffers and start transmission
>     if (!is_transmitting) {
>         uint8_t *temp = fill_buffer;
>         fill_buffer = transmit_buffer;
>         transmit_buffer = temp;
>         transmit_length = fill_index;
>         fill_index = 0;
>         transmit_index = 0;
>         is_transmitting = 1;
>         serial_port->UART->CR1 |= USART_CR1_TXEIE;
>     }
> 
>     __enable_irq();
> }
> ```

</details>

<details>
  <summary><strong>Testing</strong></summary>

#### Test Cases

| **Test Case**                               | **Expected Output**                                                              | **Observed Behaviour**                                          |
|---------------------------------------------|----------------------------------------------------------------------------------|-----------------------------------------------------------------|
| **Transmit Single Character**               | Each character transmits without delay.                                          | Verified via oscilloscope/terminal output.                      |
| **Receive Complete String ("Hello\r\n")**   | The complete string `"Hello"` is echoed over UART.                               | String correctly received and echoed.                           |
| **Command Detection ("start\r\n")**         | Outputs `"--SECRET-- Command START received."` and triggers demo functionality.   | Command correctly recognized; demo executed.                    |
| **Double-Buffered Transmission**            | New messages are queued and transmitted without blocking the ongoing transmission.| Data successfully queued and transmitted in sequence.           |
| **Error Handling (Framing/Overrun)**        | Clears error flags and maintains system responsiveness.                          | No unexpected faults; error conditions handled gracefully.      |

</details>

## **Exercise 3 - Implementing precise delays and event handling**

### Summary
A timer module that provides a hardware timer interface that is capable of triggering periodic and oneshot events with the use of function pointers in callback functions. This module is designed to run independently, and to be attached on to other modules. It utilises two STM32F303 hardware timers to schedule and trigger code at inputted intervals.
- TIM2 is used for periodic callbacks. These are events that should occur repeatedly, such as blinking leds, or updating a variable.
- TIM3 is used for oneshot callbacks. These are events that should only occur once, such as triggering an LED after a specified delay, or responding to an input.
Both of these timers have been configured to run on millisecond precision and use internal state and callback function pointers, which makes them modular and easy to coalesce.

### Usage
The code must be downloaded and integrated into the workspace of the main project. Then the timer module must be set up. Both of the timer peripherals must be enabled.
- enable_periodic_clock();
- enable_oneshot_clock()

After initialising the module, the functions may be called using their respective functions:
- timer_init(ms, callback);
- timer_oneshot(ms, callback);

Where "callback" should be replaced with the function you wish to call. (E.g. led_flash.)

The time between function triggers can be adjusted by changing uint32_t ms value that inputted into the periodic and oneshot timers. This module can be combined with a module that utilises serial ports and usart/uart to allow the user to input a sentence that is read and translated into a value that can be passed through the periodic and oneshot timers as a millisecond value.

Similarly, the callback function can be replaced with any function that the user wishes to run after a delay.

### Valid input
- any positive integer delay value for ms, noting that it is taken in milliseconds.
- any valid function pointer of type void (*callback)(void).

### Functions and modularity
The timer module is isolated to the timer_module.c/h files. Independent timers are used for the periodic and oneshot modes. Key functions include:

static void trigger_prescaler(TIM_TypeDef *TIMx)
- Forces the PSC and ARR values to be updated, using the UG bit.

static uint32_t enqueue_oneshot(uint32_t delay_ms, void(*callback)(void))
- Checks that there is room inside the buffer, if there is, the oneshot event is added to the end of the queue.

static uint32_t dequeue_oneshot(OneShotEvent *event)
- Checks that there is an item queued, if there is, it is pushed to run and the queue head is moved to the next in line.

void enable_periodic_clock(void)
- Enables the peripheral clock for the periodic function; Enables TIM2.

void enable_oneshot_clock(void)
- Enables the peripheral clock for the oneshot function; Enables TIM3.

void timer_init(uint32_t ms, void (*callback)(void))
- Initialises TIM2 using a user-defined interval which is given in milliseconds.
- Triggers the user-defined function every given interval.

void timer_oneshot(unit32_t ms, void (*callback)(void))
- Initialises TIM3 using a user-defined interval which is given in milliseconds.
- Triggers the user-defined function once after the given interval.

void TIM2_IRQHandler(void)
- Handles the interrupt process for TIM2.
- Elicits the user-defined periodic callback function.

void TIM3_IRQHandler(void)
- Handles the interrupt process for TIM3.
- Elicits the user-defined oneshot callback function.

### Testing
| Test Cases | Expected Output | Observed behaviour |
|------------|-----------------|--------------------|
|timer_init(1000, led_blink_sequence)|The lit-up LED should circle around the board, updating every one second.|The lit-up LED circled around the board, updating every one second.|
|timer_oneshot(4000), led_flash)|All the LEDs should flash once after four seconds.|The LEDs all flashed only once after four seconds.|

### Notes
- Only one active periodic and eight active oneshot callback can be called at a time.
- Future improvements could include more timers to allow for more periodic calls.

---

## **Exercise 4 - Integration**

### Summary
The integration exercise brings together the functionality of the previous modules—Digital I/O, Serial Communication, and Timer Interface—into a complete real-time embedded system. The core loop of the program constantly checks for incoming serial input using UART and processes commands in real-time without blocking delays. These commands can control LED patterns, print messages over serial, or trigger timed operations using the hardware timers. Each module remains self-contained and communicates through clear APIs and function pointers to maintain modularity.

The system reacts to complete input lines terminated by newline (\n) or carriage return (\r) characters. Once received, the string is parsed and dispatched using handleCommand() (from user_command.c), which interprets the command and calls the appropriate module-level function. This design allows for concurrent operations like LED updates and timed callbacks while still accepting user input in real time.

### Usage
Once flashed onto the STM32F3 board, the system is ready to accept user commands over UART1. You can use CuteCom (Linux/macOS) or PuTTY (Windows) to open a terminal at the settings specified in the **Installation & Usage Instructions** section. 

If needed, the baud rate can be changed in main.c by modifying the global baud_rate variable before calling SerialInitialise().

To send a command, type your string in the terminal and press Enter. The system will respond based on the input and execute the corresponding action.

### Functions and modularity

### Testing
The user command module has been rigorously tested with the following valid input:

| Valid Test Cases | Expected Output | Output Met Requirements |
|------------------|-----------------|-------------------------|
| "led 10011001" | Lights up the LEDs in the specified pattern | Yes |
| "serial This is a message" | The string "This is a message" gets printed on the serial monitor output | Yes |
| "serial 123i9013i0$W%T%" | The string correctly gets printed on the serial monitor output | Yes |
| "oneshot 2000" | Triggers the LED's to flash 2 seconds later | Yes |
| "oneshot 0" | Triggers the LED's to not flash | Yes |
| "timer 3000" | Triggers the LED's to flash once every 3 seconds | Yes |
| "timer 0" | Triggers a debugging LED flash to warn of no period | Yes |

The module was then tested with the following edge cases and invalid input to ensure the program exits gracefully when encountering an error:

| Invalid Test Cases | Expected Output | Output Met Requirements |
|--------------------|-----------------|-------------------------|
| "led" | Print error message to structure the command properly | Yes |
| "serial" | Print error to add a message after serial command | Yes |
| "oneshot" | Print error message to structure the command properly | Yes |
| "timer" | Print error message to structure the command properly | Yes |
| "1000 timer" | Print error message for unknown command | Yes |
| "dfnakfwaojo" | Print error message for unknown command | Yes |
| "timer sdasdfsd" | Triggers a debugging LED flash to warn of no period | Yes |

---

## **Acknowledgments**
- Dr. Stephany Berrio & Stewart Worrall (University of Sydney) for lab guidance  
- STM32F3 Discovery Board documentation  

---
