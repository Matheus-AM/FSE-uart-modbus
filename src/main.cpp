#include <forno.h>

int main(int argc, const char * argv[]) {
    uchar matricula[4] = {0, 3, 0, 7};
    pid_configura_constantes(30.0, 0.2, 400.0);
    if (wiringPiSetup() == -1) exit (1);
    Forno forno(matricula);

    while (1)
    {
        int user_cmd = forno.refreshCmd();
        if (user_cmd != -1) forno.handleUserCmd(user_cmd);   
        forno.playIt();
        usleep(2000000);
        // usleep(500000);
    }


    forno.uart->close_it();
    return 0;
}