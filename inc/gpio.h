#include <helpers.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <wiringPi.h>
#include <softPwm.h>
#ifdef __cplusplus
}
#endif
class GpioPWM
{
private:
    const uchar fanPin = 5; // BCM GPIO 24
    const uchar resistPin = 4; // BCM GPIO 23
    const int RANGE = 100;
    int intensity = 0;
public:
    GpioPWM(void);
    void operator<<(const int intensity_);
};