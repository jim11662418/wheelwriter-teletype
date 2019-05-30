//------------------------------------------------------------------------------------------
// Uses an STCmicro STC15W4K32S4 series micro controller to turn an IBM Wheelwriter Electronic
// Typewriter into a teletype-like device.
//
// For the Keil C51 compiler.
//
// Version 1.0.0 - Initial version
// Version 1.1.0 - Changes necessary to make it work with both Wheelwriter 3 and 6
// Version 1.1.1 - Renamed some function and variables for added readability
//
//------------------------------------------------------------------------------------------

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <reg51.h>
#include <stc51.h>
#include <intrins.h>
#include "control.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "wheelwriter.h"

#define BAUDRATE 9600                   // UART2 serial console at 9600 bps

#define FIFTEENCPI 8                    // number of micro spaces for each character on the 15P printwheel (15 cpi)
#define TWELVECPI 10                    // number of micro spaces for each character on the 12P printwheel (12 cpi)
#define TENCPI 12                       // number of micro spaces for each character on the 10P printwheel (10 cpi)

#define FALSE 0
#define TRUE  1
#define LOW 0
#define HIGH 1
#define ON 0                            // 0 turns the LEDs on
#define OFF 1                           // 1 turns the LEDs off

// 12,000,000 Hz/12 = 1,000,000 Hz = 1.0 microsecond clock period
// 50 milliseconds per interval/1.0 microseconds per clock = 50,000 clocks per interval
#define RELOADHI (65536-50000)/256
#define RELOADLO (65536-50000)&255
#define ONESEC 20                       // 20*50 milliseconds = 1 second

sbit POR      = P0^4;                   // Power-On-Reset output pin 5
sbit redLED   = P0^5;                   // red   LED connected to pin 6 0=on, 1=off
sbit amberLED = P0^6;                   // amber LED connected to pin 7 0=on, 1=off
sbit greenLED = P0^7;                   // green LED connected to pin 8 0=on, 1=off

bit autoLinefeed = FALSE;               // when true, automatically print a linefeed with each carriage return
bit typewriter = TRUE;                  // when true wheelwriter keystrokes go to wheelwriter, when false wheelwriter keystrokes go to serial console
bit errorLED = FALSE;                   // makes the red LED flash when TRUE
unsigned char wheelwriterModel = 0;
unsigned char attribute = 0;            // bit 0=bold, bit 1=continuous underline, bit 2=multiple word underline
unsigned char column = 1;               // current print column (1=left margin)
unsigned char tabStop = 5;              // horizontal tabs every 5 spaces (every 1/2 inch)
volatile unsigned char timeout = 0;     // decremented every 50 milliseconds, used for detecting timeouts
volatile unsigned char hours = 0;       // uptime hours
volatile unsigned char minutes = 0;     // uptime minutes
volatile unsigned char seconds = 0;     // uptime seconds

code char title[]     = "Wheelwriter Teletype Version 1.1.1";
code char mcu[]       = "STCmicro IAP15W4K61S4 MCU";
code char compiled[]  = "Compiled on " __DATE__ " at " __TIME__;
code char copyright[] = "Copyright 2019 Jim Loos";

//------------------------------------------------------------
// Timer 0 ISR: interrupt every 50 milliseconds, 20 times per second
//------------------------------------------------------------
void timer0_isr(void) interrupt 1 using 1{
    static char ticks = 0;

    if (timeout) {                  // countdown value for detecting timeouts
        --timeout;
    }

    if(++ticks == 20) { 		    // if 20 ticks (one second) have elapsed...
        ticks = 0;

        if (errorLED) {             // if there's an error 
           redLED = !redLED;        // toggle the red LED once each second
        }

        if (++seconds == 60) {		// if 60 seconds (one minute) has elapsed...
            seconds = 0;
            if (++minutes == 60) {	// if 60 minutes (one hour) has elapsed...
                minutes = 0;
                ++hours;
            }
        }
    }
}

//------------------------------------------------------------------------------------------
// The Wheelwriter prints the character and updates column.
// Carriage return cancels bold and underlining.
// Linefeeds automatically printed with carriage return if switch 1 is on.
// The character printed by the Wheelwriter is echoed to the serial port (for monitoring).
//
// Control characters:
//  BEL 0x07    spins the printwheel
//  BS  0x08    non-destructive backspace
//  TAB 0x09    horizontal tab to next tab stop
//  LF  0x0A    moves paper up one line
//  VT  0x0B    moves paper up one line
//  CR  0x0D    returns carriage to left margin, if switch 1 is on, moves paper up one line (linefeed)
//  ESC 0x1B    see Diablo 630 commands below...
//
// Diablo 630 commands emulated:
// <ESC><O>  selects bold printing for one line
// <ESC><&>  cancels bold printing
// <ESC><E>  selects continuous underlining  (spaces between words are underlined) for one line
// <ESC><R>  cancels underlining
// <ESC><X>  cancels both bold and underlining
// <ESC><U>  half line feed (paper up 1/2 line)
// <ESC><D>  reverse half line feed (paper down 1/2 line)
// <ESC><BS> backspace 1/120 inch
// <ESC><LF> reverse line feed (paper down one line)
//
// printer control not part of the Diablo 630 emulation:
// <ESC><u>  selects micro paper up (1/8 line or 1/48")
// <ESC><d>  selects micro paper down (1/8 line or 1/48")
// <ESC><b>  selects broken underlining (spaces between words are not underlined)
// <ESC><l><n> auto linefeed (n=1 is on, n=0 is off)
// <ESC><p>  selects Pica pitch (10 characters/inch or 12 point)
// <ESC><e>  selects Elite pitch (12 characters/inch or 10 point)
// <ESC><m>  selects Micro Elite pitch (15 characters/inch or 8 point)
//
// diagnostics/debugging:
// <ESC><^Z><c> print (on the serial console) the current column
// <ESC><^Z><e><n> turn flashing red error LED on or off (n=1 is on, n=0 is off)
// <ESC><^Z><p><n> print (on the serial console) the value of Port n (0-5) as 2 digit hex number
// <ESC><^Z><r> reset both the MCU and the wheelwriter
// <ESC><^Z><t> print (on the serial console) the state of the typewriter/keyboard flag
// <ESC><^Z><u> print (on the serial console) the uptime as HH:MM:SS
// <ESC><^Z><w> print (on the serial console) the number of watchdog resets
//-------------------------------------------------------------------------------------------
void print_char_on_WW(unsigned char charToPrint) {
    static char escape = 0;                                 // escape sequence state
    char i,t;

    switch (escape) {
        case 0:
            switch (charToPrint) {
                case NUL:
                    break;
                case BEL:
                    ww_spin();
                    putchar(BEL);
                    break;
                case BS:
                    if (column > 1){                        // only if there's at least one character on the line
                        ww_backspace();
                        --column;                           // update column
                        putchar(BS);
                    }
                    break;
                case HT:
                    t = tabStop-(column%tabStop);           // how many spaces to the next tab stop
                    ww_horizontal_tab(t);                   // move carrier to the next tab stop
                    for(i=0; i<t; i++){
                        ++column;                           // update column
                        putchar(SP);
                    }
                    break;
                case LF:
                    ww_linefeed();
                    putchar(LF);
                    break;
                case VT:
                    ww_linefeed();
                    break;
                case CR:
                    ww_carriage_return();                   // return the carrier to the left margin
                    column = 1;                             // back to the left margin
                    attribute = 0;                          // cancel bold and underlining
                    if (autoLinefeed) {                     // if TRUE, automatically print linefeed
                        ww_linefeed();
                    }
                    putchar(CR);
                    break;
                case ESC:
                    escape = 1;
                    break;
                default:
                    ww_print_letter(charToPrint,attribute);
                    putchar(charToPrint);                   // echo the character to the console
                    ++column;                               // update column
            } // switch (charToPrint)

            break;  // case 0:
        case 1:                                             // this is the second character of the escape sequence...
            switch (charToPrint) {
                case 'O':                                   // <ESC><O> selects bold printing
                    attribute |= 0x01;
                    escape = 0;
                    break;
                case '&':                                   // <ESC><&> cancels bold printing
                    attribute &= 0x06;
                    escape = 0;
                    break;
                case 'E':                                   // <ESC><E> selects continuous underline (spaces between words are underlined)
                    attribute |= 0x02;
                    escape = 0;
                    break;
                case 'R':                                   // <ESC><R> cancels underlining
                    attribute &= 0x01;
                    escape = 0;
                    break;
                case 'X':                                   // <ESC><X> cancels both bold and underlining
                    attribute = 0;
                    escape = 0;
                    break;                
                case 'U':                                   // <ESC><U> selects half line feed (paper up one half line)
                    ww_paper_up();
                    escape = 0;
                    break;
                case 'D':                                   // <ESC><D> selects reverse half line feed (paper down one half line)
                    ww_paper_down();
                    escape = 0;
                    break;
                case LF:                                    // <ESC><LF> selects reverse line feed (paper down one line)
                    ww_reverse_linefeed();
                    escape = 0;
                    break;
                case BS:                                    // <ESC><BS> backspace 1/120 inch
                    ww_micro_backspace();
                    escape = 0;
                    break;
                case 'b':                                   // <ESC><b> selects broken underline (spaces between words are not underlined)
                    attribute |= 0x04;
                    escape = 0;
                    break;
                case 'e':                                   // <ESC><e> selects Elite (12 characters/inch)
                    ww_set_printwheel(TWELVECPI);           // 10 micro spaces/character, 16 micro lines/line
                    tabStop =6;                             // tab stops every 6 characters (every 1/2 inch)
                    escape = 0;
                    break;
                case 'l':                                   // <ESC><l> selects auto linefeed, the next character turns it on or off
                    escape = 5;
                    break;
                case 'p':                                   // <ESC><p> selects Pica (10 characters/inch)
                    ww_set_printwheel(TENCPI);              // 12 micro spaces/character, 16 micro lines/line
                    tabStop =5;                             // tab stops every 5 characters (every 1/2 inch)
                    escape = 0;
                    break;
                case 'm':                                   // <ESC><m> selects Micro Elite (15 characters/inch)
                    ww_set_printwheel(FIFTEENCPI);          // 8 micro spaces/character, 12 micro lines/line
                    tabStop =7;                             // tab stops every 7 characters (every 1/2 inch)
                    escape = 0;
                    break;
                case 'u':                                   // <ESC><u> paper micro up (paper up 1/8 line)
                    ww_micro_up();
                    escape = 0;
                    break;
                case 'd':                                   // <ESC><d> paper micro down (paper down 1/8 line)
                    ww_micro_down();
                    escape = 0;
                    break;
                case '\x1A':                                // <ESC><^Z> for remote diagnostics
                    escape = 2;
                    break;  
            } // switch (charToPrint)
            break;  // case 1:
        case 2:                                             // this is the third character of the escape sequence...
            switch (charToPrint) {
                case 'c':                                   // <ESC><^Z><c> print current column
                    printf("%s %u\n","Column:",(int)column);
                    escape = 0;
                    break;
                case 'e':                                   // <ESC><^Z><e> controls the red error LED. the next character turn is on or off
                    escape = 4;
                    break;
                case 'p':                                   // <ESC><^Z><p> print port values. the next character selects the port number
                    escape = 3;
                    break;
                case 'r':                                   // <ESC><^Z><r> reset the MCU and the Wheelwriter
                    escape = 0;
                    IAP_CONTR = 0x20;
                    break; 
                case 't':                                   // <ESC><^Z><t> print "typewriter/keyboard" flag
                    printf("Wheelwriter key strokes go to ");
                    if (typewriter) printf("Wheelwriter.\n"); else printf("serial console.\n");
                    escape = 0;
                    break;
                case 'u':                                   // <ESC><^Z><u> print uptime
                    printf("%s %02u%c%02u%c%02u\n","Uptime:",(int)hours,':',(int)minutes,':',(int)seconds);
                    escape = 0;
                    break; 
                case 'w':                                   // <ESC><^Z><w> print watchdog resets
                    escape = 0;
                    break;
            } // switch (charToPrint)
            break;  // case 2:
        case 3:                                             // this is the fourth character of the escape sequence
            switch (charToPrint){
                case '0':
                    printf("%s 0x%02X\n","P0:",(int)P0);    // <ESC><^Z><p><0> print port 0 value
                    escape = 0;
                    break;
                case '1':
                    printf("%s 0x%02X\n","P1:",(int)P1);    // <ESC><^Z><p><1> print port 1 value
                    escape = 0;
                    break;
                case '2':
                    printf("%s 0x%02X\n","P2:",(int)P2);    // <ESC><^Z><p><2> print port 2 value
                    escape = 0;
                    break;
                case '3':
                    printf("%s 0x%02X\n","P3:",(int)P3);    // <ESC><^Z><p><3> print port 3 value
                    escape = 0;
                    break;
                case '4':
                    printf("%s 0x%02X\n","P4:",(int)P4);    // <ESC><^Z><p><3> print port 4 value
                    escape = 0;
                    break;
                case '5':
                    printf("%s 0x%02X\n","P5:",(int)P5);    // <ESC><^Z><p><3> print port 5 value
                    escape = 0;
                    break;

            } // switch (charToPrint)
            break;  // case 3:
        case 4:
            if (charToPrint & 0x01) {
                errorLED = TRUE;                            // <ESC><^Z><e><n> odd values of n turn the LED on, even values turn the LED off
            }
            else {
                errorLED = FALSE;
                redLED = OFF;                               // turn off the red LED
            }
            escape = 0;
            break;  // case 4
        case 5:
            if (charToPrint & 0x01) {
                autoLinefeed = TRUE;                        // <ESC><l><n> odd values of n turn autoLinefeed on, even values turn autoLinefeed off
            }
            else {
                autoLinefeed = FALSE;
            }
            escape = 0;
            break; // case 5
    } // switch (escape)
}


/*------------------------------------------------------------------------------
Note that the two function below, getchar() and putchar(), replace the library
functions of the same name.  These functions use the interrupt-driven serial
I/O routines in uart2.c
------------------------------------------------------------------------------*/
// for scanf
char _getkey() {
    return getchar2();
}

// for printf
char putchar(char c)  {
   return putchar2(c);
}

//-----------------------------------------------------------
// main(void)
//-----------------------------------------------------------
void main(void){
	unsigned int loopcounter,function_board_cmd,printer_board_reply;
    unsigned char pitch = 0;
    unsigned char state = 0;
    char c;

    P0M1 = 0;                                                   // required to make port 0 work
    P0M0 = 0;

    POR = 1;                                                    // Power-On-Reset on

    amberLED = OFF;                                             // turn off the amber LED
    greenLED = OFF;                                             // turn off the green LED
    redLED = OFF;                                               // turn off the red LED

	TL0 = RELOADLO;                         	                // load timer 0 low byte
	TH0 = RELOADHI;              	                            // load timer 0 high byte
    TMOD = 0x00;                                                // configure timer 0 for mode 0: 16-bit auto-reload timer
	ET0 = 1;                     	                            // enable timer 0 interrupt
	TR0 = 1;                     	                            // run timer 0

    uart2_init(BAUDRATE);                                       // initialize UART2 for N-8-1 at 9600bps, RTS-CTS handshaking
    uart3_init();                                               // initialize UART3 for connection to the Function Board
    uart4_init();                                               // initialize UART4 for connection to the Printer Board

	EA = TRUE;                     	                            // global interrupt enable

    printf("\n%s\n%s\n%s\n%s\n\n",title,mcu,compiled,copyright);
    printf("Initializing...\n");
    
    pitch = 0; 
    timeout = 7*ONESEC;                                         // allow up to 7 seconds in case the carrier is at the right margin
    state = 0;
    POR = 0;                                                    // reset off

    //////////// determine the pitch of the printwheel /////////////
    while(timeout) {                                            // loop for 7 seconds waiting for data from the Wheelwriter
        while(function_board_cmd_avail()){                      // if there's a command from the function board...
            function_board_cmd = get_function_board_cmd();      // retrieve the command from UART3
            send_to_printer_board(function_board_cmd);          // relay the command to the printer board
            switch (state) {
                case 0:
                    if (function_board_cmd == 0x121) state = 1;
                    break;
                case 1:
                    if (function_board_cmd == 0x001) state = 2; 
                    else state = 0;                             // 0x121,0x001 is the "reset" command  from the function board
                    break; 
            } 
        }
   	    while(printer_board_reply_avail()) {                    // if there's a reply from the Printer Board...
            printer_board_reply = get_printer_board_reply();    // retrieve the reply from UART4
            send_to_function_board(printer_board_reply);        // relay replies from the Printer Board to the Function Board
            if (state==2) {                                     // if the reset command has been sent, the reply from the printer board is the printwheel pitch
                pitch = printer_board_reply;
                state = 3;
            }
	    }
    }

    switch (pitch) {
        case 0x008:
            ww_set_printwheel(TWELVECPI);                       // 10 microspaces/char, 16 microlines/line
            tabStop = 6;                                        // tab stops every 6 characters (every 1/2 inch)
            printf("PS printwheel\n");
            break;
        case 0x010:
            ww_set_printwheel(FIFTEENCPI);                      // 8 microspaces/char, 12 microlines/line
            tabStop = 7;                                        // tab stops every 7 characters (every 1/2 inch)
            printf("15P printwheel\n");
            break;
        case 0x020:
            ww_set_printwheel(TWELVECPI);                       // 10 microspaces/char, 16 microlines/line
            tabStop = 6;                                        // tab stops every 6 characters (every 1/2 inch)
            printf("12P printwheel\n");
            break;
        case 0x021:
            ww_set_printwheel(TWELVECPI);                       // 10 microspaces/char, 16 microlines/line
            tabStop = 6;                                        // tab stops every 6 characters (every 1/2 inch)
            printf("No printwheel\n");
            break;
        case 0x040:
            ww_set_printwheel(TENCPI);                          // 12 microspaces/char, 16 microlines/line
            tabStop = 5;                                        // tab stops every 5 characters (every 1/2 inch)
            printf("10P printwheel\n");
            break;
        default:
            ww_set_printwheel(TWELVECPI);                       // 10 microspaces/char, 16 microlines/line
            tabStop = 6;                                        // tab stops every 6 characters (every 1/2 inch)
            printf("Unable to determine printwheel. Defaulting to 12P.\n");
            printf("0x%02X\n",(int) pitch);
    }

    //////////// determine the Wheelwriter model /////////////
    wheelwriterModel = 0;
    amberLED = ON;                                              // turn on the amber LED
    send_to_printer_board_wait(0x121);                          // command to "report Wheelwriter model"
    send_to_printer_board_wait(0x000);
    amberLED = OFF;                                             // turn off the amber LED
    timeout=ONESEC;                                             // 1 second timeout
    
    while(timeout){                                             // loop for one second waiting for a reply from the printer board
	    while(printer_board_reply_avail()) {                    // while there's a reply from the Printer Board...
            printer_board_reply = get_printer_board_reply();    // retrieve the replay from UART4
            wheelwriterModel = printer_board_reply;
        }
    }

    switch (wheelwriterModel) {
        case 0x006:
            printf("Wheelwriter 3\n");
            break;
        case 0x025:
            printf("Wheelwriter 5\n");
            break;
        case 0x026:
            printf("Wheelwriter 6\n");
            break;
        default:
            printf("Unknown Wheelwriter model\n");
            printf("0x%02X\n",(int) wheelwriterModel);
            errorLED=TRUE;               
            break;
    }

    printf("Ready\n");

    state = 0;

    //----------------- loop here forever -----------------------------------------
    while(TRUE) {

        if (++loopcounter==0) {                                 // every 65536 times through the loop (at about 2Hz)
            greenLED = !greenLED;                               // toggle the green "heart beat" LED
        }

        // check for commands from the function board
	    while(function_board_cmd_avail()) {                    // if there's a command from the Function Board...
            function_board_cmd = get_function_board_cmd();     // retrieve it from UART3

            switch (state) {
                case 0:
                    if (function_board_cmd==0x121) state = 1; 
                    break;
                case 1:
                    if (function_board_cmd==0x00E) state = 2;
                    else state = 0;
                    break;
                case 2:
                    if ((function_board_cmd &0x7F)==0x04F) {    // 0x121,0x00E,0x047 is Code-Erase key combo to toggle between "typewriter" and "keyboard" modes
                        typewriter = !typewriter;               // toggle the flag
                        if (typewriter) {                       // when returning from "keyboard" to "typewriter" mode...    
                            printf("Reseting Wheelwriter...\n");// reset the wheelwriter
                            POR = 1;                            // power-on-reset on
                            POR = 0;                            // power-on-reset off
                            putchar(CR);                        // return the console to the left margin
                        }
                        else {
                            ww_spin();                          // spin the printwheel when going from "typewriter" to "keyboard" mode
                        }
                    }
                    state = 0;
                    break;
            }

            if (typewriter){                                    // if "typewriter" mode 
                send_to_printer_board(function_board_cmd);      // relay commands from the Function Board to the Printer Board, do not wait for acknowledge
            }
            else {                                              // if "keyboard" mode
                send_ACK_to_function_board();                   // send Acknowledge to Function Board
                putchar2(ww_decode_keys(function_board_cmd));   // convert the function board keystroke cmd to ASCII and print it on the serial console
            }
	    }

        // check for replies from the printer board
	    while (printer_board_reply_avail()) {                   // if there's a reply from the Printer Board...
            printer_board_reply = get_printer_board_reply();    // retrieve the reply from UART4
            send_to_function_board(printer_board_reply);        // relay replies from the Printer Board to the Function Board
            //printf("%03X\n",printer_board_reply); 
	    }

        if (char_avail2()) {                                    // if there is a character in the serial receive buffer...
            c = getchar2();                                     // retrieve the character from UART2
            print_char_on_WW(c);                                // send it to the Wheelwriter for printing
        }
    }
}