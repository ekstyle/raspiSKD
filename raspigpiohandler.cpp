#include "raspigpiohandler.h"

#if (defined (LINUX) || defined (__linux__))
#include <wiringPi.h>
#endif

RaspiGPIOHandler* RaspiGPIOHandler::m_instance = 0;

RaspiGPIOHandler* RaspiGPIOHandler::instance(){
    if(m_instance==0) m_instance= new RaspiGPIOHandler();
    return m_instance;
}

RaspiGPIOHandler::RaspiGPIOHandler()
{
    init();
}

bool RaspiGPIOHandler::init()
{
    #if (defined (LINUX) || defined (__linux__))
    if (wiringPiSetup () == -1){
        return true ;
    }
    else return false;
#endif
    return false;
}

int RaspiGPIOHandler::readFromPin(int pin)
{
    #if (defined (LINUX) || defined (__linux__))
    pinMode (pin, INPUT) ;
    return digitalRead(pin);
    #endif
    return 0;
}

void RaspiGPIOHandler::blink(int pin, unsigned int duration)
{  
    for (;;)
    {
     writeToPin(pin,1);
     sleep(duration) ;
     writeToPin(pin,0);
     sleep(duration) ;
    }
}

void RaspiGPIOHandler::writeToPin(int pin, int value)
{
    #if (defined (LINUX) || defined (__linux__))
    pinMode (pin, OUTPUT) ;
    digitalWrite(pin,value);
    #endif
}

void RaspiGPIOHandler::sleep(unsigned int duration)
{
    #if (defined (LINUX) || defined (__linux__))
    delay (duration) ;
    #endif

}

int RaspiGPIOHandler::tooglePin(int pin)
{
    if(readFromPin(pin) == 1) writeToPin(pin,0);
    else writeToPin(pin,1);

}
