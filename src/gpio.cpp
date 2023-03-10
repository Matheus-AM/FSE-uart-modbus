#include <helpers.h>
#include <gpio.h>

GpioPWM::GpioPWM()
{
    pinMode(fanPin, OUTPUT);            /* set GPIO as output */
    softPwmCreate(fanPin, 0, RANGE);    /* set PWM channel along with range*/
    pinMode(resistPin, OUTPUT);         /* set GPIO as output */
    softPwmCreate(resistPin, 0, RANGE); /* set PWM channel along with range*/
    softPwmWrite(fanPin, 0);      /* change the value of PWM */
    softPwmWrite(resistPin, 0);      /* change the value of PWM */
}

int GpioPWM::operator<<(const int intensity_)
{
    if (intensity_ < 0)
    {
        if (intensity == -intensity_)
            return intensity_;
        intensity = (-intensity_ < RANGE ? -intensity_ : RANGE);
        intensity = (-intensity_ > 40 ? -intensity_ : 40);
        softPwmWrite(fanPin, intensity); /* change the value of PWM */
        softPwmWrite(resistPin, 0);      /* change the value of PWM */
        return -intensity;
    }
    else
    {
        if (intensity == intensity_)
            return intensity;
        intensity = (intensity_ < RANGE ? intensity_ : RANGE);
        softPwmWrite(fanPin, 0); /* change the value of PWM */
        softPwmWrite(resistPin, intensity);
    }
    return intensity;
};