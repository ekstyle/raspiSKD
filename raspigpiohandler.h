#ifndef RASPIGPIOHANDLER_H
#define RASPIGPIOHANDLER_H

#include <QObject>

class RaspiGPIOHandler{

public:
    static RaspiGPIOHandler* instance(); //singleton pattern

    RaspiGPIOHandler();
    bool init();
    int readFromPin(int pin);
    void blink(int pin,unsigned int duration);
    void writeToPin(int pin,int value);
    void sleep(unsigned int duration);
    int tooglePin(int pin);

private:
    static RaspiGPIOHandler* m_instance;
};

#endif // RASPIGPIOHANDLER_H
