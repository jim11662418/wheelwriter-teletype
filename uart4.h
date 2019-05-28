void uart4_init(void);
void uart4_send_ACK(void); 
void uart4_send(unsigned int wwCommand);
void uart4_send_wait(unsigned int wwCommand);
char uart4_avail(void);
unsigned int uart4_get_data(void);

