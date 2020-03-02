// Interrupt driven UART4 functions
// UART4 (for the Printer Board) uses a receive buffer in internal MOVX SRAM.
// UART4 uses the Timer 4 for baud rate generation. init_uart4 must be called 
// before using functions. No syntax error handling. No handshaking.

#include <reg51.h>
#include <stc51.h>

#define FALSE 0
#define TRUE  1

#define RBUFSIZE4 16
#if RBUFSIZE4 < 4
#error RBUFSIZE4 is too small. RBUFSIZE4 may not be less than 4.
#elif RBUFSIZE4 > 128
#error RBUFSIZE4 is too large. RBUFSIZE4 may not be greater than 128.
#elif ((RBUFSIZE4 & (RBUFSIZE4-1)) != 0)
#error RBUFSIZE4 must be a power of 2.
#endif

volatile unsigned char data rx4_head;       	// receive interrupt index for serial 1
volatile unsigned char data rx4_tail;       	// receive read index for serial 1
volatile unsigned int xdata rx4_buf[RBUFSIZE4]; // receive buffer for serial 1 in internal MOVX RAM
volatile bit tx4_ready;                         // set when ready to transmit
sbit WWbus4 = P0^2;		  			            // P0.2, (RXD4, pin 3) used to monitor the Wheelwriter BUS

// ---------------------------------------------------------------------------
// UART4 interrupt service routine
// ---------------------------------------------------------------------------
void uart4_isr(void) interrupt 18 using 3 {
	unsigned int wwBusData;

    // UART4 transmit interrupt
    if (S4TI) {                                 // transmit interrupt?
	   CLR_S4TI;                                // clear transmit interrupt flag
	   tx4_ready = TRUE;						// transmit buffer is ready for a new character
    }

    if(S4RI) {                         	        // receive interrupt?
       CLR_S4RI;                                // clear receive interrupt flag
       wwBusData = S4BUF;                       // retrieve the lower 8 bits
       if (S4RB8) wwBusData |= 0x0100;          // ninth bit is in S3RB8
       rx4_buf[rx4_head++ & (RBUFSIZE4-1)] = wwBusData;  // save it in the buffer
    }   
}

// ---------------------------------------------------------------------------
//  Initialize UART4 for mode 3. Mode 3 is an asynchronous mode that transmits and receives
//  a total of 11 bits: 1 start bit, 9 data bits, and 1 stop bit. The ninth bit to be 
//  transmitted is determined by the value in S3TB8 (S3CON.3). When the ninth bit is received, 
//  it is stored in S3RB8 (S3CON.2). The baud rate is determined by the T4 overflow rate.
//  The formula for calculating the UART4 baud rate is: baud rate = (T4 overflow)/4.
//  If T4 is operating in 1T mode (T4x12=1), the baud rate of UART4 = SYSclk/(65536-[T4H,T4L])/4.
// ---------------------------------------------------------------------------
void uart4_init(void) {
    rx4_head = 0;                   			// initialize UART4 buffer head/tail pointers.
    rx4_tail = 0;
    
    SET_S4ST4;                                  // set S4ST4 to select Timer 4 as baud rate generator for UART3.
    CLR_T4_CT;                                  // clear T2_C/T to make Timer 2 operate as timer instead of counter 
    SET_T4x12;                                  // set T2x12=1 to make Timer 2 operate in 1T mode.
    T4L = 0XF0;                                 // the baud rate of UART3 = 12MHz/(65536-65520)/4 = 187500 bps
    T4H = 0XFF;
    SET_T4R;                                    // set T2R to enable Timer 2 to run

    SET_S4SM0;                                  // set S4SM0 for mode 3
    SET_S4REN;                    			    // set S4REN to enable reception
    SET_S4TI;                                   // set S4TI to enable transmit
    CLR_S4RI;                                   // clear S4RI 
    SET_ES4;                    			    // set ES4 to enable UART4 serial interrupt
    EA = TRUE;                     				// enable global interrupt
}

// ---------------------------------------------------------------------------
// sends an unsigned integer as 11 bits (start bit, 9 data bits, stop bit)
// to the Printer Board. waits for acknowledge from printer board.
// ---------------------------------------------------------------------------
void send_to_printer_board_wait(unsigned int wwCommand) {
   while (!tx4_ready);		 					// wait until transmit buffer is empty
   tx4_ready = 0;                               // clear flag   
   while(!WWbus4);                              // wait until the Wheelwriter bus goes high
   CLR_S4REN;                                   // clear S4REN to disable reception
   if (wwCommand & 0x100) SET_S4TB8; else CLR_S4TB8;
   S4BUF = wwCommand & 0xFF;                    // lower 8 bits
   while(!tx4_ready);                           // wait until finished transmitting
   while(!WWbus4);                              // wait until the Wheelwriter bus goes high
   while(WWbus4);                               // wait until the Wheelwriter bus goes low (acknowledge)
   while(!WWbus4);                              // wait until the Wheelwriter bus goes high again
   SET_S4REN;                    		        // set S4REN to re-enable reception
}

// ---------------------------------------------------------------------------
// sends an unsigned integer as 11 bits (start bit, 9 data bits, stop bit)
// to the Printer Board. does not wait for acknowledge from printer board.
// ---------------------------------------------------------------------------
void send_to_printer_board(unsigned int wwCommand) {
   while (!tx4_ready);		 					// wait until transmit buffer is empty
   tx4_ready = 0;                               // clear flag   
   while(!WWbus4);                              // wait until the Wheelwriter bus goes high
   CLR_S4REN;                                   // clear S4REN to disable reception
   if (wwCommand & 0x100) SET_S4TB8; else CLR_S4TB8;
   S4BUF = wwCommand & 0xFF;                    // lower 8 bits
   while(!tx4_ready);                           // wait until finished transmitting
   while(!WWbus4);                              // wait until the Wheelwriter bus goes high
   SET_S4REN;                    		        // set S4REN to re-enable reception
}

// ---------------------------------------------------------------------------
// returns 1 if there is an unsigned integer from the Printer Board waiting in the UART4 receive buffer.
// ---------------------------------------------------------------------------
char printer_board_reply_avail(void) {
   return (rx4_head != rx4_tail);               // not equal means there's something in the buffer
}

//----------------------------------------------------------------------------
// returns the next unsigned integer from the Printer Board in the UART4 receive buffer.
// waits for an integer to become available if necessary.
//----------------------------------------------------------------------------
unsigned int get_printer_board_reply(void) {
    unsigned int buf;

    while (rx4_head == rx4_tail);     			// wait until a word is available
    buf = rx4_buf[rx4_tail++ & (RBUFSIZE4-1)];  // retrieve the word from the buffer
    return(buf);
}

