// For the Keil C51 compiler.

#ifndef __UART4_H__
#define __UART4_H__

void uart4_init(void);
void send_to_printer_board(unsigned int wwCommand);
void send_to_printer_board_wait(unsigned int wwCommand);
char printer_board_reply_avail(void);
unsigned int get_printer_board_reply(void);

#endif