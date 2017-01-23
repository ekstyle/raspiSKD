#include <QCoreApplication>
#include "gpiocontrol.h"

int main (int argc, char *argv[])
{
   QCoreApplication a(argc,argv);
   GPIOControl* control=new GPIOControl();
   return a.exec();
}
