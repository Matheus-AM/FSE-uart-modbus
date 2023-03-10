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
    int operator<<(const int intensity_);
};