#include <msp430.h>
#include "LCD_Utilities.h"
#define ACLK        0x0100
#define UP_MODE     0x0010

#define SETBIT(m, k) ((m) =  (m) | ( 1 << (k)))
#define RESETBIT(m, k) ((m) =  (m) & ~( 1 << (k)))
#define TOGGLEBIT(m, k) ((m) =  (m) ^ (1 << (k)))
#define GETBIT(m, k) (((m) & ( 1 << (k) )) !=0 )

void wait(int ms) {
    int static i;
    for(i=ms; i>0; i--)
        __delay_cycles(1000);
}

int count = 0;

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer (void) {

    if (GETBIT(TA0CTL,TAIFG)) //Test bit0 (TAIFG)
    {
        if(count == 600000) count = 0;
        if(((count%10000) % 6000 == 0) && count != 0) count += 4000;
        Display_Number(count);
        if(count >= 100) *(char *)0x0A33 |= 4;
        if(count >= 10000) *(char *)0x0A26 |= 4;
        count++;
        RESETBIT(TA0CTL,0);  //Reset TAIFG
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    wait(200);       //delay
    if ((GETBIT(P1IFG, 1) && (GETBIT(P1IN,1))))
    {
        if(TA0CTL != 0) TA0CTL = 0; // Disables Timer
        else TA0CTL = ACLK + UP_MODE + TAIE;
    }
    if ((GETBIT(P1IFG, 2) && (GETBIT(P1IN,2))))
    {
        count=0;
        TA0CTL = ACLK + UP_MODE + TAIE; // Disables Timer
    }
    // Make sure to reset IFG even if this is a false alarm
    RESETBIT (P1IFG,1);
    RESETBIT (P1IFG,2);
}

void main()  {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Clear locked IO Pins

    LCDInit();
    LCD_All_On();

    SETBIT(P1REN, 1); // Pull Resistor enable
    SETBIT(P1OUT, 1); // Pull-Up
    SETBIT(P1REN, 2); // Pull Resistor enable
    SETBIT(P1OUT, 2); // Pull-Up

    P1IFG = 0x00;

    SETBIT(P1IES, 1); // High to low
    SETBIT(P1IE, 1);  // Interrupt enable
    SETBIT(P1IES, 2); // High to low
    SETBIT(P1IE, 2);  // Interrupt enable

    TA0CCR0 = 327; //count to 327

    __bis_SR_register(GIE);
    _BIS_SR(LPM0_bits | GIE);
}
