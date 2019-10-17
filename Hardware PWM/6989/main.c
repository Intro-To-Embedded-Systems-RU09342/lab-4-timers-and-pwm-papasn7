#include <msp430.h>

volatile int duty = 500; //duty cycle value

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;        //holds watchdog timer

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    P9DIR |= BIT7;  //sets pin9.7 to output
    P1DIR |= BIT0;  //set P1.0 to output
    P1SEL0 |= BIT0;  //P1.0 set to pwm
    P1DIR &= ~BIT1; //sets P1.1 to input
    P1OUT =  BIT1;  //enables pull up resistor
    P1REN |= BIT1;  //enables pull up resistor
    P1IE |= BIT1;   //enables interrupt
    P1IES |= BIT1;  //enable interrupt
    P1IFG &= ~BIT1; //flag set off at pin1.1

    TA0CCR0 = 1000;       //100% duty cycle
    TA0CCTL1 = OUTMOD_7;  //set output mode
    TA0CCR1 = duty;       //50% duty cycle
    TA0CTL = TASSEL_2 + MC_1; //set up timer

     __bis_SR_register(LPM0_bits + GIE);  //enters lowpowermode0 w/ interrupt
    }

#pragma vector=PORT1_VECTOR       //interrupt triggers on button press
__interrupt void Button(void){
          P9OUT ^= BIT7;    //debugging toggle
 if (duty == 1000){        //if at 100% duty cycle
        duty = 0;          //sets duty cycle back to 0
    } else {
        duty += 100;   // increment duty cycle by 10%
    }
    TA0CCR1 = duty;          //sets duty cycle
    P1IFG = ~BIT1;           // interrupt flag off
}
