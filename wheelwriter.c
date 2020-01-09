#include <stdio.h>
#include <reg51.h>
#include <stc51.h>
#include "uart3.h"
#include "uart4.h"
#include "control.h"

#define FALSE 0
#define TRUE  1
#define ON 0                                    // 0 turns the amber LED on
#define OFF 1                                   // 1 turns the amber LED off

unsigned char uSpacesPerChar = 10;              // micro spaces per character (8 for 15cpi, 10 for 12cpi and PS, 12 for 10cpi)
unsigned char uLinesPerLine = 16;               // micro lines per line (12 for 15cpi; 16 for 10cpi, 12cpi and PS)
unsigned int  uSpaceCount = 0;                  // number of micro spaces on the current line (for carriage return)

sbit amberLED = P0^6;                           // amber LED connected to pin 6 0=on, 1=off

//------------------------------------------------------------------------------------------------
// ASCII to Wheelwriter printwheel translation table
// The Wheelwriter printwheel code indicates the position of the character on the printwheel. 
// “a” (code 01) is at the 12 o’clock position of the printwheel. Going counter clockwise, 
// “n” (code 02) is next character on the printwheel followed by “r” (code 03), “m” (code 04),
// “c” (code 05), “s” (code 06), “d” (code 07), “h” (code 08), and so on.
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
// convert printwheel character to ASCII
//------------------------------------------------------------------------------------------------
code const char printwheel2ASCII[96] = {
// a    n    r    m    c    s    d    h    l    f    k    ,    V    _    G    U  
  0x61,0x6E,0x72,0x6D,0x63,0x73,0x64,0x68,0x6C,0x66,0x6B,0x2C,0x56,0x2D,0x47,0x55,
// F    B    Z    H    P    )    R    L    S    N    C    T    D    E    I    A       
  0x46,0x42,0x5A,0x48,0x50,0x29,0x52,0x4C,0x53,0x4E,0x43,0x54,0x44,0x45,0x49,0x41,
// J    O    (    M    .    Y    ,    /    W    9    K    3    X    1    2    0 
  0x4A,0x4F,0x28,0x4D,0x2E,0x59,0x2C,0x2F,0x57,0x39,0x4B,0x33,0x58,0x31,0x32,0x30,
// 5    4    6    8    7    *    $    #    %    ¢    +    ±    @    Q    &    ]
  0x35,0x34,0x36,0x38,0x37,0x2A,0x24,0x23,0x25,0xA2,0x2B,0xB1,0x40,0x51,0x26,0x5D,
// [    ³    ²    º    §    ¶    ½    ¼    !    ?    "    '    =    :    -    ;   
  0x5B,0xB3,0xB2,0xBA,0xA7,0xB6,0xBD,0xBC,0x21,0x3F,0x22,0x60,0x3D,0x3A,0x5F,0x3B,
// x    q    v    z    w    j    .    y    b    g    u    p    i    t    o    e   
  0x78,0x71,0x76,0x7A,0x77,0x6A,0x2E,0x79,0x62,0x67,0x75,0x70,0x69,0x74,0x6F,0x65};

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
    if ((letter > 0x1F) && (letter < 0xC0)) {                   // "letter" must be a printable character
        amberLED = ON;
        send_to_printer_board_wait(0x121);
        send_to_printer_board_wait(0x003);
        send_to_printer_board_wait(ASCII2printwheel[letter-0x20]);// ascii character (-0x20) as index to printwheel table    
        if ((attribute & 0x06) && ((letter!=0x20) || (attribute & 0x02))){// if underlining AND the letter is not a space OR continuous underlining is on
            send_to_printer_board_wait(0x000);                  // advance zero micro spaces
            send_to_printer_board_wait(0x121);
            send_to_printer_board_wait(0x003);
            send_to_printer_board_wait(0x04F);                  // print '_' underscore
        }
        if (attribute & 0x01) {                                 // if the bold bit is set   
            send_to_printer_board_wait(0x001);                  // advance carriage by one micro space
            send_to_printer_board_wait(0x121);
            send_to_printer_board_wait(0x003);
            send_to_printer_board_wait(ASCII2printwheel[letter-0x20]);// re-print the character offset by one micro space
            send_to_printer_board_wait((uSpacesPerChar)-1);     // advance carriage the remaining micro spaces
        } 
        else { // not boldprint
            send_to_printer_board_wait(uSpacesPerChar);      
        }
        uSpaceCount += uSpacesPerChar;                          // update the micro space count
        if (uSpaceCount > 1319) {                               // 1 inch from right stop   
            ww_carriage_return();
        }
        amberLED = OFF;
    }
}

#define FIFTEENCPI 8                                        // number of micro spaces for each character on the 15P printwheel (15 cpi)
#define TWELVECPI 10                                        // number of micro spaces for each character on the 12P printwheel (12 cpi)
#define TENCPI 12                                           // number of micro spaces for each character on the 10P printwheel (10 cpi)
//------------------------------------------------------------------------------------------------
// set micro spaces per character and micro lines per line values according to the printwheel in use
// microspace = 1/120th inch, micro line = 1/96th inch
//------------------------------------------------------------------------------------------------
void ww_set_printwheel(unsigned char pw) {
    switch(pw) {
        case FIFTEENCPI:                                    // 15P printwheel (15 cpi)
            uSpacesPerChar = 8;                             // 8 micro spaces/character
            uLinesPerLine = 12;                             // 12 micro lines/full line
            break;
        case TWELVECPI:                                     // 12P printwheel (12 cpi)
            uSpacesPerChar = 10;                            // 10 micro spaces/character
            uLinesPerLine = 16;                             // 16 micro lines/full line
            break;
        case TENCPI:                                        // 10P printwheel (10 cpi)
            uSpacesPerChar = 12;                            // 12 micro spaces/character
            uLinesPerLine = 16;                             // 16 micro lines/full line
            break;
        default:
            uSpacesPerChar = 10;                            // 10 micro spaces/character
            uLinesPerLine = 16;                             // 16 micro lines/full line
    }
}

//--------------------------------------------------------------------------------------------------
// decodes the 9 bit words received from the Wheelwriter Function Board on the BUS when keys are
// pressed and returns the equivalent ASCII character (if there is one). typically a sequence of a 
// minimum of three words (sometimes more, depending on the key) is required to decode each keypress.
// Call this function for each word received from the Function Board. Intermediate words return zeros.
//--------------------------------------------------------------------------------------------------
char ww_decode_keys(unsigned int WWdata) {
    static char keystate = 0;
    char result;

    result = 0;
    switch(keystate) {
        case 0:                                             // waiting for first data word from Wheelwriter...
            if (WWdata == 0x121)                            // all commands must start with 0x121...
               keystate = 1;
            break;
        case 1:                                             // 0x121 has been received...
            switch(WWdata) {
                case 0x003:                                 // 0x121,0x003 is start of alpha-numeric character sequence
                    keystate = 2;
                    break;
                case 0x004:                                 // 0x121,0x004 is start of erase sequence
                    keystate = 0;
                    result = BS;   
                    break;
                case 0x005:                                 // 0x121,0x005 is start of vertical movement sequence
                    keystate = 6;
                    break;          
                case 0x006:                                 // 0x121,0x006 is start of horizontal movement sequence
                    keystate = 3;
                    break;
                case 0x00E:                                 // 0x121,0x00E is the start of a code key sequence
                    keystate = 7;
                    break;
                default:
                    keystate = 0;
        } // switch(WWdata)
            break;
        case 2:                                             // 0x121,0x003 has been received...          
            keystate = 0;
            if (WWdata)                                     // 0x121,0x003,printwheel code
               result = printwheel2ASCII[(WWdata-1)];       // get the ASCII code from the table
            else
               result = SP;                                 // 0x121,0x003,0x000 is the sequence for SPACE
            break;
        case 3:                                             // 0x121,0x006 has been received...
            if (WWdata & 0x080)                             // if bit 7 is set...         
               keystate = 4;                                // 0x121,0x006,0x080 is horizontal movement to the right...
            else                                            // else...
               keystate = 5;                                // 0x121,0x006,0x000 is horizontal movement to the left...
            break;
        case 4:                                             // 0x121,0x006,0x080 has been received, move carrier to the right...
            keystate = 0;
            if (WWdata>uSpacesPerChar)                      // if more than one space, must be horizontal tab
                result = HT;
            else 
                result = SP;
            break;
        case 5:                                             // 0x121,0x006,0x000 has been received, move carrier to the left...
            keystate = 0;
            if (WWdata == uSpacesPerChar) 
               result = BS;
            break;
        case 6:                                             // 0x121,0x005 has been received, move paper vertically...
            keystate = 0;
            if ((WWdata&0x1F) == uLinesPerLine)
               result = CR;                                 // 0x121,0x005,0x090 is the sequence for paper up one line (for 10P, 12P and PS printwheels)
            break;
        case 7:                                             // 0x121,0x00E has ben received (code key combination)
            keystate = 0;
            // convert code key combinations into control keys i.e. code c is converted into control c
            switch(WWdata & 0x17F) {// bit 7 is cleared on WW3, set on WW6
                case 0x002:	        // Code Q
                    result = DC1;   // converted to Control Q
                    break;
                case 0x004:	        // Code A
                    result = SOH;   // converted to Control A
                    break;
                case 0x006:	        // Code Z
                    result = SUB;   // converted to Control Z
                    break;
                case 0x00A:	        // Code W
                    result = ETB;   // converted to Control W
                    break;
                case 0x00C:	        // Code S
                    result = DC3;   // converted to Control S
                    break;
                case 0x00E:	        // Code X
                    result = CAN;   // converted to Control X
                    break;
                case 0x012:	        // Code E
                    result = ENQ;   // converted to Control E
                    break;
                case 0x014:	        // Code D
                    result = EOT;   // converted to Control D
                    break;
                case 0x016:	        // Code C
                    result = ETX;   // converted to Control C
                    break;
                case 0x01A:	        // Code R
                    result = DC2;   // converted to Control R
                    break;
                case 0x01B:	        // Code T
                    result = DC4;   // converted to Control T
                    break;
                case 0x01C:	        // Code F
                    result = ACK;   // converted to Control F
                    break;
                case 0x01D:	        // Code G
                    result = BEL;   // converted to Control G
                    break;
                case 0x01E:	        // Code V
                    result = SYN;   // converted to Control V
                    break;
                case 0x01F:	        // Code B
                    result = STX;   // converted to Control B
                    break;
                case 0x022:	        // Code U
                    result = NAK;   // converted to Control U
                    break;
                case 0x023:	        // Code Y
                    result = EM;    // converted to Control Y
                    break;
                case 0x024:	        // Code J
                    result = LF;    // converted to Control J
                    break;
                case 0x025:	        // Code H
                    result = BS;    // converted to Control H
                    break;
                case 0x026:	        // Code M
                    result = CR;    // converted to Control M
                    break;
                case 0x02A:	        // Code I
                    result = HT;    // converted to Control I
                    break;
                case 0x02C:	        // Code K
                    result = VT;    // converted to Control K
                    break;
                case 0x032:	        // Code O
                    result = SI;    // converted to Control O
                    break;
                case 0x03A:	        // Code P
                    result = DLE;   // converted to Control P
                    break;
                case 0x072:	        // Code N
                    result = SO;    // converted to Control N
                    break;
            } // switch(WWdata & 0x17F)
            break;
    }   // switch(keystate)
    return(result);
}

    