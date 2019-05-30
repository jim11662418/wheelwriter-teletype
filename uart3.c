//************************************************************************//
// UART3 functions for connecting with the Wheelwriter Function Board     //
//                                                                        //
// Interrupt driven UART3 functions. UART3 uses a receive buffer in       //
// internal MOVX SRAM. UART3 uses the Timer 3 for baud rate generation.   //
// init_uart3 must be called before using functions. No syntax error      //
// handling. No handshaking. RxD3 on pin 1, TxD3 on pin 2                 //
//************************************************************************//

#include <reg51.h>
#include "stc51.h"

#define FALSE 0
#define TRUE  1

#define RBUFSIZE3 16
volatile unsigned char data rx3_head;       	// receive interrupt index for UART3
volatile unsigned char data rx3_tail;       	// receive read index for UART3
volatile unsigned int xdata rx3_buf[RBUFSIZE3]; // receive buffer for UART3 1 in internal MOVX RAM
volatile bit tx3_ready;                         // set when ready to transmit
sbit WWbus3 = P0^0;		  			            // P0.0, (RXD3, pin 1) used to monitor the Wheelwriter BUS

// ---------------------------------------------------------------------------
// UART3 interrupt service routine
// ---------------------------------------------------------------------------
void uart3_isr(void) interrupt 17 using 3 {
	unsigned int wwBusData;
	//static char count = 0;

    // UART3 transmit interrupt
    if (S3TI) {                                 // transmit interrupt?
	   CLR_S3TI;                                // clear transmit interrupt flag
	   tx3_ready = TRUE;						// transmit buffer is ready for a new character
    }

    if(S3RI) {                         	        // receive interrupt?
       CLR_S3RI;                                // clear receive interrupt flag
       wwBusData = S3BUF;                       // retrieve the lower 8 bits
       if (S3RB8) wwBusData |= 0x0100;          // ninth bit is in S3RB8
       rx3_buf[rx3_head++ & (RBUFSIZE3-1)] = wwBusData;  // save it in the buffer
    }   
}

// ---------------------------------------------------------------------------
//  Initialize UART3 for mode 3. Mode 3 is an asynchronous mode that transmits and receives
//  a total of 11 bits: 1 start bit, 9 data bits, and 1 stop bit. The ninth bit to be 
//  transmitted is determined by the value in S3TB8 (S3CON.3). When the ninth bit is received, 
//  it is stored in S3RB8 (S3CON.2). 
//  The baud rate is determined by the T3 overflow rate.
//  the formula for calculating the UART3 baud rate is: baud rate = (T3 overflow)/4.
//  If T3 is operating in 1T mode (T3x12=1), the baud rate of UART3 = SYSclk/(65536-[T3H,T3L])/4.
// ---------------------------------------------------------------------------
void uart3_init(void) {
    rx3_head = 0;                   			// initialize UART3 buffer head/tail pointers.
    rx3_tail = 0;
    
    SET_S3ST3;                                  // set S3ST3 to select Timer 3 as baud rate generator for UART3.
    CLR_T3_CT;                                  // clear T3_C/T to make Timer 3 operate as timer instead of counter 
    SET_T3x12;                                  // set T3x12=1 to make Timer 3 operate in 1T mode.
    T3L = 0XF0;                                 // low byte of 65520
    T3H = 0XFF;                                 // high byte of 65520
    SET_T3R;                                    // set T3R to enable Timer 3 to run

    SET_S3SM0;                                  // set S3SM0 for UART3 mode 3 operation
    SET_S3REN;                    			    // set S3REN to enable reception
    SET_S3TI;                                   // set S3TI to enable transmit
    CLR_S3RI;                                   // clear S3RI 
    SET_ES3;                    			    // set ES3 to enable UART3 serial interrupt
    EA = TRUE;                     				// enable global interrupt
}   

// ---------------------------------------------------------------------------
// sends Acknowledge (all zeros) to the Function Board
// ---------------------------------------------------------------------------
void send_ACK_to_function_board(void) {
   while (!tx3_ready);		 					// wait until transmit buffer is empty
   tx3_ready = 0;                               // clear flag   
   while(!WWbus3);                              // wait until the Wheelwriter bus goes high
   CLR_S3REN;                                   // clear S3REN to disable reception
   CLR_S3TB8;
   S3BUF = 0x00;                                // lower 8 bits
   while(!tx3_ready);                           // wait until finished transmitting
   while(!WWbus3);                              // wait until the Wheelwriter bus goes high
   SET_S3REN;                    		        // set S3REN to re-enable reception
}

// ---------------------------------------------------------------------------
// sends an unsigned integer as 11 bits (start bit, 9 data bits, stop bit)
// to the Function Board. does not wait for acknowledge
// ---------------------------------------------------------------------------
void send_to_function_board(unsigned int wwCommand) {
   while (!tx3_ready);		 					// wait until transmit buffer is empty
   tx3_ready = 0;                               // clear flag   
   while(!WWbus3);                              // wait until the Wheelwriter bus goes high
   CLR_S3REN;                                   // clear S3REN to disable reception
   if (wwCommand & 0x100) SET_S3TB8; else CLR_S3TB8;
   S3BUF = wwCommand & 0xFF;                    // lower 8 bits
   while(!tx3_ready);                           // wait until finished transmitting
   while(!WWbus3);                              // wait until the Wheelwriter bus goes high
   SET_S3REN;                    		        // set S3REN to re-enable reception
}

// ---------------------------------------------------------------------------
// returns 1 if there is an unsigned integer from the Function Board waiting in the UART3 receive buffer.
// ---------------------------------------------------------------------------
char function_board_cmd_avail(void) {
   return (rx3_head != rx3_tail);               // not equal means there's something in the buffer
}

//----------------------------------------------------------------------------
// returns the next unsigned integer from the Function Board in the UART3 receive buffer.
// waits for an integer to become available if necessary.
//----------------------------------------------------------------------------
unsigned int get_function_board_cmd(void) {
    unsigned int buf;

    while (rx3_head == rx3_tail);     			// wait until a word is available
    buf = rx3_buf[rx3_tail++ & (RBUFSIZE3-1)];           // retrieve the word from the buffer
    return(buf);
}

