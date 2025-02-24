# **Project Description**
This project involves creating a driver to interface with the **MPU6050** sensor, which reads accelerometer, gyroscope, and temperature data. The data is transmitted to a PC terminal via UART0 of the LPC2148 microcontroller.

## **Status**: This project is currently under development. At this stage:

* The driver is implemented in **blocking form**.
* We have configured the **WhoAmI** register with **interrupts**.
* The driver in **Final State Machine form** for Real-Time usage is planned but not yet completed.

## **Features**
* Real-time data acquisition from the MPU6050 sensor.
* Outputs accelerometer, gyroscope, and temperature data to a PC terminal.
* Configurable UART communication for data transmission.
* Compatibility with LPC2148 microcontroller.
* Debugging support via Digital Logic Analyzer (recommended: Saleae Logic Pro 16).

## **Getting Started**

**Prerequisites**
* LPC2148 microcontroller.
* MPU6050 sensor module.
* Digital Logic Analyzer (optional for debugging).
* Development environment supporting ARM LPC2148 (e.g., Keil uVision, or any other ARM-compatible IDE).
  
**Documentation**
For detailed information on hardware and firmware configuration, please consult:
* LPC2141/42/44/46/48 datasheet
* UM10139 LPC214x User manual
* lpc-ARM-book_srn

**Usage**
Power on the LPC2148 board with the MPU6050 connected (don't forget about the pullup resistors)
Open your terminal software and connect to the appropriate COM port at the configured baud rate (e.g., 9600 baud).
Start the program to begin data transmission from the MPU6050. Data will display in real-time on the terminal.

**Debugging**
Using a Digital Logic Analyzer is recommended for debugging. We used the Saleae Logic Pro 16, which helped us capture and analyze I2C signals for effective troubleshooting.

## **Authors**
This project was developed under the guidance of Valentin Stangaciu at DspLabs.

Project Authors:

* Ghimpau Mihai-Vladimir
* Sztanarec Adrian-Gabriel

## **Acknowledgments**

DspLabs for providing resources and guidance.

Valentin Stangaciu for mentorship throughout the project.

## **Contact**
For inquiries, please reach out via GitHub.

