#include <QCoreApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include "gpiocontrol.h"

#define ORGANIZATION_NAME "kassy.ru"
#define ORGANIZATION_DOMAIN "kassy.ru"
#define APPLICATION_NAME "raspiSKD"

int main (int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication a(argc,argv);

    QSystemSemaphore semaphore("raspiSKD1", 1);
    semaphore.acquire();
#ifndef Q_OS_WIN32
    QSharedMemory nix_shared ("raspiSKD2");
    if(nix_shared.attach()){
        nix_shared.detach();
    }
#endif
    QSharedMemory sharedMemory("raspiSKD2");
    bool is_running;
    if (sharedMemory.attach()){
        is_running = true;
    }else{
        sharedMemory.create(1);
        is_running = false;
    }
    semaphore.release();
    if(is_running){
        printf("Already running\n");
        return 1;
    }
   GPIOControl* control=new GPIOControl();
   return a.exec();
}
