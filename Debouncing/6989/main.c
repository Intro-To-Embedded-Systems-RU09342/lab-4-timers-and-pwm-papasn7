#include <msp430.h>

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Disable the GPIO power-on default high-impedance mode
    PM5CTL0 &= ~LOCKLPM5;

    P1DIR |= BIT0; //set P1.0 to output
    P1OUT &= ~BIT0; //turn off LED

    P1DIR &= ~(BIT1); //set P1.1 input
    P1REN |= BIT1; //enable pull-up resistor
    P1IE |= BIT1; //enable the interrupt on P1.1
    P1IES |= ~BIT1; //set to flag on high to low
    P1IFG &= ~BIT1; //clear interrupt flag


    TA0CCR0 = 64; //set desired debouncing time
    TA0CCTL0 |= CCIE; //enable interrupts
    TA0CTL |= TASSEL__SMCLK + MC__CONTINUOUS; //timer0 set up

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // enable interrupts in LPM0

    while(1){
        }
}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) Timer_A0 (void)
{//ISR
    TA0CTL &= ~TAIFG; //clear interrupt flag
    TA0CTL &= ~MC_3; //timer stop

    if(!(P1IN & BIT1)) //if button is pressed
        P1OUT ^= BIT0; //toggle LED
}

void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
{
    P1IFG &= ~BIT1; //clear interrupt flag
    TA0R = 0; //reset timer
    TA0CTL |= MC_2; //start continuous mode
}
