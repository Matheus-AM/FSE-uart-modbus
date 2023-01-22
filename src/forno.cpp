#include <forno.h>

Forno::Forno(uchar matricula[4]) : uart(new UartController(matricula))
{
    temp_ambiente = get_home_temp_bme280();

    power = 0;
    dash = 0;
    play = 0;

    temp_self = uart->send_tx<float>(RECV_SELF_TEMP, NULL);
    temp_ref = uart->send_tx<float>(RECV_REL_TEMP, NULL);
    uart->send_tx<int>(SEND_POW_STATE, (uchar*)&power);
    uart->send_tx<int>(SEND_DASH_STATE, (uchar*)&dash);
    uart->send_tx<int>(SEND_PLAY_STATE, (uchar*)&play);
    uart->send_tx<int>(SEND_ROOM_TEMP, (uchar*)&temp_ambiente);

    printf("temp_self %f\n", temp_self);
    printf("temp_ref %f\n", temp_ref);
    printf("temp_ambiente %f\n", temp_ambiente);
    
    pid_atualiza_referencia(temp_ref);
}

void Forno::handleUserCmd(int user_cmd){
    switch (user_cmd)
    {
    case POWER_ON_CODE:
        power = 1;
        uart->send_tx<int>(SEND_POW_STATE, (uchar*)&power);
        break;
    case POWER_OFF_CODE:
        power = 0;
        uart->send_tx<int>(SEND_POW_STATE, (uchar*)&power);
        break;
    case PLAY_CODE:
        if(power == 0) break;
        play = 1;
        uart->send_tx<int>(SEND_PLAY_STATE, (uchar*)&play);
        break;
    case CANCEL_CODE:
        play = 0;
        uart->send_tx<int>(SEND_PLAY_STATE, (uchar*)&play);
        break;
    case MENU_CODE:
        dash = !dash;
        uart->send_tx<int>(SEND_DASH_STATE, (uchar*)&dash);
        break;
    default:
        break;
    }
}

int Forno::refreshCmd(){
    return uart->send_tx<int>(RECV_CMD, NULL);
}

int Forno::playIt(){
    if(play == 0) return 0;
    int ref_ = uart->send_tx<float>(RECV_REL_TEMP, NULL);
    int ref2_ = temp_ref;
    if(ref_ != ref2_)
        pid_atualiza_referencia(temp_ref);

    temp_self = uart->send_tx<float>(RECV_SELF_TEMP, NULL);
    double controle = pid_controle((double)temp_self);
    int res = controle*10;
    if(res%10 > 5)res++;
    res/=10;
    int sinal_controle = pwm<<res;
    uart->send_tx<uchar>(SEND_CTR, sinal_controle);

}