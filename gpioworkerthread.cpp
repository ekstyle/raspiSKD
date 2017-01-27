#include "gpioworkerthread.h"
#include <QTextStream>
#include "raspigpiohandler.h"


#define threadSleepTime 30
#define HIGHT 1
#define LOW 0




void GPIOWorkerThread::setState(GPIOWorkerThread::State state)
{
    m_state=state;
    emit stateChanged(state);
}

void GPIOWorkerThread::run()
{
    m_state=GateClosed;
    btn_is_press=false;
    while(true){

        switch (m_state) {

        case NoState:

            msleep(1000);
           // RaspiGPIOHandler::instance()->tooglePin(0);
            break;

        case GateClosed:

            if(onClosing(1)){
                RaspiGPIOHandler::instance()->writeToPin(0,1);
                RaspiGPIOHandler::instance()->writeToPin(2,0);
                setState(GateOpen);
                break;
            }
            RaspiGPIOHandler::instance()->writeToPin(0,1);
            RaspiGPIOHandler::instance()->writeToPin(2,0);
            msleep(threadSleepTime);
            RaspiGPIOHandler::instance()->writeToPin(0,0);
            RaspiGPIOHandler::instance()->writeToPin(2,1);
            msleep(threadSleepTime);
            break;

        case GateOpen:

            if(onClosing(1)){
                RaspiGPIOHandler::instance()->writeToPin(0,0);
                RaspiGPIOHandler::instance()->writeToPin(2,1);
                setState(ClosedState);
                break;
            }
              msleep(threadSleepTime);
              break;

        case ClosedState:

            if(onClosing(1)){
              RaspiGPIOHandler::instance()->writeToPin(0,0);
              RaspiGPIOHandler::instance()->writeToPin(2,1);
              setState(GateClosed);
              break;
            }
            msleep(threadSleepTime);
            break;
    }
        if(btn_is_press==true && RaspiGPIOHandler::instance()->readFromPin(1)==0){
            btn_is_press=false;
            //QTextStream(stdout)<<"unpress";
        }
   }

}

void GPIOWorkerThread::msleep(unsigned long msecs)
{
    QThread::msleep(msecs);
}

bool GPIOWorkerThread::onClosing(int pin)
{
    if(RaspiGPIOHandler::instance()->readFromPin(1)==1 && btn_is_press==false){
        btn_is_press=true;
        emit closingPin(pin);
        return true;
    }
}
