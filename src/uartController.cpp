#include <uartController.h>

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

    int ctr = 87654;
    float ctrf = 3.1416;
    switch (subcode)
    {
    case SEND_CTR:
        memcpy(p_tx_buffer, &ctr, 4);
        return 4;
    case SEND_REF_TEMP:
        memcpy(p_tx_buffer, &ctrf, 4);
        return 4;
    case SEND_POW_STATE:
        *p_tx_buffer = 0;
        return 1;
    case SEND_DASH_STATE:
        *p_tx_buffer = 0;
        return 1;
    case SEND_PLAY_STATE:
        *p_tx_buffer = 0;
        return 1;
    case SEND_ROOM_TEMP:
        memcpy(p_tx_buffer, &ctrf, 4);
        return 4;    
    case RECV_SELF_TEMP:
    case RECV_REL_TEMP:
    case RECV_CMD:
        return 0;
    default:
        printf("send_rx() erro\n");
        return -1;
    }

}

uchar UartController::handleRecv(uchar* p_tx_buffer, uchar subcode){

    // 0 = 0, 1 = int, 2 = float
    switch (subcode)
    {
    case RECV_SELF_TEMP:
    case RECV_REL_TEMP:
    case SEND_ROOM_TEMP:
        return 2;
    case RECV_CMD:
    case SEND_POW_STATE:
    case SEND_DASH_STATE:
    case SEND_PLAY_STATE:
        return 1;
    case SEND_CTR:
    case SEND_REF_TEMP:
        return 0;
    default:
        printf("recv_rx() erro\n");
        return -1;
    }

}

int UartController::send_tx(uchar command, const char* msg){

    uchar tx_buffer[13];
    uchar *p_tx_buffer;
    p_tx_buffer = tx_buffer;
    *p_tx_buffer++ = endereco;
    *p_tx_buffer++ = getModbusCode(command);
    *p_tx_buffer++ = command;
    memcpy(p_tx_buffer, matricula, 4);
    p_tx_buffer+=4;
    uchar datasize = handleData(p_tx_buffer, command);
    if(datasize == -1) return -1;
    memcpy(p_tx_buffer, msg, datasize);
    p_tx_buffer += datasize;
    uchar msgsize = p_tx_buffer - tx_buffer;
    ushort crc = calcula_CRC(tx_buffer, msgsize);
    memcpy(p_tx_buffer, &crc , 2);
    p_tx_buffer+=2;
    msgsize += 2;

    printf("Buffers de memória criados!\n");
    
    if (filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...\n");
        for(int i=0;i<msgsize;i++)printf("%0x ", tx_buffer[i]);
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
    return recv_rx(command);
}
int UartController::recv_rx(uchar command){

    if (filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        uchar rx_buffer[256];
        uchar* p_rx_buffer;
        p_rx_buffer = rx_buffer;
        
        int rx_length = -1;
        while(rx_length < 0)
        {
            rx_length = read(filestream, (void*)rx_buffer, 255);      //Filestream, buffer to store in, number of bytes to read (max)
            // printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
            usleep(100000);
        }
        if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            struct modbus_header head;
            memcpy(&head, p_rx_buffer, sizeof(head));
            p_rx_buffer += sizeof(head);
            uchar dataType = handleRecv(p_rx_buffer, command);
            uchar datasize = 0;
            int data = 0;
            if(dataType == -1) return -1;
            if(dataType != 0){
                datasize = 4;
            }
            memcpy(&data, p_rx_buffer, datasize);
            p_rx_buffer += datasize;
            uchar msgsize = p_rx_buffer - rx_buffer;
            ushort crc = calcula_CRC(rx_buffer, msgsize);
            ushort mcrc;
            memcpy(&mcrc, p_rx_buffer, 2);
            if(crc == mcrc){
                printf("dado: %0x\n", data);
                return data;
            }
            return -1;
        }
    }
}

short UartController::CRC16(short crc, char data)
{
    const int tbl[256] = {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};
    return ((crc & 0xFF00) >> 8) ^ (unsigned short)(tbl[(crc & 0x00FF) ^ (data & 0x00FF)]);
}

short UartController::calcula_CRC(unsigned char *commands, int size) {
	int i;
	short crc = 0;
	for(i=0;i<size;i++) {
		crc = CRC16(crc, commands[i]);
	}
	return crc;
}


void UartController::close_it(){
    close(filestream);
    return;
}

