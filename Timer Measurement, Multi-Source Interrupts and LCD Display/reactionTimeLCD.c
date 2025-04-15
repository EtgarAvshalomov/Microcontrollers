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

unsigned char Ptr2Char[26] =
   // A    ,   B  ,   c  ,   d  ,  e    ,   f    g       h     i*      j      k*      L      M*    N*     O      p      q      R*     S       T*     U      V     W*      X*     Y*     Z*
    { 0xEF , 0xFD , 0x9C , 0x7B ,  0x9F , 0x8F , 0xBD , 0x6F , 0x00 , 0x70 , 0x0E , 0x1C , 0x6C , 0x6C , 0xFC , 0xCF , 0xE7 , 0xCF , 0xB7 ,  0x80 , 0x7C , 0x38 , 0x6C , 0x00 , 0x00 , 0x90};


unsigned char Ptr2CharA[26] =
   // A    ,   B  ,   c  ,   d  ,  e    ,   f    g       h     i*      j      k*      L      M*    N*     O      p      q      R*     S       T*     U      V     W*      X*     Y*     Z
    { 0x00 , 0x00 , 0x00 , 0x00 ,  0x00 , 0x00 , 0x00 , 0x00 , 0x50 , 0x00 , 0x22 , 0x00 , 0xA0 , 0x82 , 0x00 , 0x00 , 0x00 , 0x02 , 0x00 ,  0x50 , 0x00 , 0x00 , 0x0A , 0xAA , 0xB0 , 0x28};

char *Ptr2Num[7] = {
    (char *)0x0,
    (char *)0x0A29,
    (char *)0x0A25,
    (char *)0x0A23,
    (char *)0x0A32,
    (char *)0x0A2E,
    (char *)0x0A27
};

char *Ptr2NumA[7] = {
   (char *)0x0,
   (char *)0x0A2A,
   (char *)0x0A26,
   (char *)0x0A24,
   (char *)0x0A33,
   (char *)0x0A2F,
   (char *)0x0A28
};

void Error() {
    *Ptr2Num[1] = Ptr2Char[4];
    *Ptr2Num[2] = Ptr2Char[17];
    *Ptr2NumA[2] = Ptr2CharA[17];
    *Ptr2Num[3] = Ptr2Char[17];
    *Ptr2NumA[3] = Ptr2CharA[17];
    *Ptr2Num[4] = Ptr2Char[14];
    *Ptr2Num[5] = Ptr2Char[17];
    *Ptr2NumA[5] = Ptr2CharA[17];
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer (void) {

    if (GETBIT(TA0CTL,TAIFG)) //Test bit0 (TAIFG)
    {
        TA0CTL = 0;
        RESETBIT(P1OUT, 0);
        LCD_All_Off();
        int i;
        for(i=1; i<=6; i++) { // Show 000000
            *Ptr2Num[i] = Ptr2Char[14];
        }
        *(char *)0x0A22 |= 1; // Show !
        RESETBIT(TA0CTL,0);  //Reset TAIFG
    }
}

int s1_pressed = 0;

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    wait(200);       //delay
    if ((GETBIT(P1IFG, 1) && (GETBIT(P1IN,1)))){
        LCD_All_Off();
        if(s1_pressed == 0) TA0R = 0;
        SETBIT(P1OUT, 0);
        s1_pressed = 1;
        TA0CTL = ACLK + UP_MODE + TAIE;
    }
    if ((GETBIT(P1IFG, 2) && (GETBIT(P1IN,2))))
    {
        TA0CTL = 0;
        if(s1_pressed == 0) {
            LCD_All_Off();
            *(char *)0x0A22 |= 1;
            Error();
        } else {
            RESETBIT(P1OUT, 0);
            s1_pressed = 0;
            Display_Number(TA0R/32);
        }
    }
    // Make sure to reset IFG even if this is a false alarm
    RESETBIT (P1IFG,1);
    RESETBIT (P1IFG,2);
}

void main()  {
    WDTCTL = WDTPW | WDTHOLD;// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;// Clear locked IO Pins

    LCDInit();
    LCD_All_On();

    SETBIT(P1DIR, 0); // Set output Red LED
    RESETBIT(P1OUT, 0);

    SETBIT(P1REN, 1); // Pull Resistor enable
    SETBIT(P1OUT, 1); // Pull-Up
    SETBIT(P1REN, 2); // Pull Resistor enable
    SETBIT(P1OUT, 2); // Pull-Up

    P1IFG = 0x00;

    SETBIT(P1IES, 1); // High to low
    SETBIT(P1IE, 1);  // Interrupt enable
    SETBIT(P1IES, 2); // High to low
    SETBIT(P1IE, 2);  // Interrupt enable

    TA0CCR0 = 32768; //count in seconds

    __bis_SR_register(GIE);
    _BIS_SR(LPM0_bits | GIE);
}
