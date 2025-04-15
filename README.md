# TI MSP430 Microcontroller Assignments

## Software PWM via Delays

`pwmLED.txt` - This script dims an LED connected to P1.0 by manually controlling its on and off time using software delays within a loop. This creates a software-based Pulse Width Modulation (PWM) effect, causing the LED's brightness to cycle from fully on down to off repeatedly.

## Timer Measurement, Multi-Source Interrupts and LCD Display

`stopwatch.txt` - This script functions as a stopwatch, using Timer A0 to keep time and displaying it on an LCD. Button S1 starts and stops the timer, while button S2 resets the timer count.

`reactionTimeLCD.txt` - This measures the time between pressing button S1 (lights LED P1.0, starts timer) and button S2 (turns off LED, stops timer, displays time). It uses Timer A0 for timing, displays results on an LCD, and includes error handling for incorrect button order or if the timer exceeds one second.

## LCD Control and Character Animation

`nameShiftLCD.txt` - This script initializes and uses an LCD to display names, toggling between "Etgar" and "Ben" when a button (P1.1) is pressed. It also continuously shifts the characters of the currently displayed name in a circular animation.

## GPIO and Port Interrupts

`gpioLED.txt` - This script configures two buttons (P1.1, P1.2) as inputs with interrupts enabled. Pressing a button triggers its corresponding interrupt, which then toggles the state of an LED (P1.0 or P9.7).
