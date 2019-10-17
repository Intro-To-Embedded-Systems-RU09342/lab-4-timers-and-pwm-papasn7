# Software PWM
This project is meant to make a PWM signal using the timers and compare and control registers. This specifically will blink an LED at a 50% duty cycle, 
and then at each button press the duty cycle increases by 10%. It does this using a timer for debouncing, a button interrupt, and a timer for the PWM.

The difference between the two is that the FR6989 has to have is default high impedance state disabled, and that the button has to have it pullup resistor enabled.