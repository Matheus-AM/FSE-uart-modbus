#include <forno.h>

Forno::Forno(uchar matricula[4]) : uart(new UartController(matricula))
{
    temp_ambiente = get_home_temp_bme280();

    power = 0;
    dash = 0;
    play = 0;

    read_record(&curva);
    
    dash_count = 0;

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
        stopIt();
        break;
    case MENU_CODE:
        dash = !dash;
        uart->send_tx<int>(SEND_DASH_STATE, (uchar*)&dash);
        dash_count = 0;
        break;
    default:
        break;
    }
}

int Forno::refreshCmd(){
    return uart->send_tx<int>(RECV_CMD, NULL);
}

int Forno::isPlaying(){
    return play;
}


void Forno::playIt(){
    if (!isPlaying()) return;
    if (dash)
    {
        if(dash_count%120 == 0){
            int idx = 1+(dash_count/120);
            idx = (idx > 9 ? 9 : idx);
            printf("curva: %d\n", idx);
            temp_ref = curva.temp[idx];
            printf("curva: %f\n", temp_ref);
            uart->send_tx<float>(SEND_REF_TEMP, (uchar*)&temp_ref);
            pid_atualiza_referencia(temp_ref);
        }
        dash_count++;
    }else{
        int ref_ = uart->send_tx<float>(RECV_REL_TEMP, NULL);
        int ref2_ = temp_ref;
        if(ref_ != ref2_){
            temp_ref = ref_;
            pid_atualiza_referencia(temp_ref);
        }
    }
    
    temp_self = uart->send_tx<float>(RECV_SELF_TEMP, NULL);
    printf("temp_self: %f\n", temp_self);
    double controle = pid_controle((double)temp_self);
    printf("pid: %lf\n", controle);
    int intensity = controle*10;
    if(intensity%10 > 5)intensity++;
    intensity/=10;
    int sinal_controle = pwm<<intensity;
    uart->send_tx<uchar>(SEND_CTR, (uchar*)&sinal_controle);
    return;
}

void Forno::stopIt(){
    int sinal_controle = pwm<<0;
    uart->send_tx<uchar>(SEND_CTR, (uchar*)&sinal_controle);
    uart->send_tx<uchar>(SEND_PLAY_STATE, (uchar*)&sinal_controle);
    uart->send_tx<uchar>(SEND_POW_STATE, (uchar*)&sinal_controle);
    return;
}

void Forno::finishIt(){
    stopIt();
    delete uart;
    return;
}