#include <stdio.h>
#include <stdlib.h>
#include <string.h>         //Used for UART
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <helpers.h>

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
    float send_tx(uchar command, const uchar* msg);
    int send_tx(uchar command, const uchar* msg);
    void close_it();

    //Solicita Temperatura Interna
    const static uchar RECV_SELF_TEMP = 0xC1;
    //Solicita Temperatura de Referência
    const static uchar RECV_REL_TEMP = 0xC2;
    //Lê comandos do usuário
    const static uchar RECV_CMD = 0xC3;

    //Envia sinal de controle Int (4 bytes)  PID
    const static uchar SEND_CTR = 0xD1;
    //Envia sinal de Referência Float (4 bytes)
    const static uchar SEND_REF_TEMP = 0xD2;
    //Envia Estado do Sistema (Ligado = 1 / Desligado = 0)
    const static uchar SEND_POW_STATE = 0xD3;
    //Modo de Controle da Temperatura de referência (Dashboard = 0 / Curva/Terminal = 1) (1 byte)
    const static uchar SEND_DASH_STATE = 0xD4;
    //Envia Estado de Funcionamento (Funcionando = 1 / Parado = 0)
    const static uchar SEND_PLAY_STATE = 0xD5;
    //Envia Temperatura Ambiente (Float))
    const static uchar SEND_ROOM_TEMP = 0xD6;

};

struct modbus_header
{
    uchar endereco;
    uchar code;
    uchar subcode;
};
