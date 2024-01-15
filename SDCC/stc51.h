// for the Small Device C Compiler (SDCC)

#ifndef __STC51_H__
#define __STC51_H__

/*
#define SET BIT 7        __sfr __at |= 0x80        // Set bit 7 of the __sfr __at
#define SET BIT 6        __sfr __at |= 0x40        // Set bit 6 of the __sfr __at
#define SET BIT 5        __sfr __at |= 0x20        // Set bit 5 of the __sfr __at
#define SET BIT 4        __sfr __at |= 0x10        // Set bit 4 of the __sfr __at
#define SET BIT 3        __sfr __at |= 0x08        // Set bit 3 of the __sfr __at
#define SET BIT 2        __sfr __at |= 0x04        // Set bit 2 of the __sfr __at
#define SET BIT 1        __sfr __at |= 0x02        // Set bit 1 of the __sfr __at
#define SET BIT 0        __sfr __at |= 0x01        // Set bit 0 of the __sfr __at

#define CLR BIT 7        __sfr __at &= 0x7F        // Clear bit 7 of the __sfr __at
#define CLR BIT 6        __sfr __at &= 0xBF        // Clear bit 6 of the __sfr __at
#define CLR BIT 5        __sfr __at &= 0xDF        // Clear bit 5 of the __sfr __at
#define CLR BIT 4        __sfr __at &= 0xEF        // Clear bit 4 of the __sfr __at
#define CLR BIT 3        __sfr __at &= 0xF7        // Clear bit 3 of the __sfr __at
#define CLR BIT 2        __sfr __at &= 0xFB        // Clear bit 2 of the __sfr __at
#define CLR BIT 1        __sfr __at &= 0xFD        // Clear bit 1 of the __sfr __at
#define CLR BIT 0        __sfr __at &= 0xFE        // Clear bit 0 of the __sfr __at
*/

#define SET_T0x12       AUXR |= 0x80        // The clock source of Timer 0 is SYSclk/1
#define SET_T1x12       AUXR |= 0x40        // The clock source of Timer 1 is SYSclk/1
#define SET_UART_M0x6   AUXR |= 0x20        // The baud-rate of UART1 in mode 0 is SYSclk/2
#define SET_T2R         AUXR |= 0x10        // Run Timer 2
#define SET_T2CT        AUXR |= 0x08        // Timer 2 configured as counter
#define SET_T2x12       AUXR |= 0x04        // The clock source of Timer 2 is SYSclk/1
#define SET_EXTRAM      AUXR |= 0x02        // On-chip auxiliary RAM is disabled
#define SET_S1ST2       AUXR |= 0x01        // Select Timer 2 as the baud-rate generator for UART1

#define CLR_T0x12       AUXR &= 0x7F        // The clock source of Timer 0 is SYSclk/12
#define CLR_T1x12       AUXR &= 0xBF        // The clock source of Timer 1 is SYSclk/12
#define CLR_UART_M0x6   AUXR &= 0xDF        // The baud-rate of UART1 in mode 0 is SYSclk/12
#define CLR_T2R         AUXR &= 0xEF        // Stop Timer 2
#define CLR_T2_CT       AUXR &= 0xF7        // Timer 2 configured as timer
#define CLR_T2x12       AUXR &= 0xFB        // The clock source of Timer 2 is SYSclk/12
#define CLR_EXTRAM      AUXR &= 0xFD        // On-chip auxiliary RAM is enabled
#define CLR_S1ST2       AUXR &= 0xFE        // Select Timer 1 as the baud-rate generator for UART1

#define SET_S2SM0       S2CON |= 0x80       // UART2 Mode 2: 9-bit UART, variable baud-rate
#define SET_S2SM2       S2CON |= 0x20       // Enable UART2 automatic address recognition feature 
#define SET_S2REN       S2CON |= 0x10       // Enable UART2 receive
#define SET_S2TB8       S2CON |= 0x08       // Set the ninth data bit to be transmitted in mode 3 
#define SET_S2RB8       S2CON |= 0x04       // Set the ninth data bit received in mode 3 
#define SET_S2TI        S2CON |= 0x02       // Set UART2 Transmit Interrupt flag 
#define SET_S2RI        S2CON |= 0x01       // Set UART2 Receive Interrupt flag 

#define CLR_S2SM0       S2CON &= 0x7F       // UART2 Mode 0: 8-bit UART, variable baud-rate
#define CLR_S2SM2       S2CON &= 0xDF       // Disable UART2 automatic address recognition feature 
#define CLR_S2REN       S2CON &= 0xEF       // Disable UART2 receive
#define CLR_S2TB8       S2CON &= 0xF7       // Clear the ninth data bit to be transmitted by UART2 in mode 3 
#define CLR_S2RB8       S2CON &= 0xFB       // Clear the ninth data bit received by UART2 in mode 3 
#define CLR_S2TI        S2CON &= 0xFD       // Clear UART2 Transmit Interrupt flag 
#define CLR_S2RI        S2CON &= 0xFE       // Clear UART2 Receive Interrupt flag 

#define SET_S3SM0       S3CON |= 0x80       // UART3 Mode 3: 9-bit UART, variable baud-rate
#define SET_S3ST3       S3CON |= 0x40       // Select Timer 3 as the baud-rate generator of UART3
#define SET_S3SM2       S3CON |= 0x20       // Enable UART3 automatic address recognition feature 
#define SET_S3REN       S3CON |= 0x10       // Enable UART3 receive
#define SET_S3TB8       S3CON |= 0x08       // Set the ninth data bit to be transmitted by UART3 in mode 3 
#define SET_S3RB8       S3CON |= 0x04       // Set the ninth data bit received by UART3 in mode 3 
#define SET_S3TI        S3CON |= 0x02       // Set UART3 Transmit Interrupt flag 
#define SET_S3RI        S3CON |= 0x01       // Set UART3 Receive Interrupt flag 

#define CLR_S3SM0       S3CON &= 0x7F       // UART3 Mode 0: 8-bit UART, variable baud-rate
#define CLR_S3ST3       S3CON &= 0xBF       // Select Timer 2 as the baud-rate generator of UART3
#define CLR_S3SM2       S3CON &= 0xDF       // Disable UART3 automatic address recognition feature 
#define CLR_S3REN       S3CON &= 0xEF       // Disable UART3 receive 
#define CLR_S3TB8       S3CON &= 0xF7       // Clear the ninth data bit to be transmitted by UART4 in mode 3 
#define CLR_S3RB8       S3CON &= 0xFB       // Clear the ninth data bit received by UART4 in mode 3 
#define CLR_S3TI        S3CON &= 0xFD       // Clear UART3 Transmit Interrupt flag 
#define CLR_S3RI        S3CON &= 0xFE       // Clear UART3 Receive Interrupt flag 

#define SET_S4SM0       S4CON |= 0x80       // UART4 Mode 3: 9-bit UART, variable baud-rate
#define SET_S4ST4       S4CON |= 0x40       // Select T4 as UART4 baud-rate generator
#define SET_S4SM2       S4CON |= 0x20       // Enable UART4 automatic address recognition feature 
#define SET_S4REN       S4CON |= 0x10       // Enable UART4 receive
#define SET_S4TB8       S4CON |= 0x08       // Set the ninth data bit to be transmitted in mode 3 
#define SET_S4RB8       S4CON |= 0x04       // Set the ninth data bit received in mode 3 
#define SET_S4TI        S4CON |= 0x02       // Set UART4 Transmit Interrupt flag  
#define SET_S4RI        S4CON |= 0x01       // Set UART4 Receive Interrupt flag 

#define CLR_S4SM0       S4CON &= 0x7F       // UART4 Mode 0: 8-bit UART, variable baud-rate
#define CLR_S4ST4       S4CON &= 0xBF       // Select T2 as UART4 baud-rate generator
#define CLR_S4SM2       S4CON &= 0xDF       // Disable UART4 automatic address recognition feature 
#define CLR_S4REN       S4CON &= 0xEF       // Disable UART4 receive
#define CLR_S4TB8       S4CON &= 0xF7       // Clear the ninth data bit to be transmitted in mode 3 
#define CLR_S4RB8       S4CON &= 0xFB       // Clear the ninth data bit received in mode 3 
#define CLR_S4TI        S4CON &= 0xFD       // Clear UART4 Transmit Interrupt flag 
#define CLR_S4RI        S4CON &= 0xFE       // Clear UART4 Receive Interrupt flag 

#define SET_T4R         T4T3M |= 0x80       // Run Timer 4
#define SET_T4_CT       T4T3M |= 0x40       // Timer 4 configured as Counter
#define SET_T4x12       T4T3M |= 0x20       // The clock source of Timer 4 is SYSclk/1
#define SET_T4CLKO      T4T3M |= 0x10       // P0.6 is configured for Timer 4 clock output (T4 overflow/2)
#define SET_T3R         T4T3M |= 0x08       // Run Timer 3
#define SET_T3_CT       T4T3M |= 0x04       // Timer 3 configured as Counter
#define SET_T3x12       T4T3M |= 0x02       // The clock source of Timer 3 is SYSclk/1
#define SET_T3CLKO      T4T3M |= 0x01       // P0.4 is configured for Timer 3 clock output (T3 overflow/2)

#define CLR_T4R         T4T3M &= 0x7F       // Stop Timer 4
#define CLR_T4_CT       T4T3M &= 0xBF       // Timer 4 configured as Timer
#define CLR_T4x12       T4T3M &= 0xDF       // The clock source of Timer 4 is SYSclk/12
#define CLR_T4CLKO      T4T3M &= 0xEF       // P0.6 is not configured for Timer 4 clock output
#define CLR_T3R         T4T3M &= 0xF7       // Stop Timer 3
#define CLR_T3_CT       T4T3M &= 0xFB       // Timer 3 configured as Timer
#define CLR_T3x12       T4T3M &= 0xFD       // The clock source of Timer 3 is SYSclk/12
#define CLR_T3CLKO      T4T3M &= 0xFE       // P0.4 is not configured for Timer 3 clock output

#define S2SM0           S2CON & 0x80        // UART2 mode select bit
#define S2SM2           S2CON & 0x20        // UART2 automatic address recognition control bit
#define S2REN           S2CON & 0x10        // UART2 receive enable bit
#define S2TB8           S2CON & 0x08        // The 9th data bit to be transmitted by UART2 in mode 3 
#define S2RB8           S2CON & 0x04        // The 9th data bit received by UART2 in mode 3 
#define S2TI            S2CON & 0x02        // UART2 Transmit Interrupt flag 
#define S2RI            S2CON & 0x01        // UART2 Receive Interrupt flag 

#define S3SM0           S3CON & 0x80        // UART3 mode select bit
#define S3ST3           S3CON & 0x40        // UART3 Timer 3 baud rate generator control bit
#define S3SM2           S3CON & 0x20        // UART3 automatic address recognition control bit
#define S3REN           S3CON & 0x10        // UART3 receive enable bit 
#define S3TB8           S3CON & 0x08        // The 9th data bit transmitted by UART3 in mode 3
#define S3RB8           S3CON & 0x04        // The 9th data bit received by UART3 in mode 3
#define S3TI            S3CON & 0x02        // UART3 Transmit Interrupt flag 
#define S3RI            S3CON & 0x01        // UART3 Receive Interrupt flag 

#define S4SM0           S4CON & 0x80        // UART4 mode select bit
#define S4ST3           S4CON & 0x40        // UART4 Timer 3 baud rate generator control bit
#define S4SM2           S4CON & 0x20        // UART4 automatic address recognition control bit
#define S4REN           S4CON & 0x10        // UART4 receive enable bit 
#define S4TB8           S4CON & 0x08        // The 9th data bit transmitted by UART4 in mode 3 
#define S4RB8           S4CON & 0x04        // The 9th data bit received by UART4 in mode 3
#define S4TI            S4CON & 0x02        // UART4 Transmit Interrupt flag 
#define S4RI            S4CON & 0x01        // UART4 Receive Interrupt flag 

#define SET_ET4         IE2 |= 0x40         // Enable Timer 4 interrupt
#define SET_ET3         IE2 |= 0x20         // Enable Timer 3 iterrupt
#define SET_ES4         IE2 |= 0x10         // Enable UART4 interrupt
#define SET_ES3         IE2 |= 0x08         // Enable UART3 interrupt
#define SET_ET2         IE2 |= 0x04         // Enable Timer 2 interrupt
#define SET_ESPI        IE2 |= 0x02         // Enable SPI interrupt
#define SET_ES2         IE2 |= 0x01         // Enable UART2 interrupt

#define CLR_ET4         IE2 &= 0xBF         // Disable Timer 4 interrupt
#define CLR_ET3         IE2 &= 0xDF         // Disable Timer 3 iterrupt
#define CLR_ES4         IE2 &= 0xEF         // Disable UART4 interrupt
#define CLR_ES3         IE2 &= 0xF7         // Disable UART3 interrupt
#define CLR_ET2         IE2 &= 0xFB         // Disable Timer 2 interrupt
#define CLR_ESPI        IE2 &= 0xFD         // Disable SPI interrupt
#define CLR_ES2         IE2 &= 0xFE         // Disable UART2 interrupt

// Watch Dog Timer
#define WDT_FLAG        WDT_CONTR &  0x80    // This bit is set by hardware to indicate a WDT reset occurred. This bit should be cleared by software.
#define CLR_WDT_FLAG    WDT_CONTR &= 0x7F    // Clear WDT Reset Flag   
#define ENABLE_WDT      WDT_CONTR |= 0x20    // Start Watch Dog Timer
#define DISABLE_WDT     WDT_CONTR &= 0xDF    // Stop Watch Dog Timer
#define RESET_WDT       WDT_CONTR |= 0x10    // Reset Watch Dog Timer

// WDT overflow time for SYSclk=12MHz:
// PS2 PS1 PS0  Pre-scale   Overflow Time
//  0   0   0       2          65.5 mS
//  0   0   1       4         131.0 mS
//  0   1   0       8         262.1 mS
//  0   1   1      16         524.2 mS
//  1   0   0      32        1048.5 mS
//  1   0   1      64        2097.1 mS
//  1   1   0     128        4194.3 mS
//  1   1   1     256        8388.6 mS

// WDT overflow time for SYSclk=11.0592MHz:
// PS2 PS1 PS0  Pre-scale   Overflow Time
//  0   0   0       2          71.1 mS
//  0   0   1       4         142.2 mS
//  0   1   0       8         284.4 mS
//  0   1   1      16         568.8 mS
//  1   0   0      32        1137.7 mS
//  1   0   1      64        2275.5 mS
//  1   1   0     128        4551.1 mS
//  1   1   1     256        9102.2 mS

#define POF PCON & 0x10
#define CLR_POF PCON &= 0xEF

// ____sfr __at __at (0xCD) TH2     ;

__sfr __at(0x84) S4CON;
__sfr __at(0x85) S4BUF;
__sfr __at(0x8E) AUXR ;
__sfr __at(0x8F) AUXR2;
__sfr __at(0x91) P1M1 ;
__sfr __at(0x92) P1M0 ;
__sfr __at(0x93) P0M1 ;
__sfr __at(0x94) P0M0 ;
__sfr __at(0x95) P2M1 ;
__sfr __at(0x96) P2M0 ;
__sfr __at(0x97) PCON2;
__sfr __at(0x9A) S2CON;
__sfr __at(0x9B) S2BUF;
__sfr __at(0x9D) P1ASF;
__sfr __at(0xA1) BUS_SPEED;
__sfr __at(0xA2) AUXR1;
__sfr __at(0xAA) WKTCL;
__sfr __at(0xAB) WKTCH;
__sfr __at(0xAC) S3CON;
__sfr __at(0xAD) S3BUF;
__sfr __at(0xAF) IE2  ;
__sfr __at(0xB1) P3M1 ;
__sfr __at(0xB2) P3M0 ;
__sfr __at(0xB3) P4M1 ;
__sfr __at(0xB4) P4M0 ;
__sfr __at(0xB5) IP2  ;
__sfr __at(0xBA) P_SW2;
__sfr __at(0xBC) ADC_CONTR;
__sfr __at(0xBD) ADC_RES;
__sfr __at(0xBE) ADC_RESL;
__sfr __at(0xC0) P4   ;
__sfr __at(0xC1) WDT_CONTR;
__sfr __at(0xC2) IAP_DATA;
__sfr __at(0xC3) IAP_ADDRH;
__sfr __at(0xC4) IAP_ADDRL;
__sfr __at(0xC5) IAP_CMD;
__sfr __at(0xC6) IAP_TRIG;
__sfr __at(0xC7) IAP_CONTR;
__sfr __at(0xC8) P5   ;
__sfr __at(0xC9) P5M1 ;
__sfr __at(0xCA) P5M0 ;
__sfr __at(0xCB) P6M1 ;
__sfr __at(0xCC) P6M0 ;
__sfr __at(0xCD) SPSTAT;
__sfr __at(0xCE) SPCTL;
__sfr __at(0xCF) SPDAT;
__sfr __at(0xD1) T4T3M;
__sfr __at(0xD2) T4H  ;
__sfr __at(0xD3) T4L  ;
__sfr __at(0xD4) T3H  ;
__sfr __at(0xD5) T3L  ;
__sfr __at(0xD6) T2H  ;
__sfr __at(0xD7) T2L  ;
__sfr __at(0xD8) CCON ;
__sfr __at(0xD9) CMOD ;
__sfr __at(0xDA) CCAPM0;
__sfr __at(0xDB) CCAPM1;
__sfr __at(0xE1) P7M1 ;
__sfr __at(0xE2) P7M0 ;
__sfr __at(0xE8) P6   ;
__sfr __at(0xE9) CL   ;
__sfr __at(0xEA) CCAP0L;
__sfr __at(0xEB) CCAP1L;
__sfr __at(0xF2) PCA_PWM0;
__sfr __at(0xF3) PCA_PWM1;
__sfr __at(0xF9) CH_PCA;
__sfr __at(0xFA) CCAP0H;
__sfr __at(0xFB) CCAP1H;
__sfr __at(0xF1) PWMCFG;
__sfr __at(0xF5) PWMCR;
__sfr __at(0xF6) PWMIF;
__sfr __at(0xF7) PWMFDCR;

#define PWMC        (*(unsigned int  volatile __xdata *)0xfff0)
#define PWMCH       (*(unsigned char volatile __xdata *)0xfff0)
#define PWMCL       (*(unsigned char volatile __xdata *)0xfff1)
#define PWMCKS      (*(unsigned char volatile __xdata *)0xfff2)
#define PWM2T1      (*(unsigned int  volatile __xdata *)0xff00)
#define PWM2T1H     (*(unsigned char volatile __xdata *)0xff00)
#define PWM2T1L     (*(unsigned char volatile __xdata *)0xff01)
#define PWM2T2      (*(unsigned int  volatile __xdata *)0xff02)
#define PWM2T2H     (*(unsigned char volatile __xdata *)0xff02)
#define PWM2T2L     (*(unsigned char volatile __xdata *)0xff03)
#define PWM2CR      (*(unsigned char volatile __xdata *)0xff04)
#define PWM3T1      (*(unsigned int  volatile __xdata *)0xff10)
#define PWM3T1H     (*(unsigned char volatile __xdata *)0xff10)
#define PWM3T1L     (*(unsigned char volatile __xdata *)0xff11)
#define PWM3T2      (*(unsigned int  volatile __xdata *)0xff12)
#define PWM3T2H     (*(unsigned char volatile __xdata *)0xff12)
#define PWM3T2L     (*(unsigned char volatile __xdata *)0xff13)
#define PWM3CR      (*(unsigned char volatile __xdata *)0xff14)
#define PWM4T1      (*(unsigned int  volatile __xdata *)0xff20)
#define PWM4T1H     (*(unsigned char volatile __xdata *)0xff20)
#define PWM4T1L     (*(unsigned char volatile __xdata *)0xff21)
#define PWM4T2      (*(unsigned int  volatile __xdata *)0xff22)
#define PWM4T2H     (*(unsigned char volatile __xdata *)0xff22)
#define PWM4T2L     (*(unsigned char volatile __xdata *)0xff23)
#define PWM4CR      (*(unsigned char volatile __xdata *)0xff24)
#define PWM5T1      (*(unsigned int  volatile __xdata *)0xff30)
#define PWM5T1H     (*(unsigned char volatile __xdata *)0xff30)
#define PWM5T1L     (*(unsigned char volatile __xdata *)0xff31)
#define PWM5T2      (*(unsigned int  volatile __xdata *)0xff32)
#define PWM5T2H     (*(unsigned char volatile __xdata *)0xff32)
#define PWM5T2L     (*(unsigned char volatile __xdata *)0xff33)
#define PWM5CR      (*(unsigned char volatile __xdata *)0xff34)
#define PWM6T1      (*(unsigned int  volatile __xdata *)0xff40)
#define PWM6T1H     (*(unsigned char volatile __xdata *)0xff40)
#define PWM6T1L     (*(unsigned char volatile __xdata *)0xff41)
#define PWM6T2      (*(unsigned int  volatile __xdata *)0xff42)
#define PWM6T2H     (*(unsigned char volatile __xdata *)0xff42)
#define PWM6T2L     (*(unsigned char volatile __xdata *)0xff43)
#define PWM6CR      (*(unsigned char volatile __xdata *)0xff44)
#define PWM7T1      (*(unsigned int  volatile __xdata *)0xff50)        
#define PWM7T1H     (*(unsigned char volatile __xdata *)0xff50)        
#define PWM7T1L     (*(unsigned char volatile __xdata *)0xff51)
#define PWM7T2      (*(unsigned int  volatile __xdata *)0xff52)
#define PWM7T2H     (*(unsigned char volatile __xdata *)0xff52)
#define PWM7T2L     (*(unsigned char volatile __xdata *)0xff53)
#define PWM7CR      (*(unsigned char volatile __xdata *)0xff54)

#endif

