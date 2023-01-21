#include <helpers.h>
#include <wiringPi.h>
#include <softPwm.h>

class GpioPWM
{
private:
    const uchar fanPin = 5; // BCM GPIO 24
    const uchar resistPin = 4; // BCM GPIO 23
    const int RANGE = 100;
    int intensity = 0;
public:
    GpioPWM(void);
    void operator<<(const int intensity_){
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
    };
};