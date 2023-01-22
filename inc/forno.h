#include <helpers.h>
#include <uartController.h>
#include <i2cbme280.h>
#include <gpio.h>
#include <pid.h>

class Forno
{
private:
    float temp_ambiente;
    float temp_self;
    float temp_ref;
    uchar power;
    uchar dash;
    uchar play;


    const static uchar POWER_ON_CODE = 0xA1;
    const static uchar POWER_OFF_CODE = 0xA2;
    const static uchar PLAY_CODE = 0xA3;
    const static uchar CANCEL_CODE = 0xA4;
    const static uchar MENU_CODE = 0xA5;

public:
    GpioPWM pwm;
    Forno(uchar matricula[4]);
    UartController* uart;    
    void handleUserCmd(int user_cmd);
    int refreshCmd();
    void playIt();
    void stopIt();
    int isPlaying();
    void finishIt();
};