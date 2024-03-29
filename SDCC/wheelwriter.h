// for the Small Device C Compiler (SDCC)

#ifndef __WHEELWRITER_H__
#define __WHEELWRITER_H__

void ww_print_character(unsigned char letter,unsigned char attribute);
void ww_backspace(void);                        
void ww_micro_backspace(void);
void ww_space(void);
void ww_carriage_return(void);
void ww_spin(void);
void ww_horizontal_tab(unsigned char spaces);
void ww_erase_letter(unsigned char letter);
void ww_linefeed(void);
void ww_reverse_linefeed(void);
void ww_paper_up(void);                    
void ww_paper_down(void);
void ww_micro_up(void);
void ww_micro_down(void);
char ww_decode_keys(unsigned int WWdata);
void ww_reset(char board);

#endif

