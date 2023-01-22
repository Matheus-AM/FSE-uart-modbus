#include <stdio.h>
#include <stdlib.h>
#include <string.h>         //Used for UART
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <helpers.h>

//Solicita Temperatura Interna
#define RECV_SELF_TEMP 0xC1
//Solicita Temperatura de Referência
#define RECV_REL_TEMP 0xC2
//Lê comandos do usuário
#define RECV_CMD 0xC3
//Envia sinal de controle Int (4 bytes)  PID
#define SEND_CTR 0xD1
//Envia sinal de Referência Float (4 bytes)
#define SEND_REF_TEMP 0xD2
//Envia Estado do Sistema (Ligado = 1 / Desligado = 0)
#define SEND_POW_STATE 0xD3
//Modo de Controle da Temperatura de referência (Dashboard = 0 / Curva/Terminal = 1) (1 byte)
#define SEND_DASH_STATE 0xD4
//Envia Estado de Funcionamento (Funcionando = 1 / Parado = 0)
#define SEND_PLAY_STATE 0xD5
//Envia Temperatura Ambiente (Float))
#define SEND_ROOM_TEMP 0xD6

class UartController 
{
private:
    int filestream;
    uchar matricula[4];
    uchar endereco;
    
    const static uchar RECV_CODE = 0x23;
    const static uchar SEND_CODE = 0x16;

    int recv_rx(uchar command);
    uchar getModbusCode(uchar subcode);
    uchar handleData(uchar* p_tx_buffer, uchar subcode);
    uchar handleRecv(uchar* p_tx_buffer, uchar subcode);
    
    short CRC16(short crc, char data);
    short calcula_CRC(unsigned char *commands, int size);

public:
    UartController(uchar matric[4]);
    void close_it();



    template<typename T = int> 
    T send_tx(uchar command, const uchar* msg){
        
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
        T res;
        int aux = recv_rx(command);
        memcpy(&res, &aux, 4);
        return res;
    };
};

struct modbus_header
{
    uchar endereco;
    uchar code;
    uchar subcode;
};
