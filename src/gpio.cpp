#include <helpers.h>
#include <gpio.h>

GpioPWM::GpioPWM()
{
    wiringPiSetup();                /* initialize wiringPi setup */
    pinMode(fanPin, OUTPUT);       /* set GPIO as output */
    softPwmCreate(fanPin, 0, RANGE); /* set PWM channel along with range*/
    pinMode(resistPin, OUTPUT);       /* set GPIO as output */
    softPwmCreate(resistPin, 0, RANGE); /* set PWM channel along with range*/
}
