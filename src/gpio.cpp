#include <gpio.h>
#include <helpers.h>

GpioPWM::GpioPWM()
{
    wiringPiSetup();                /* initialize wiringPi setup */
    pinMode(fanPin, OUTPUT);       /* set GPIO as output */
    softPwmCreate(fanPin, 0, RANGE); /* set PWM channel along with range*/
    pinMode(resistPin, OUTPUT);       /* set GPIO as output */
    softPwmCreate(resistPin, 0, RANGE); /* set PWM channel along with range*/
}
void GpioPWM::operator<<(const int intensity_){
    if (intensity_ < 0){
        if(intensity == -intensity_) return;
        intensity = (-intensity_ < RANGE ? -intensity_: RANGE);
        intensity = (-intensity_ > 40 ? -intensity_: 40);
        softPwmWrite(fanPin, intensity); /* change the value of PWM */
        softPwmWrite(resistPin, 0); /* change the value of PWM */
    }else{
        if(intensity == intensity_) return;
        intensity = (intensity_ < RANGE ? intensity_: RANGE);
        softPwmWrite(fanPin, 0); /* change the value of PWM */
        softPwmWrite(resistPin, intensity);
    }
}
