#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    P1DIR |= BIT0; //set P1.0 to output
    P1DIR |= BIT6; //set P1.6 to output
    P1OUT &= ~BIT6; //Turn LED off

    P1DIR &= ~BIT3; //set P1.3 to input
    P1REN |= BIT3; //enable pull-up resistor
    P1OUT |= BIT3; //enable pull-up resistor
    P1IES |= BIT3; //enable interrupt on low to high edge
    P1IE |= BIT3; //enable interrupt
    P1IFG &= ~BIT3; //clear interrupt flags

    TA1CTL = TASSEL_2 | MC_2; // timer A0 set up
    TA1CCR0 = 64; //set debounce time
    TA1CCTL0 |= CCIE; //enable interrupt
    TA1CTL |= TAIE; //enable interrupts

    TA0CTL = TASSEL_2 | MC_1; // timer A1 set up
    TA0CCR0 = 1000; //100% duty cycle
    TA0CCR1 = 500; //50% duty cycle
    TA0CCTL0 |= CCIE; //enable interrupt
    TA0CCTL1 |= CCIE; //enable interrupt
    TA0CTL |= TAIE; //enable interrupts

    __bis_SR_register(LPM0_bits + GIE); //enter LPM 0 and enable interrupts

   while(1){}
}
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
{
    P1IFG &= ~BIT3; //clear interrupt flag
    TA1R = 0; //reset timer
    TA1CTL = TASSEL_1 | MC_2; //start in continuous mode
}

void __attribute__((interrupt(TIMER1_A0_VECTOR))) timer1_A0 (void)
{
    TA1CTL &= ~TAIFG; //clear interrupt flag
    TA1CTL &= ~MC_3; //stop debouncing

    if(!(P1IN & BIT3) && (P1IES & BIT3)) //if button is pressed
    {
        P1IES &= ~BIT3; //change button interrupt edge
        P1OUT |= BIT6; //turn on LED

        if(TA0CCR1 <= 900) //if the duty cycle is not 100%
        {
            TA0CCR1 += 100; //increase duty cycle by 10%
            TA0CTL |= MC_1; //up mode
        }
        else
        {
            TA0CCR1 = 0; //set duty cycle back to 0
            TA0CTL &= ~MC_3; //stop timer
            P1OUT &= ~BIT0;//turn off LED
        }
    }

    if((P1IN & BIT3) && !(P1IES & BIT3)) // if button is released
    {
        P1IES |= BIT3; //change button interrupt edge
        P1OUT &= ~BIT6; //turn off LED
    }
}

void __attribute__((interrupt(TIMER0_A0_VECTOR))) timer0_A0 (void)
{
    TA0CTL &= ~TAIFG; //clear interrupt flag
    P1OUT |= BIT0; //turn on LED
}

void __attribute__((interrupt(TIMER0_A1_VECTOR))) timer0_A1 (void)
{
    if(TA0IV == 0x02) //if interrupt was generated from CCR1 compare
        P1OUT &= ~BIT0; //turn off LED
}
