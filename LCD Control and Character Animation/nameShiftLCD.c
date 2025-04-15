#include <msp430.h>
#include "LCD_Utilities.h"

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

void Etgar() {
    LCD_All_Off();

    *Ptr2Num[1] = Ptr2Char[4]; // E
    *Ptr2Num[2] = Ptr2Char[19]; // T
    *(char *)0x0A26 = Ptr2CharA[19];
    *Ptr2Num[3] = Ptr2Char[6]; // G
    *Ptr2Num[4] = Ptr2Char[0]; // A
    *Ptr2Num[5] = Ptr2Char[17]; // R
    *(char *)0x0A2F = Ptr2CharA[17];
}

void Ben() {
    LCD_All_Off();

    *Ptr2Num[1] = Ptr2Char[1]; // B
    *Ptr2Num[2] = Ptr2Char[4]; // E
    *Ptr2Num[3] = Ptr2Char[13]; // N
    *(char *)0x0A24 = Ptr2CharA[13];
}

int state = 0;

void Loop() {
    while(1) {
       wait(1000);

       if (state == 0) {
           int temp = *Ptr2Num[6];
           int tempA = *Ptr2NumA[6];

           int i;
           for (i=1; i<6; i++) {
               *Ptr2Num[7-i] = *Ptr2Num[7-i-1];
               *Ptr2NumA[7-i] = *Ptr2NumA[7-i-1];
           }

           *Ptr2Num[1] = temp;
           *Ptr2NumA[1] = tempA;

       } else {
           int temp = *Ptr2Num[1];
           int tempA = *Ptr2NumA[1];

           int i;
           for (i=1; i<6; i++) {
               *Ptr2Num[i] = *Ptr2Num[i+1];
               *Ptr2NumA[i] = *Ptr2NumA[i+1];
           }

           *Ptr2Num[6] = temp;
           *Ptr2NumA[6] = tempA;
       }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    wait(200);       //delay
    if ((GETBIT(P1IFG, 1) && (GETBIT(P1IN,1))))
    {
        if(state == 0) {
            Ben(); // Show "Ben"
            state = 1;
        }
        else {
            Etgar(); // Show "Etgar"
            state = 0;
        }
    }
    // Make sure to reset IFG even if this is a false alarm
    RESETBIT (P1IFG,1);
}

void main()  {
    WDTCTL = WDTPW | WDTHOLD;// Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;// Clear locked IO Pins

    LCDInit();

    SETBIT(P1REN, 1); // Pull Resistor enable
    SETBIT(P1OUT, 1); // Pull-Up

    P1IFG = 0x00;

    SETBIT(P1IES, 1); // High to low
    SETBIT(P1IE, 1);  // Interrupt enable

    __bis_SR_register(GIE);

    Etgar();

    Loop();
}
