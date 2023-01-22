#include <forno.h>

uchar interrupt = 0;
int main(int argc, const char * argv[]) {
    uchar matricula[4] = {0, 3, 0, 7};
    pid_configura_constantes(30.0, 0.2, 400.0);
    if (wiringPiSetup() == -1) exit (1);
    Forno forno(matricula);
    
    signal(SIGINT, sighandler);
    while (1)
    {
        int user_cmd = forno.refreshCmd();
        if (user_cmd != -1) forno.handleUserCmd(user_cmd);
        if(forno.isPlaying())
            forno.playIt();
        usleep(2000000);
        if(interrupt) break;
        // usleep(500000);
    }
    forno.finishIt();
    return 0;
}

void sighandler(int signum) {
    interrupt = 1;
}