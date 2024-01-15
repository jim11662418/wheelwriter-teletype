// for the Small Device C Compiler (SDCC)

#ifndef __UART1_H__
#define __UART1_H__

void uart1_isr(void) __interrupt(4) __using(2);
void uart1_init(unsigned long baudrate);
char char_avail1(void);
char getchar1(void);
char putchar1(char c);
void puts1 (char *s);
#endif
