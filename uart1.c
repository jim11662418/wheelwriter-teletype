// Interrupt driven UART1 functions with RTS/CTS handshaking.
// UART1  uses a receive buffer in internal MOVX SRAM.
// UART1 uses the Timer 2 for baud rate generation. init_uart1 must be called 
// before using functions. No syntax error handling.
// RxD on pin 21, TxD on pin 22, RTS on pin 31

#include <reg51.h>
#include <stc51.h>

#define FALSE 0
#define TRUE  1

#define FOSC 12000000L                          // 12 MHz system clock frequency
#define RBUFSIZE1 256                           // receive buffer for 256 bytes
#if RBUFSIZE1 < 4
#error RBUFSIZE1 is too small. RBUFSIZE1 may not be less than 4.
#elif RBUFSIZE1 > 256
#error RBUFSIZE1 is too large. RBUFSIZE1 may not be greater than 256.
#elif ((RBUFSIZE1 & (RBUFSIZE1-1)) != 0)
#error RBUFSIZE1 must be a power of 2.
#endif

#define PAUSELEVEL RBUFSIZE1/4		            // pause communications to avoid overflow (RTS = 1) when buffer space < 64 bytes
#define RESUMELEVEL RBUFSIZE1/2                 // resume communications (RTS = 0) when buffer space > 128 bytes

sbit CTS = P4^2;  								// CTS input on pin 30 (not used)
sbit RTS = P4^4;								// RTS output on pin 31
volatile unsigned char data rx1_head;    	    // receive interrupt index for UART1
volatile unsigned char data rx1_tail;  	   	    // receive read index for UART1
volatile unsigned char rx1_remaining;		    // receive buffer space remaining
volatile unsigned char xdata rx1_buf[RBUFSIZE1];// receive buffer for UART1 in internal MOVX RAM
volatile bit tx1_ready;

// ---------------------------------------------------------------------------
// UART1 interrupt service routine
// ---------------------------------------------------------------------------
void uart1_isr() interrupt 4 using 2 {

   // uart1 transmit interrupt
   if (TI) {                                        // transmit interrupt?
	   TI = FALSE;                                  // clear transmit interrupt flag
	   tx1_ready = TRUE;							// transmit buffer is ready for a new character
    }

    // serial 0 receive interrupt
    if(RI) {                                	    // receive character?
        RI = 0;                             	    // clear serial receive interrupt flag
        rx1_buf[rx1_head++ & (RBUFSIZE1-1)] = SBUF;  // Get character from serial port and put into serial 0 fifo.
        --rx1_remaining;							// space remaining in UART2 buffer decreases
        if (!RTS){ 								// if communications is not now paused...
        	if (rx1_remaining < PAUSELEVEL) {
          		RTS = 1;						// pause communications when space in UART2 buffer decreases to less than 64 bytes
       		}
    	}
    }
}

// ---------------------------------------------------------------------------
//  Initialize UART1 using Timer 1 for baud rate generation
// ---------------------------------------------------------------------------
void init_uart1(unsigned baudrate) {
    rx1_head = 0;                   			// initialize UART1 buffer head/tail pointers
    rx1_tail = 0;

    SCON = 0x50;                  				// configure UART1 for mode 1:  8 bit UART with variable baud rate
    SET_S1ST2;                                  // select Timer 2 as baud rate generator
    
    CLR_T2_CT;                                  // clear T2_C/T to make Timer 2 operate as timer instead of counter 
    SET_T2x12;                                  // set T2x12=1 to make Timer 2 operate in 1T mode.
    T2L = (65536-(FOSC/4/baudrate));            // low byte of preload
    T2H = (65536-(FOSC/4/baudrate))>>8;         // high byte of preload
    SET_T2R;                                    // set T2R to enable Timer 2 to run
    
    REN = TRUE;                    				// enable receive characters.
    TI = TRUE;                     				// set TI of SCON to Get Ready to Send
    RI  = FALSE;                   				// clear RI of SCON to Get Ready to Receive
    RTS = 0;                                    // clear RTS to allow transmissions from remote console
    ES = TRUE;                    				// enable serial interrupt.
    EA = TRUE;                     				// enable global interrupt
}

// ---------------------------------------------------------------------------
// returns 1 if there are character waiting in the UART1 receive buffer
// ---------------------------------------------------------------------------
char char_avail1() {
   return (rx1_head != rx1_tail);
}

//-----------------------------------------------------------
// waits until a character is available in the UART1 receive
// buffer. returns the character. does not echo the character.
//-----------------------------------------------------------
char getchar1() {
    unsigned char buf;

    while (rx1_head == rx1_tail);     			// wait until a character is available
    buf = rx1_buf[rx1_tail++ &(RBUFSIZE1-1)];
	++rx1_remaining;			   			    // space remaining in buffer increases
	if (RTS) {	  								// if communication is now paused...
   		if (rx1_remaining > RESUMELEVEL) {			
      		RTS = 0;							// clear RTS to resume communications when space remaining in buffer increases above 128 bytes
   		}  
	}    
    return(buf);
}

// ---------------------------------------------------------------------------
// sends one character out UART1.
// ---------------------------------------------------------------------------
char putchar1(char c)  {
   while (!tx1_ready);                          // wait here for transmit ready
   SBUF = c;
   tx1_ready = 0;
   return (c);
}


