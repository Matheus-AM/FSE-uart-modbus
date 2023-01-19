/* #include <bcm2835.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXTIMINGS 100

//helpers
#define repeat(x) for(int it=0;it<x;it++)


class Pin 
{
private:
    uint8_t state;
    std::string type;
    std::string tag;
public:
    uint8_t isInput;
    uint8_t gpioPin;
    Pin(json jgpio, uint8_t isIn);
    void refreshState(uint8_t* stat);
    void showState();
    void changeState();
    uint8_t getGpioPin();
    uint8_t getState();

};

class Gpio
{
public:
    Pin* light1;
    Pin* light2;
    Pin* air;
    Pin* projector;
    Pin* alarmBuzz;

    Pin* sPresence;
    Pin* sSmoke;
    Pin* sWindow;
    Pin* sDoor;
    Pin* sCountIn;
    Pin* sCountOut;

    Pin* sTemp;
    Pin* sHumid;
    Gpio(json data);
    void refreshAll(uint8_t* states);
    void handle_smoke(uint8_t* states);
};



void *gpio_handler(void* args);

int readDHT(int pin, uint8_t* states);
*/