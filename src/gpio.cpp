#include <gpio.h>

#define DEV

Gpio::Gpio(json data){
    light1 = new Pin(data["outputs"][0], 0);
    light2 = new Pin(data["outputs"][1], 0);
    air = new Pin(data["outputs"][2], 0);
    projector = new Pin(data["outputs"][3], 0);
    alarmBuzz = new Pin(data["outputs"][4], 0);
    sPresence = new Pin(data["inputs"][0], 1);
    sSmoke = new Pin(data["inputs"][1], 1);
    sWindow = new Pin(data["inputs"][2], 1);
    sDoor = new Pin(data["inputs"][3], 1);
    sCountIn = new Pin(data["inputs"][4], 1);
    sCountOut = new Pin(data["inputs"][5], 1);
    sTemp = new Pin(data["sensor_temperatura"][0], 1);// 1-Wire
    sHumid = new Pin(data["sensor_temperatura"][0], 1);// 1-Wire
}

void Gpio::refreshAll(uint8_t* states){
    handle_smoke(states);
    light1->refreshState(&states[1]);
    light2->refreshState(&states[2]);
    air->refreshState(&states[3]);
    projector->refreshState(&states[4]);
    alarmBuzz->refreshState(&states[5]);
    sPresence->refreshState(&states[6]);
    sSmoke->refreshState(&states[7]);
    sWindow->refreshState(&states[8]);
    sDoor->refreshState(&states[9]);
    sCountIn->refreshState(&states[10]);
    sCountOut->refreshState(&states[11]);
    sTemp->refreshState(&states[12]);
    sHumid->refreshState(&states[13]);
}

void Gpio::handle_smoke(uint8_t * states){
    if(states[7]){
        states[5] = states[7];
    }
}

Pin::Pin(json jgpio, uint8_t isIn){
    // isIn value 1 to input, value 0 to output
    gpioPin = jgpio["gpio"];
    type = jgpio["type"];
    tag = jgpio["tag"];
    isInput = isIn;
#ifndef DEV
    bcm2835_gpio_fsel(gpioPin, (isInput? BCM2835_GPIO_FSEL_INPT : BCM2835_GPIO_FSEL_OUTP));
    if(isInput){
        state = bcm2835_gpio_lev(gpioPin);
    }else{
        state = 0x0;
    }
#else
    state = 0x1;
    showState();     
#endif
}

void Pin::changeState(){
    if(isInput) return;
    state = !state;
#ifndef DEV
    bcm2835_gpio_write(gpioPin, (uint8_t)state); 
#endif
}

void Pin::refreshState(uint8_t* stat){
    if(!isInput){
        if(*stat != state){
            changeState();
        }
    }else{
#ifndef DEV
        state = bcm2835_gpio_lev(gpioPin);
#endif
    } 

    *stat = state;
}

void Pin::showState(){
    printf("Sensor %s: %d\n", tag.data(), state);
}

uint8_t Pin::getGpioPin(){
    return gpioPin;
}

uint8_t Pin::getState(){
    return state;
}



int readDHT(int pin, uint8_t* states) {

    int bits[250], data[100];
    int bitidx = 0;
    int counter = 0;
    int laststate = HIGH;
    int j=0;

    // Set GPIO pin to output
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_write(pin, HIGH);
    usleep(500000);  // 500 ms
    bcm2835_gpio_write(pin, LOW);
    usleep(20000);

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    // wait for pin to drop?
    while (bcm2835_gpio_lev(pin) == 1) {
        usleep(1);
    }

    // read data!
    for (int i=0; i< MAXTIMINGS; i++) {
        counter = 0;
        while ( bcm2835_gpio_lev(pin) == laststate) {
        counter++;
        //nanosleep(1);		// overclocking might change this?
            if (counter == 1000)
        break;
        }
        laststate = bcm2835_gpio_lev(pin);
        if (counter == 1000) break;
        bits[bitidx++] = counter;

        if ((i>3) && (i%2 == 0)) {
        // shove each bit into the storage bytes
        data[j/8] <<= 1;
        if (counter > 200)
            data[j/8] |= 1;
        j++;
        }
    }


#ifdef DEBUG
    for (int i=3; i<bitidx; i+=2) {
        printf("bit %d: %d\n", i-3, bits[i]);
        printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > 200);
    }
#endif

    if ((j >= 39) &&
        (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        // yay!
        float t, h;
        t = data[0] * 256 + data[1];
        t /= 10;

        h = (data[2] & 0x7F)* 256 + data[3];
            h /= 10.0;
            if (data[2] & 0x80)  h *= -1;
        if(t > 1){
            states[12] = (uint8_t)t;
        }
        if(h > 1){
            states[13] = (uint8_t)h;
        }
        return 1;
    }

  return 0;
}


void *gpio_handler(void* args){
    std::ifstream f("configuracao_sala_02.json");
    json data = json::parse(f);
    
    Gpio main_gpio(data);
    
    uint8_t* response = (uint8_t*)args;
    float* dht;
    while (1)
    {

        delay(200);
#ifndef DEV
	    readDHT(main_gpio.sTemp->getGpioPin(), response);
        printf("temp: %d, hum %d", response[12], response[13]);
#endif 
        main_gpio.refreshAll(response);
    }
}
