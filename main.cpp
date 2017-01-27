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
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication a(argc,argv);

    QSystemSemaphore semaphore(a.applicationName()+a.organizationName(), 1);
    semaphore.acquire();
#ifndef Q_OS_WIN32
    QSharedMemory nix_shared (a.applicationName());
    if(nix_shared.attach()){
        nix_shared.detach();
    }
#endif
    QSharedMemory sharedMemory(a.applicationName());
    bool is_running;
    if (sharedMemory.attach()){
        is_running = true;
    }else{
        sharedMemory.create(1);
        is_running = false;
    }
    semaphore.release();
    if(is_running){
        printf("Already running %s %s\n",a.applicationName().toLatin1().data(),a.applicationVersion().toLatin1().data());
        return 1;
    }
   GPIOControl* control=new GPIOControl();
   return a.exec();
}
