// For the Keil C51 compiler.

#ifndef __UART3_H__
#define __UART3_H__

void uart3_init(void);
void send_ACK_to_function_board(void);
void send_to_function_board(unsigned int wwCommand);
char function_board_cmd_avail(void);
unsigned int get_function_board_cmd(void);

#endif
