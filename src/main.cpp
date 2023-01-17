#include "uartController.h"

int main(int argc, const char * argv[]) {
    if(argc != 3){
        printf("\n$ ./exec <type> <msg>\n\ntype: 1(int), 2(float), 3(string)\nmsg: 00(recv), <text>");
    }
    uchar matricula[4] = {0x03, 0x00, 0x07, 0x00};
    UartController uart(matricula);
    int cmd = handle_interface(argv[1], argv[2]);
    // if(cmd > 0xa3){
    uart.send_tx(0xc3, argv[2]);
    // }else{
    // uart.recv_rx(cmd);
    // }
    uart.close_it();
    return 0;
}