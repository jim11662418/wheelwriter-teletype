// For the Keil C51 compiler.

#ifndef __UART2_H__
#define __UART2_H__

void uart2_init(unsigned long baudrate);
char char_avail2();
char getchar2();
char putchar2(char c);

#endif