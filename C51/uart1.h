// For the Keil C51 compiler.

#ifndef __UART1_H__
#define __UART1_H__
// uart1.c function prototypes...
void uart1_init(unsigned long baudrate);
char char_avail1(void);
char getchar1(void);
char putchar1(char c);
void puts1 (char *s);
#endif