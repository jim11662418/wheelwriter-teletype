// for the Small Device C Compiler (SDCC)

#ifndef __REG51_H__
#define __REG51_H__

/*  BYTE Register  */
__sfr __at(0x80) P0;
__sfr __at(0x90) P1;
__sfr __at(0xA0) P2;
__sfr __at(0xB0) P3;
__sfr __at(0xD0) PSW;
__sfr __at(0xE0) ACC;
__sfr __at(0xF0) B;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x87) PCON;
__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0xA8) IE;
__sfr __at(0xB8) IP;
__sfr __at(0x98) SCON;
__sfr __at(0x99) SBUF;


/*  BIT Register  */
/*  PSW   */
__sbit __at (0xD7) CY;
__sbit __at (0xD6) AC;
__sbit __at (0xD5) F0;
__sbit __at (0xD4) RS1;
__sbit __at (0xD3) RS0;
__sbit __at (0xD2) OV;
__sbit __at (0xD0) P;

/*  TCON  */
__sbit __at (0x8F) TF1;
__sbit __at (0x8E) TR1;
__sbit __at (0x8D) TF0;
__sbit __at (0x8C) TR0;
__sbit __at (0x8B) IE1;
__sbit __at (0x8A) IT1;
__sbit __at (0x89) IE0;
__sbit __at (0x88) IT0;

/*  IE   */
__sbit __at (0xAF) EA;
__sbit __at (0xAC) ES;
__sbit __at (0xAB) ET1;
__sbit __at (0xAA) EX1;
__sbit __at (0xA9) ET0;
__sbit __at (0xA8) EX0;

/*  IP   */ 
__sbit __at (0xBC) PS;
__sbit __at (0xBB) PT1;
__sbit __at (0xBA) PX1;
__sbit __at (0xB9) PT0;
__sbit __at (0xB8) PX0;

/*  P3  */
__sbit __at (0xB7) RD;
__sbit __at (0xB6) WR;
__sbit __at (0xB5) T1;
__sbit __at (0xB4) T0;
__sbit __at (0xB3) INT1 ;
__sbit __at (0xB2) INT0 ;
__sbit __at (0xB1) TXD;
__sbit __at (0xB0) RXD;

/*  SCON  */
__sbit __at (0x9F) SM0;
__sbit __at (0x9E) SM1;
__sbit __at (0x9D) SM2;
__sbit __at (0x9C) REN;
__sbit __at (0x9B) TB8;
__sbit __at (0x9A) RB8;
__sbit __at (0x99) TI;
__sbit __at (0x98) RI;

#endif

