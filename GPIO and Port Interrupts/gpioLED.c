#include <msp430.h>
void wait(int ms) {
          int static i;
          for(i=ms; i>0; i--)
              __delay_cycles(1000);
}
#define SETBIT(m, k) ((m) =  (m) | ( 1 << (k)))
#define RESETBIT(m, k) ((m) =  (m) & ~( 1 << (k)))
#define TOGGLEBIT(m, k) ((m) =  (m) ^ (1 << (k)))
#define GETBIT(m, k) (((m) & ( 1 << (k) )) !=0 )

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void) {
    wait(200);       //delay
    switch (__even_in_range(P1IV, P1IV_P1IFG7))    //check P1IV
    {
        case P1IV_NONE:
         break;
        case P1IV_P1IFG1:
            if  (!(GETBIT(P1IN, 1))) //check for Fault alarm
                TOGGLEBIT(P9OUT,7);
            break;
        case P1IV_P1IFG2:
            if  (!(GETBIT(P1IN, 2))) //check for Fault alarm
                TOGGLEBIT(P1OUT,0);
            break;
        default:
        break;
    }
}

void main()  {
     WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
     PM5CTL0 &= ~LOCKLPM5; // Clear locked IO Pins

     SETBIT (P1DIR,0);     // Set P1.0 to output-RED LED, all rest - INPUT
     RESETBIT(P1OUT,0);    // Reset RED LED
     SETBIT (P9DIR,7);     // Set P9.7 to output-GREEN LED, all rest - INPUT
     RESETBIT(P9OUT,7);    // Reset GREEN LED

     P1REN  = 0xFE;          // Enable P1.1 to P1.7 Pull up Resistor
     P1OUT  = 0xFE;        // Set Pull up to BIT1 to BIT7

     SETBIT (P1IES,0);     // Set Interrupt on High-to-low Transition
     P1IFG = 0x00;         // Reset IFG
     SETBIT(P1IE,1);       // Enable P1.1 Interrupt
     SETBIT(P1IE,2);       // Enable P1.2 Interrupt
     __bis_SR_register(GIE); //Enable global interrupts
}
