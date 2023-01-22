#include <uartController.h>
#include <gpio.h>
#include <i2cbme280.h>


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
    Forno(uchar matricula[4]);
    UartController* uart;    
    void handleUserCmd(int user_cmd);
i   int refreshCmd();
};

Forno::Forno(uchar matricula[4]) : uart(new UartController(matricula))
{
    temp_ambiente = get_home_temp_bme280();
    power = 0;
    dash = 0;
    play = 0;

    temp_self = uart->send_tx<float>(0xC1, NULL);
    temp_ref = uart->send_tx<float>(0xC2, NULL);
    uart->send_tx<int>(0xD3, power);
    uart->send_tx<int>(0xD4, dash);
    uart->send_tx<int>(0xD5, play);
    uart->send_tx<int>(0xD6, temp_ambiente);
    printf("%f\n", temp_ambiente);
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


int Forno::refreshCmd(){
    return uart.send_tx(0xc3, NULL);
}


int main(int argc, const char * argv[]) {
    uchar matricula[4] = {0x00, 0x03, 0x00, 0x07};

    if (wiringPiSetup() == -1) exit (1);
    Forno forno(matricula);
    GpioPWM pwm;

    // while (1)
    // {
    int user_cmd = forno.refreshCmd();
    if (user_cmd != -1) forno.handleUserCmd(user_cmd);   
        
    //     usleep(500000);
    // }

    usleep(2000000);

    forno.uart->close_it();
    return 0;
}