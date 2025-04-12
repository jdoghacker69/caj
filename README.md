# Eliza's Circuit Breakers/Cajun Circuits

## **MTRX2700 Microcontrollers in C**  

### **Group Members and Roles**
| Name  | Role | Responsibilities |
|--------|------|----------------|
| Jasper | Serial Communication Lead | Development of UART communication functions Parts A, C, E |
|        | Integration Co-Lead | Integrating the ... |
| Connor | Hardware Timers Lead | Timer-based operations and integration |
|        | Integration Co-Lead | Integrating the ... |
| Aurora | Digital I/O Lead | Implementation of LED and button control functions |
|        | Integration Co-Lead | Integrating the ... |

---

## **Project Overview**
This project involves developing and testing an **STM32 c language program** to perform various low-level microcontroller operations. The code is structured into modules, each focusing on different functionalities of the STM32:

1. **Digital I/O** – LED control and button handling
2. **Serial Communication (UART)** – String transmission, reception, and port forwarding
3. **Hardware Timers** – Implementing precise delays and event handling
4. **Integration** – Combining all components to demonstrate an end-to-end working system

The following flowcharts for each part depict the flow...

[insert flowcharts here once made in draw.io]

---

## **Installation & Usage Instructions**
To run each of the modules, the **STM32CubeIDE** and a software to communicate to the boards must be installed. 

| Computer  | Communication App |
|--------|------|
| Mac | CuteCom |
| Windows | PuTTY |

An STM32F3 Discovery Board must then be connected via USB, and the correct port terminal opened using the CuteCom/PuTTY interface. Make sure the following ... are set to:

| Setting  | ... |
|--------|------|
| Baud rate | 115200 |
| Data bits | 8 |
| Stop bits | 1 |
| Parity | none |

This git repository must be cloned onto your device, and a workspace opened in STM32CubeIDE to run each of the separate modules. The testing procedure and expected output of each program is outlined in the following section.

---

## **Exercise 1 - LED control and button handling**

### Summary

### Usage

To adjust the starting LED

### Functions and modularity

### Testing (valid input aswell)

### Notes

---

## **Exercise 2 - String transmission, reception, and port forwarding**

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes

---

## **Exercise 3 - Implementing precise delays and event handling**

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes

---

## **Exercise 4 - Integration**

### Summary

### Usage

### Valid input

### Functions and modularity

### Testing

### Notes

---

## **Acknowledgments**
- Dr. Stephany Berrio & Stewart Worrall (University of Sydney) for lab guidance  
- STM32F3 Discovery Board documentation  

---
