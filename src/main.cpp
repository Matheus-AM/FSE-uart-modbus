#include <uartController.h>
#include <gpio.h>

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
    Forno(/* args */);
    //
    void handleUserCmd(int user_cmd);
    ~Forno();
};

Forno::Forno()
{
    temp_ambiente = 0;
    temp_self = 0;
    temp_ref = 0;
}

Forno::~Forno()
{
}

void Forno::handleUserCmd(int user_cmd){
    switch (user_cmd)
    {
    case POWER_ON_CODE:
        power = 1;
        break;
    case POWER_OFF_CODE:
        power = 0;
        break;
    case PLAY_CODE:
        play = 1;
        break;
    case CANCEL_CODE:
        play = 0;
    case MENU_CODE:
        dash = !dash;
        break;
    default:
        break;
    }
}


int main(int argc, const char * argv[]) {
    uchar matricula[4] = {0x00, 0x03, 0x00, 0x07};
    UartController uart(matricula);
    Forno forno;
    GpioPWM pwm();
    while (1)
    {
        int user_cmd = uart.send_tx(0xc3, 0);
        if (user_cmd != -1) forno.handleUserCmd(user_cmd);   
        
        usleep(500000);
    }
    
    uart.close_it();
    return 0;
}