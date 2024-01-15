// for the Small Device C Compiler (SDCC)

#ifndef __UART3_H__
#define __UART3_H__

void uart3_isr(void) __interrupt(17) __using(3);
void uart3_init(void);
void send_ACK_to_function_board(void);
void send_to_function_board(unsigned int wwCommand);
char function_board_cmd_avail(void);
unsigned int get_function_board_cmd(void);

#endif

