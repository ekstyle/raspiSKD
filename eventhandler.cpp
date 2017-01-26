#include "eventhandler.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <QString>

#include "logger.h"

EventHandler::EventHandler(QString eventDevice, QObject *parent) : QObject(parent),device(eventDevice),shift(false)
{
    fd = open(eventDevice.toLatin1().data(), O_RDONLY );
        if (fd == -1) {
            Logger::instance()->log(QObject::tr("Cannot open %1").arg(eventDevice),Logger::Error);
        } else{
            notifier=new QSocketNotifier(fd,QSocketNotifier::Read,this);
            EventHandler::connect(notifier, &QSocketNotifier::activated, this, &EventHandler::readEvent);
        }

}

char EventHandler::scancodeToChar(int code)
{
    char ret;

#if (defined (LINUX) || defined (__linux__))

    switch (code) {
    case KEY_0:
        ret='0';
        break;
    case KEY_1:
        ret='1';
        break;
    case KEY_2:
        ret='2';
        break;
    case KEY_3:
        ret='3';
        break;
    case KEY_4:
        ret='4';
        break;
    case KEY_5:
        ret='5';
        break;
    case KEY_6:
        ret='6';
        break;
    case KEY_7:
        ret='7';
        break;
    case KEY_8:
        ret='8';
        break;
    case KEY_9:
        ret='9';
        break;
    case KEY_KP0:
        ret='0';
        break;
    case KEY_KP1:
        ret='1';
        break;
    case KEY_KP2:
        ret='2';
        break;
    case KEY_KP3:
        ret='3';
        break;
    case KEY_KP4:
        ret='4';
        break;
    case KEY_KP5:
        ret='5';
        break;
    case KEY_KP6:
        ret='6';
        break;
    case KEY_KP7:
        ret='7';
        break;
    case KEY_KP8:
        ret='8';
        break;
    case KEY_KP9:
        ret='9';
        break;
    case KEY_A:
        ret = 'A';
        break;
    case KEY_B:
        ret = 'B';
        break;
    case KEY_C:
        ret = 'C';
        break;
    case KEY_D:
        ret = 'D';
        break;
    case KEY_E:
        ret = 'E';
        break;
    case KEY_F:
        ret = 'F';
        break;
    case KEY_G:
        ret = 'G';
        break;
    case KEY_H:
        ret = 'H';
        break;
    case KEY_I:
        ret = 'I';
        break;
    case KEY_J:
        ret = 'J';
        break;
    case KEY_K:
        ret = 'K';
        break;
    case KEY_L:
        ret = 'L';
        break;
    case KEY_M:
        ret = 'M';
        break;
    case KEY_N:
        ret = 'N';
        break;
    case KEY_O:
        ret = 'O';
        break;
    case KEY_P:
        ret = 'P';
        break;
    case KEY_Q:
        ret = 'Q';
        break;
        case KEY_R:
        ret = 'R';
        break;
        case KEY_S:
        ret = 'S';
        break;
        case KEY_T:
        ret = 'T';
        break;
        case KEY_U:
        ret = 'U';
        break;
        case KEY_V:
        ret = 'V';
        break;
        case KEY_W:
        ret = 'W';
        break;
        case KEY_X:
        ret = 'X';
        break;
    case KEY_Y:
        ret = 'Y';
        break;
    case KEY_Z:
        ret = 'Z';
        break;
    case KEY_SPACE:
        ret = ' ';
        break;
    case KEY_DOWN:
        ret = '\n';
        break;
    case KEY_MINUS:
        ret = '-';
        break;
    case KEY_LEFTBRACE:
        ret = '{';
        break;
    case KEY_RIGHTBRACE:
        ret = '}';
        break;
    case KEY_APOSTROPHE:
        ret = '\"';
        break;
    case KEY_SEMICOLON:
        ret = ':';
        break;
    case KEY_BACKSLASH:
        ret = '\\';
        break;
    case KEY_SLASH:
        ret = '/';
        break;
    case KEY_DOT:
        ret = '.';
        break;
    case KEY_COMMA:
        ret = ',';
        break;
    case KEY_EQUAL:
        ret = '=';
        break;
    case KEY_TAB:
        ret = '\t';
        break;
    case KEY_KPPLUS:
        ret = '+';
        break;
    default:
        Logger::instance()->log(QObject::tr("Unknown symbol %1 from %2").arg(code).arg(device) ,Logger::Error);
        ret='?';
        break;
    }
    #endif
    return ret;

}

void EventHandler::readEvent()
{
    #if (defined (LINUX) || defined (__linux__))
    struct input_event ev;
    ssize_t n;
    n = read(fd, &ev, sizeof ev);
    if( n!=(size_t)-1 && n == sizeof(ev) && ev.type == EV_KEY && ev.value == 1 ){
        //press
        switch (ev.code) {
        case KEY_ENTER:
            printf("input");
            emit inputEvent(buf);
            buf.clear();
            break;

        case KEY_LEFTSHIFT:
            shift = true;
            break;

        default:
            buf.append( shift ? scancodeToChar(ev.code) : tolower(scancodeToChar(ev.code)));
            break;
        }
        return;
    }
    if( n!=(size_t)-1 && n == sizeof(ev) && ev.type == EV_KEY && ev.value == 0){
        //unpress
        if(ev.code == KEY_LEFTSHIFT) shift = false;
    }
    #endif
}
