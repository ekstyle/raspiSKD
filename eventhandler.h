#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QSocketNotifier>

#if (defined (LINUX) || defined (__linux__))
#include <linux/input.h>
#endif


class EventHandler : public QObject
{
    Q_OBJECT
public:
    explicit EventHandler(QString eventDevice, QObject *parent = 0);
    QString device;
private:
    QSocketNotifier *notifier;
    char scancodeToChar(int code);
    int fd;
    QString buf;
    bool shift;


signals:
    void inputEvent(QString input);
public slots:
    void readEvent();
};
#endif // EVENTHANDLER_H
