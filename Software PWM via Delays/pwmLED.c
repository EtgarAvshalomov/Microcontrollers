#include <msp430.h>
#define SETBIT(m, k) ((m) =  (m) | ( 1 << (k)))
#define TOGGLEBIT(m, k) ((m) =  (m) ^ (1 << (k)))
#define GETBIT(m, k) (((m) & ( 1 << (k) )) !=0 )
#define RESETBIT(m, k) ( (m) = (m) & (~(1 << (k))))

void wait(unsigned int cycles) {
    while (cycles--) {
        __delay_cycles(100);
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Clear locked IO Pins

    SETBIT(P1DIR,0);               // Set P1.0 to output direction
    P1OUT  = 0xFE;                 // and connect them to pull ups

    const unsigned int T = 250;
    int i = 0;
    while (1) {
     RESETBIT(P1OUT,0);
     wait(i); // Turned off for i * 100 (us)
     SETBIT(P1OUT,0);
     wait(T-i); // Turned on for (T-i) * 100 (us)
     if ((T-i) == 0) i = 0;
     else i++;
    }

    return 0;
}