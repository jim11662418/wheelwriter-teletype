# IBM Wheelwriter "Teletype"
This project uses an STCmicro [STC15W4K32S4](https://www.stcmicro.com/datasheet/STC15W4K32S4-en.pdf) microcontroller (an Intel 8052-compatible MCU) to turn an IBM Wheelwriter Electronic Typewriter into a teletype-like device. This project only works on earlier Wheelwriter models, the ones that internally have two circuit boards: the Function Board and the Printer Board (Wheelwriter models 3, 5 and 6).

The ribbon cable that normally connects the Wheelwriter's Function and Printer boards is disconnected and the STC15W4K32S4 MCU is interposed instead between the two boards. (see diagram below) The MCU uses one of its four UARTs to communicate with the Wheelwriter's Function Board and a second UART to communicate with the Printer Board. See the schematic for details.

The MCU intercepts commands generated by the Function Board when keys are pressed, and converts these commands into ASCII characters that correspond to the keys pressed. In the default "typewriter" or "local" mode, the MCU sends the characters to the Printer Board for printing and so the Wheelwriter acts as a normal typewriter. In the "keyboard" or "line" mode, the MCU sends characters through the console serial port to the host computer. In “keyboard” mode the characters from the Function Board are not sent to the Printer Board, so no characters are printed when keys are pressed. At any time, ASCII characters received from the host computer by the MCU through the console serial port are converted into commands and sent to the Printer Board. Thus, the Wheelwriter acts as a serial printer.

The object file (in Intel HEX format) is also provided for those who don't have access to the Keil C51 compiler.
<p align="center"><img src="/images/Wheelwriter Interface.JPEG"/>
<p align="center"><img src="/images/Schematic-1.png"/>
<p align="center"><img src="/images/IAP15W4K61S4 Connection.png/">
