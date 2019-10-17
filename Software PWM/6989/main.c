#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer


    PM5CTL0 &= ~LOCKLPM5; //disable the GPIO power-on default high-impedance mode

    P1DIR |= BIT0; //set P1.0 to output
    P1SEL0 |= BIT0; //set P1.0 to use PWM
    P9DIR |= BIT7; //set P9.7 to output
    P9OUT &= ~BIT7; //Turn LED off

    P1DIR &= ~BIT1; //set P1.1 to input
    P1REN |= BIT1; //enable pull-up resistor
    P1OUT |= BIT1; //enable pull-up resistor
    P1IES |= BIT1; //enable interrupt on low to high edge
    P1IE |= BIT1; //enable interrupt
    P1IFG &= ~BIT1; //clear interrupt flags

    TA1CTL = TASSEL__SMCLK | MC__CONTINUOUS; // timer A0 set up
    TA1CCR0 = 64; //set debounce time
    TA1CCTL0 |= CCIE; //enable interrupt

    TA0CTL = TASSEL__SMCLK | MC__UP; // timer A1 set up
    TA0CCR0 = 1000; //100% duty cycle
    TA0CCR1 = 500; //50% duty cycle
    TA0CCTL1 |= OUTMOD_7; // resets output at CCR0, sets output at CCR1

    __bis_SR_register(LPM0_bits + GIE); //enter LPM 0 and enable interrupts

   while(1){}
}
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
{
    P1IFG &= ~BIT1; //clear interrupt flag
    TA1R = 0; //reset timer
    TA1CTL = TASSEL__SMCLK | MC__CONTINUOUS; //start in continuous mode
}

void __attribute__((interrupt(TIMER1_A0_VECTOR))) timer1_A0 (void)
{
    TA1CTL &= ~TAIFG; //clear interrupt flag
    TA1CTL &= ~MC_3; //stop debouncingr

    if(!(P1IN & BIT1) && (P1IES & BIT1)) //if button is pressed
    {
        P1IES &= ~BIT1; //change button interrupt edge
        P9OUT |= BIT7; //turn on LED

        if(TA0CCR1 <= 900) //if the duty cycle is not 100%
        {
            TA0CCR1 += 100; //increase duty cycle by 10%
            TA0CTL |= MC_1; //up mode
            TA0CCTL1 |= OUTMOD_7; // change output mode
        }
        else
        {
            TA0CCR1 = 0; //set duty cycle back to 0
            TA0CTL &= ~MC_3; //stop timer
            TA0CCTL1 &= ~OUTMOD_7; //turn output off
        }
    }

    if((P1IN & BIT1) && !(P1IES & BIT1)) // if button is released
    {
        P1IES |= BIT1; //change button interrupt edge
        P9OUT &= ~BIT7; //turn off LED
    }
}
