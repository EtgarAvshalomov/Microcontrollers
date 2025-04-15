#include "LCD_Utilities.h"

void LCDInit ()
{
    PJSEL0 = BIT4 | BIT5;                   // For LFXT

    // Initialize LCD segments 0 - 21; 26 - 43
    LCDCPCTL0 = 0xFFFF;
    LCDCPCTL1 = 0xFC3F;
    LCDCPCTL2 = 0x0FFF;

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY >> 8;                  // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do
    {
      CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
      SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    // Initialize LCD_C
    // ACLK, Divider = 1, Pre-divider = 16; 4-pin MUX
    LCDCCTL0 = LCDDIV__1 | LCDPRE__16 | LCD4MUX | LCDLP;

    // VLCD generated internally,
    // V2-V4 generated internally, v5 to ground
    // Set VLCD voltage to 2.60v
    // Enable charge pump and select internal reference for it
    LCDCVCTL = VLCD_1 | VLCDREF_0 | LCDCPEN;

    LCDCCPCTL = LCDCPCLKSYNC;               // Clock synchronization enabled

    LCDCMEMCTL = LCDCLRM;                   // Clear LCD memory
      LCDCCTL0 |= LCDON;

 }

void LCD_All_On(){

    int i;
    char *LCD_ptr = (char*) 0x0A20; // LCD_ptr points to 0x0A20;
    for (i = 0;i<21;i++)
        *LCD_ptr++ = 0xFF;

}

void LCD_All_Off(){

    int i;
    char *LCD_ptr = (char*) 0x0A20; // LCD_ptr points to 0x0A20;
    for (i = 0;i<21;i++)
        *LCD_ptr++ = 0x00;
}


void Display_Number (long n){

    int i=0;
    const unsigned char lcd_num[10] = { 0xFC, 0x60, 0xDB, 0xF3, 0x67, 0xB7, 0xBF, 0xE4,  0xFF, 0xF7};
    char *Ptr2Num[7] = {
        (char *)0x0,
        (char *)0x0A29,
        (char *)0x0A25,
        (char *)0x0A23,
        (char *)0x0A32,
        (char *)0x0A2E,
        (char *)0x0A27
    };

    LCD_All_Off();
    if(n<0){
       n=-n;
       SIGN_LESS_ON; }
    if (n>999999){
       SIGN_ERR_ON;
       return; }
    do {
      *Ptr2Num[6-i] = lcd_num[n%10];
       i++;
       n = n/10;
    }while(n);
}
