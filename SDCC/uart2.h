// for the Small Device C Compiler (SDCC)

#ifndef __UART2_H__
#define __UART2_H__

void uart2_isr(void) __interrupt(8) __using(3);
void uart2_init(unsigned long baudrate);
char char_avail2(void);
char getchar2(void);
char putchar2(char c);

#endif
