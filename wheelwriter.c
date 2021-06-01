#include <stdio.h>
#include <reg51.h>
#include <stc51.h>
#include "ww-uart3.h"
#include "ww-uart4.h"
#include "control.h"

#define FALSE 0
#define TRUE  1
#define ON 0                                    // 0 turns the amber LED on
#define OFF 1                                   // 1 turns the amber LED off

extern unsigned char column;                    // defined in main.c
extern bit typewriter;                          // defined in main.c

sbit P_RESET  = P0^4;                   		// Power-On-Reset for Printer Board output pin 5 0=on, 1=off
sbit F_RESET  = P1^4;                   		// Power-On-Reset for Function Board output pin 13 0=on, 1=off
sbit amberLED = P0^6;                           // amber LED connected to pin 6 0=on, 1=off

unsigned char uSpacesPerChar = 10;              // micro spaces per character (8 for 15cpi, 10 for 12cpi and PS, 12 for 10cpi)
unsigned char uLinesPerLine = 16;               // micro lines per line (12 for 15cpi; 16 for 10cpi, 12cpi and PS)
unsigned int  uSpaceCount = 0;                  // number of micro spaces on the current line (for carriage return)

//------------------------------------------------------------------------------------------------
// ASCII to Wheelwriter printwheel translation table
// The Wheelwriter printwheel code indicates the position of the character on the printwheel. 
// a (code 01) is at the 12 oclock position of the printwheel. Going counter clockwise, 
// n (code 02) is next character on the printwheel followed by r (code 03), m (code 04),
// c (code 05), s (code 06), d (code 07), h (code 08), and so on.
//------------------------------------------------------------------------------------------------
char code ASCII2printwheel[160] =  
// col: 00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F    row:
//      sp     !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
      {0x00, 0x49, 0x4b, 0x38, 0x37, 0x39, 0x3f, 0x4c, 0x23, 0x16, 0x36, 0x3b, 0x0c, 0x0e, 0x57, 0x28, // 20
//       0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
       0x30, 0x2e, 0x2f, 0x2c, 0x32, 0x31, 0x33, 0x35, 0x34, 0x2a ,0x4e, 0x50, 0x00, 0x4d, 0x00, 0x4a, // 30
//       @     A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
       0x3d, 0x20, 0x12, 0x1b, 0x1d, 0x1e, 0x11, 0x0f, 0x14, 0x1F, 0x21, 0x2b, 0x18, 0x24, 0x1a, 0x22, // 40
//       P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _   
       0x15, 0x3e, 0x17, 0x19, 0x1c, 0x10, 0x0d, 0x29, 0x2d, 0x26, 0x13, 0x41, 0x00, 0x40, 0x00, 0x4f, // 50
//       `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
       0x00, 0x01, 0x59, 0x05, 0x07, 0x60, 0x0a, 0x5a, 0x08, 0x5d, 0x56, 0x0b, 0x09, 0x04, 0x02, 0x5f, // 60
//       p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~    DEL  
       0x5c, 0x52, 0x03, 0x06, 0x5e, 0x5b, 0x53, 0x55, 0x51, 0x58, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, // 70
//     
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 80      
//     
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 90      
//                   ¢                             §                                                  
       0x00, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // A0      
//       °     ±     ²     ³                 ¶                                   ¼     ½              
       0x44, 0x3C, 0x42, 0x43, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x47, 0x00, 0x00};// B0

//------------------------------------------------------------------------------------------------
// Wheelwriter printwheel to ASCII translation table
//------------------------------------------------------------------------------------------------
char code printwheel2ASCII[97] = {
// SP   a    n    r    m    c    s    d    h    l    f    k    ,    V    _    G
  0x20,0x61,0x6E,0x72,0x6D,0x63,0x73,0x64,0x68,0x6C,0x66,0x6B,0x2C,0x56,0x2D,0x47,
// U    F    B    Z    H    P    )    R    L    S    N    C    T    D    E    I   
  0x55,0x46,0x42,0x5A,0x48,0x50,0x29,0x52,0x4C,0x53,0x4E,0x43,0x54,0x44,0x45,0x49,
// A    J    O    (    M    .    Y    ,    /    W    9    K    3    X    1    2
  0x41,0x4A,0x4F,0x28,0x4D,0x2E,0x59,0x2C,0x2F,0x57,0x39,0x4B,0x33,0x58,0x31,0x32,
// 0    5    4    6    8    7    *    $    #    %    ¢    +    ±    @    Q    &
  0x30,0x35,0x34,0x36,0x38,0x37,0x2A,0x24,0x23,0x25,0xA2,0x2B,0xB1,0x40,0x51,0x26,
// ]    [    ³    ²    º    §    ¶    ½    ¼    !    ?    "    '    =    :    -
  0x5D,0x5B,0xB3,0xB2,0xBA,0xA7,0xB6,0xBD,0xBC,0x21,0x3F,0x22,0x60,0x3D,0x3A,0x5F,
// ;    x    q    v    z    w    j    .    y    b    g    u    p    i    t    o    e   
  0x3B,0x78,0x71,0x76,0x7A,0x77,0x6A,0x2E,0x79,0x62,0x67,0x75,0x70,0x69,0x74,0x6F,0x65};


//--------------------------------------------------------------------------------------------------
// 1 - resets the Function Board
// 2 - resets the Printer Board
// 3 - resets both boards
//--------------------------------------------------------------------------------------------------
void ww_reset(unsigned char board) {
	unsigned char delay;
	switch (board) {
		case 1:																									// reset the function board
			F_RESET = 1;                                    // Function Board reset on																					// turn on reset transistor for the function board
			break;
		case 2:																									// reset the printer board
			P_RESET = 1;                                    // Printer Board reset on																					// turn on reset transistor for the printer board
			break;
		default:																								// reset both boards
			P_RESET = 1;                        			// Printer Board reset on
            F_RESET = 1;                        			// Function Board reset on
	}
	for(delay=0;delay<110;++delay);     					// ~1 mSec delay
    P_RESET = 0;                        					// Printer Board reset off
    F_RESET = 0;                        					// Function Board reset off
}	

//------------------------------------------------------------------------------------------------
// backspace, no erase. decreases micro space count by uSpacesPerChar.
//------------------------------------------------------------------------------------------------
void ww_backspace(void) {    
    amberLED = ON;                    
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x006);                      // move the carrier horizontally
    send_to_printer_board_wait(0x000);                      // bit 7 is cleared for right to left direction
    send_to_printer_board_wait(uSpacesPerChar);    
    uSpaceCount -= uSpacesPerChar;
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// backspace 1/120 inch. decrements micro space count
//------------------------------------------------------------------------------------------------
void ww_micro_backspace(void) {
    if (uSpaceCount){                                       // only if the carrier is not at the left margin
        amberLED = ON;
        send_to_printer_board_wait(0x121);
        send_to_printer_board_wait(0x006);                  // move the carrier horizontally
        send_to_printer_board_wait(0x000);                  // bit 7 is cleared for right to left direction
        send_to_printer_board_wait(0x001);                  // one microspace
        --uSpaceCount;
        amberLED = OFF;
    }
}

//------------------------------------------------------------------------------------------------
// To return the carrier to the left margin, the Wheelwriter requires an eleven bit number which 
// indicates the number of micro spaces to move to reach the left margin. The upper three bits of
// the 11-bit number are sent as the 3rd word of the command, and lower 8 bits are sent as the 4th
// word. Bit 7 of the 3rd word is cleared to indicate carriage movement left direction.
// resets micro space count back to zero.
//------------------------------------------------------------------------------------------------
void ww_carriage_return(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x006);                      // move the carrier horizontallly
    send_to_printer_board_wait((uSpaceCount>>8)&0x007);     // bit 7 is cleared for right to left direction, bits 0-2 = upper 3 bits of micro spaces to left margin
    send_to_printer_board_wait(uSpaceCount&0xFF);           // lower 8 bits of micro spaces to left margin
    uSpaceCount = 0;                                        // clear count
    amberLED = OFF;                                         
}

//------------------------------------------------------------------------------------------------
// ww_spins the printwheel as a visual and audible indication
//------------------------------------------------------------------------------------------------
void ww_spin(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x007);
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// horizontal tab number of "spaces". updates micro space count.
//------------------------------------------------------------------------------------------------
void ww_horizontal_tab(unsigned char spaces) {
    unsigned int s;

    amberLED = ON;
    s = spaces*uSpacesPerChar;                              // number of microspaces to move right
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x006);                      // move the carrier horizontally
    send_to_printer_board_wait(((s>>8)&0x007)|0x80);        // bit 7 is set for left to right direction, bits 0-2 = upper 3 bits of micro spaces to move right
    send_to_printer_board_wait(s&0xFF);                     // lower 8 bits of micro spaces to move right
    uSpaceCount += s;                                       // update micro space count
    amberLED = OFF;                                         
}

//------------------------------------------------------------------------------------------------
// backspaces and erases "letter". updates micro space count.
// Note: erasing bold or underlined characters or characters on lines other than 
// the current line is not implemented yet.
//------------------------------------------------------------------------------------------------
void ww_erase_letter(unsigned char letter) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x006);                      // move the carrier horizontally
    send_to_printer_board_wait(0x000);                      // bit 7 is cleared for right to left direction
    send_to_printer_board_wait(uSpacesPerChar);             // number of micro spaces to move left    
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x004);                      // print on correction tape
    send_to_printer_board_wait(ASCII2printwheel[letter-0x20]);
    send_to_printer_board_wait(uSpacesPerChar);             // number of micro spaces to move right
    uSpaceCount -= uSpacesPerChar;                          // update the micro space count
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// paper up one line
//------------------------------------------------------------------------------------------------
void ww_linefeed(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x080|uLinesPerLine);        // bit 7 is set to indicate paper up direction, bits 0-4 indicate number of microlines for 1 full line
    amberLED = OFF;
}    

//------------------------------------------------------------------------------------------------
// paper down one line
//------------------------------------------------------------------------------------------------
void ww_reverse_linefeed(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x000|uLinesPerLine);        // bit 7 is cleared to indicate paper down direction, bits 0-4 indicate number of microlines for 1 full line
    amberLED = OFF;
}    

//------------------------------------------------------------------------------------------------
// paper up 1/2 line
//------------------------------------------------------------------------------------------------
void ww_paper_up(void) {     
    amberLED = ON;               
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x080|(uLinesPerLine>>1));   // bit 7 is set to indicate up direction, bits 0-3 indicate number of microlines for 1/2 line
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// paper down 1/2 line
//------------------------------------------------------------------------------------------------
void ww_paper_down(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x000|(uLinesPerLine>>1));   // bit 7 is cleared to indicate down direction, bits 0-3 indicate number of microlines for 1/2 full line
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// paper up 1/8 line
//------------------------------------------------------------------------------------------------
void ww_micro_up(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x080|(uLinesPerLine>>3));   // bit 7 is set to indicate up direction, bits 0-3 indicate number of microlines for 1/8 full line or 1/48"
    amberLED = OFF;
}

//------------------------------------------------------------------------------------------------
// paper down 1/8 line
//------------------------------------------------------------------------------------------------
void ww_micro_down(void) {
    amberLED = ON;
    send_to_printer_board_wait(0x121);
    send_to_printer_board_wait(0x005);                      // vertical movement
    send_to_printer_board_wait(0x000|(uLinesPerLine>>3));   // bit 7 is cleared to indicate down direction, bits 0-3 indicate number of microlines for 1/8 full line or 1/48"
    amberLED = OFF;
}

//-----------------------------------------------------------
// Sends the printwheel code for the ASCII "letter" to be printed the Wheelwriter. 
// Handles bold, continuous and multiple word underline printing.
// Carrier moves to the right by uSpacesPerChar.
// Increases the micro space count by uSpacesPerChar for each letter printed.
//-----------------------------------------------------------
void ww_print_letter(unsigned char letter,attribute) {
    if ((letter > 0x1F)&&(letter < 0xC0)) {                 // "letter" must be a printable character
        amberLED = ON;
        send_to_printer_board_wait(0x121);
        send_to_printer_board_wait(0x003);
        send_to_printer_board_wait(ASCII2printwheel[letter-0x20]);// ascii character (-0x20) as index to printwheel table    
        if ((attribute & 0x06) && ((letter!=0x20) || (attribute & 0x02))){// if underlining AND the letter is not a space OR continuous underlining is on
            send_to_printer_board_wait(0x000);              // advance zero micro spaces
            send_to_printer_board_wait(0x121);
            send_to_printer_board_wait(0x003);
            send_to_printer_board_wait(0x04F);              // print '_' underscore
        }
        if (attribute & 0x01) {                             // if the bold bit is set   
            send_to_printer_board_wait(0x001);              // advance carriage by one micro space
            send_to_printer_board_wait(0x121);
            send_to_printer_board_wait(0x003);
            send_to_printer_board_wait(ASCII2printwheel[letter-0x20]);// re-print the character offset by one micro space
            send_to_printer_board_wait((uSpacesPerChar)-1); // advance carriage the remaining micro spaces
        } 
        else { // not boldprint
            send_to_printer_board_wait(uSpacesPerChar);      
        }
        
        uSpaceCount += uSpacesPerChar;                      // update the micro space count
        if (uSpaceCount > 1450) {                           // right stop   
            ww_carriage_return();                           // automatically return to left margin
            column = 1;
        }
        amberLED = OFF;
    }
}

//--------------------------------------------------------------------------------------------------
// Decodes the 9 bit words sent by the Wheelwriter Function Board (on the BUS) when keys are pressed
// and returns the equivalent ASCII character (if there is one). Typically a sequence of a 
// minimum of three words (sometimes more, depending on the key) is required to decode each keypress.
// Call this function for each word received from the Function Board. Intermediate words return zeros.
//
// In "Typewriter" Mode, sends vertical movement commands (Paper Up, Paper Down, Micro Up, Micro
// Down and SAPI) to the Wheelwriter. Vertical movement commands ignored when not in typewriter mode.
//
// Horizontal movement commands are returned as Space, Backspace and Tab characters.
//
// Code key combinations are returned as control keys i.e. Code C is returned as Control C.
//
// The Code Erase key combo returns 0xF0.
//--------------------------------------------------------------------------------------------------
char ww_decode_keys(unsigned int WWdata) {
    static unsigned char keystate = 0xFF;
    static unsigned int lastWWdata = 0;
    char result;

    result = 0;
    switch(keystate) {
        case 0xFF:                                          // waiting for first data word from Wheelwriter...
            if (WWdata == 0x121)                            // all commands must start with 0x121...
               keystate = 0xFE;
            break;
        case 0xFE:                                          // 0x121 has been received...
            switch(WWdata) {
                case 0x003:                                 // 0x121,0x003 is start of alpha-numeric character sequence
                    keystate = 0x03;
                    break;
                case 0x005:                                 // 0x121,0x005 is start of vertical movement sequence
                    keystate = 0x05;
                    break;          
                case 0x006:                                 // 0x121,0x006 is start of horizontal movement sequence
                    keystate = 0x06;
                    break;
                case 0x00E:                                 // 0x121,0x00E is the start of a code key sequence
                    keystate = 0x0E;
                    break;
                default:
                    keystate = 0xFF;
            } // switch(WWdata)
            break;
        case 0x03:                                          // 0x121,0x003 has been received...          
            keystate = 0x31;                                // must wait for the microspaces value to follow
            break;
        case 0x31:                                          // 0x121,0x003,printwheel code  has been received, waiting for microspaces...      
            keystate = 0xFF;                                // reset keystate back to start    
            result = printwheel2ASCII[(lastWWdata)];        // get the ASCII code from the table
            break;
        case 0x05:                                          // 0x121,0x005 has been received, move paper vertically...
            keystate = 0xFF;
            if (((WWdata&0x1F)==uLinesPerLine)&&(WWdata&0x80)){// one line AND paper up direction
                result = CR;                                // LF used to detect when C Rtn key is pressed
            }                
            else {
                if (typewriter) {                           // if typewriter mode...
                    send_to_printer_board_wait(0x121);      // pass all other vertical commands thru...
                    send_to_printer_board_wait(0x005);      // Paper Up, Paper Down, Micro Up, Micro Down and SAPI
                    send_to_printer_board_wait(WWdata);
                }
            }
            break;
        case 0x06:                                          // 0x121,0x006 has been received...
            if (WWdata & 0x080)                             // if bit 7 is set...         
               keystate = 0x61;                             // 0x121,0x006,0x080 is horizontal movement to the right...
            else                                            // else...
               keystate = 0x62;                             // 0x121,0x006,0x000 is horizontal movement to the left...
            break;
        case 0x61:                                          // 0x121,0x006,0x08X has been received, move carrier to the right...
            keystate = 0xFF;
            if ((WWdata>uSpacesPerChar)&&(WWdata<uSpacesPerChar*10)) // if more than one space but less than 10 spaces, must be horizontal tab
                result = HT;
            else if (WWdata==uSpacesPerChar)
                result = SP;
            break;
        case 0x62:                                          // 0x121,0x006,0x00X has been received, move carrier to the left...
            keystate = 0xFF;
            /*if (((lastWWdata&0x007)==(uSpaceCount>>8))&&(WWdata==(uSpaceCount&0xFF)))
                result = CR;                                // instead, use the linefeed emitted when key is pressed to detect C Rtn (see above)
            else */                                         
            if (WWdata==uSpacesPerChar) 
                result = BS;
            break;
        case 0x0E:                                          // 0x121,0x00E has been received (code key combination)
            keystate = 0xFF;
            // convert code key combinations into control keys i.e. code c is converted into control c
            switch(WWdata & 0x07F) {                        // bit 7 is cleared on WW3, set on WW6
                case 0x006:	                                // Code+Z
                    result = SUB;                           // converted to ^Z
                    break;
                case 0x016:	                                // Code+C
                    result = ETX;                           // converted to ^C
                    break;
                case 0x01D:	                                // Code+G
                    result = BEL;                           // converted to ^G
                    break;
                case 0x024:	                                // Code+J
                    result = LF;                            // converted to ^J
                    break;
                case 0x025:	                                // Code+H
                    result = BS;                            // converted to ^H
                    break;
                case 0x026:	                                // Code+M
                    result = CR;                            // converted to ^M
                    break;
                case 0x02A:	                                // Code+I
                    result = HT;                            // converted to ^I
                    break;
                case 0x02C:	                                // Code+K
                    result = VT;                            // converted to ^K
                    break;
                case 0x034:	                                // Code+L
                    result = FF;                            // converted to ^L
                    break;
                case 0x048:                                 // Code+Mar Rel
                    result = ESC;                           // converted to Escape
                    break;                                  
                case 0x4F:                                  // Code+Erase
                    result = 0xF0;                          // return 0xFF for Code+Erase key combo
            } // switch(WWdata & 0x17F)
            break;
    }   // switch(keystate)
    lastWWdata = WWdata;                                    // save for next time
    return(result);
}
