#include "uartController.h"

UartController::UartController(uchar matric[4]){
    memcpy(matricula, matric, 4);
    endereco = 1;
    filestream = -1;
    filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;     //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(filestream, TCIFLUSH);
    tcsetattr(filestream, TCSANOW, &options);
}

uchar UartController::getModbusCode(uchar subcode){
    return (subcode > 0xc3 ? SEND_CODE : RECV_CODE);
}

uchar UartController::handleData(uchar* p_tx_buffer, uchar subcode){

    switch (subcode)
    {
    case SEND_CTR:
        int ctr = 87654;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;
    case SEND_REF_TEMP:
        float ctr = 3.1416;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;
    case SEND_POW_STATE:
        *p_tx_buffer = forno.power;
        return 1;
    case SEND_DASH_STATE:
        *p_tx_buffer = forno.dash;
        return 1;
    case SEND_PLAY_STATE:
        *p_tx_buffer = forno.play;
        return 1;
    case SEND_ROOM_TEMP:
        int ctr = 3.618;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;    
    case RECV_SELF_TEMP:
    case RECV_REL_TEMP:
    case RECV_CMD:
        return 0;
    default:
        printf("send_rx() erro\n");
        return 0;
    }

}

uchar UartController::handleRecv(uchar* p_tx_buffer, uchar subcode){

    switch (subcode)
    {
    case SEND_CTR:
        //pid
        int ctr = 87654;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;
    case SEND_REF_TEMP:
        float ctr = 3.1416;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;
    case SEND_POW_STATE:
        *p_tx_buffer = forno.power;
        return 1;
    case SEND_DASH_STATE:
        *p_tx_buffer = forno.dash;
        return 1;
    case SEND_PLAY_STATE:
        *p_tx_buffer = forno.play;
        return 1;
    case SEND_ROOM_TEMP:
        int ctr = 3.618;
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;    
    case RECV_SELF_TEMP:
    case RECV_REL_TEMP:
    case RECV_CMD:
        return 0;
    default:
        printf("send_rx() erro\n");
        return 0;
    }

}

class Forno
{
private:
public:
    uchar power;
    uchar dash;
    uchar play;
};


Forno forno;
void UartController::send_tx(uchar command, const char* msg){

    uchar tx_buffer[13];
    uchar *p_tx_buffer;
    p_tx_buffer = tx_buffer;
    *p_tx_buffer++ = endereco;
    *p_tx_buffer++ = getModbusCode(command);
    *p_tx_buffer++ = command;
    memcpy(p_tx_buffer, matricula, 4);
    p_tx_buffer+=4;
    uchar datasize = handleData(p_rx_buffer, command);
    if(datasize == -1) return;
    p_tx_buffer += datasize;
    uchar msgsize = p_tx_buffer - tx_buffer;
    ushort crc = calcula_CRC(tx_buffer, msgsize);
    memcpy(p_tx_buffer, &crc , 2);
    p_tx_buffer+=2;
    msgsize += 2;

    printf("Buffers de memória criados!\n");
    
    if (filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(filestream, tx_buffer, msgsize);
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }

    sleep(1);
    //----- CHECK FOR ANY RX BYTES -----
    if (filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        uchar rx_buffer[256];
        uchar* p_rx_buffer;
        p_rx_buffer = rx_buffer;
        int rx_length = read(filestream, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            struct modbus_header head;
            memcpy(&head, p_rx_buffer, sizeof(head));
            p_rx_buffer += sizeof(head);
            uchar datasize = handleRecv(p_rx_buffer, command);
            if(datasize == -1) return;
            p_rx_buffer += datasize;
            uchar msgsize = p_rx_buffer - rx_buffer;
            ushort crc = calcula_CRC(rx_buffer, msgsize);
            ushort mcrc;
            memcpy(&mcrc, p_rx_buffer, 2);
            if(crc == mcrc){
                
            }
        }
    }
    return;
}
void UartController::recv_rx(uchar command){

    uchar tx_buffer[20];
    uchar *p_tx_buffer;
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = endereco;
    *p_tx_buffer++ = RECV_CODE;
    *p_tx_buffer++ = command;
    ushort crc = Compute_CRC16_Simple(tx_buffer);
    memcpy(p_tx_buffer, &crc, 2);    
    p_tx_buffer += 2;
    // *p_tx_buffer++ = matricula[0];
    // *p_tx_buffer++ = matricula[1];
    // *p_tx_buffer++ = matricula[2];
    // *p_tx_buffer++ = matricula[3];

    printf("Buffers de memória criados!\n");
    
    if (filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("escrito.\n");
        }
    }
    sleep(1);
    //----- CHECK FOR ANY RX BYTES -----
    if (filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        uchar rx_buffer[256];
        int rx_length = read(filestream, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            if(command == RECV_INT){
                int num = 0;
                memcpy(&num, rx_buffer, 4);    
                printf("%i Bytes lidos : %d\n", rx_length, num);

            }else if(command == RECV_FLOAT){
                float num = 0;
                memcpy(&num, rx_buffer, 4);    
                printf("%i Bytes lidos : %f\n", rx_length, num);

            }else if(command == RECV_STRING){
                printf("%i Bytes lidos : %s\n", rx_length, rx_buffer);

            }else{
                printf("recv_rx() erro\n");
            }
        }
    }

    // sleep(1);
    return;
}

void UartController::close_it(){
    close(filestream);
    return;
}

int handle_interface(const char* type,const char* msg){
    int cmd = 0xb0;
    if(msg[0] == '0' && msg[1] == '0'){
        cmd -= 0x10;
    }
    
    switch(type[0]){
        case 'i':
            cmd += 1;
        break;
        case 'f':
            cmd += 2;        
        break;
        case 's':
            cmd += 3;
        break;
    }

    return cmd;
}



