# MPU6050 Driver Development for LPC Microcontrollers

This project is a structured development of drivers for the MPU6050 sensor, targeting various LPC architectures and driver models (Blocking, Non-Blocking, FreeRTOS).

---

##  Project Structure

```
├── Development 
|
├── MPU6050 Driver 
|   |
|   ├── MPU6050Blocking [LPC2148] 
|   |
|   ├── MPU6050NonBlocking [LPC2148] 
|   | 
|   └── MPU6050FreeRTOS [LPC2294] 
|
└── WhoAmITest 
    ├── WhoAmITest (Blocking) [LPC2148] 
    ├── WhoAmITest (NonBlocking) [LPC2148] 
    └── WhoAmITest (FreeRTOS) [LPC2294] 
``` 

---

##  Project Overview

The project includes MPU6050 sensor drivers implemented and tested using three different approaches:

- **Blocking** — Simple implementation for LPC2148.
- **Non-Blocking** — Avoids CPU stalling, using LPC2294 for better performance.
- **FreeRTOS** — Multi-tasking support under FreeRTOS for LPC2294.

---

## Features

- Real-time data acquisition from the MPU6050 sensor.
- Outputs accelerometer, gyroscope, and temperature data to a PC terminal.
- Configurable UART communication.
- Compatible with LPC2148 and LPC2294 microcontrollers.
- Debugging support using Digital Logic Analyzer (recommended: Saleae Logic Pro 16).

---

##  Getting Started

### Prerequisites

- LPC2148 or LPC2294 microcontroller
- MPU6050 sensor module
- Serial terminal 
- Digital Logic Analyzer (optional, for debugging)
- IDE supporting ARM LPC (e.g., Keil uVision, CLion)

---

##  Documentation

Refer to the following documents for deeper hardware and firmware understanding:

- LPC2148/2294 datasheet
- UM10139 LPC214x User Manual
- lpc-ARM-book_srn

---

## Usage

Clone the repo:

```bash
git clone https://github.com/VladimirTM/ProiectDSPLabs.git
```

### Steps:

    
- Open the desired project folder in your IDE.

- Power on the LPC2148/2294 board with the MPU6050 connected.

- Ensure pull-up resistors are in place.

- Open your terminal and connect to the correct COM port (e.g., 115200 baud).

- Start the program to begin real-time data output.

