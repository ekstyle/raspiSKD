QT       += core network
QT       -= gui
TARGET = raspiSKD
CONFIG   += console static c++11
CONFIG   -= app_bundle

TEMPLATE = app

linux: LIBS +=-lwiringPi

SOURCES += main.cpp \
    raspigpiohandler.cpp \
    gpiocontrol.cpp \
    gpioworkerthread.cpp \
    raspisettings.cpp \
    apianswer.cpp \
    logger.cpp

HEADERS += \
    raspigpiohandler.h \
    gpiocontrol.h \
    gpioworkerthread.h \
    raspisettings.h \
    apianswer.h \
    logger.h
