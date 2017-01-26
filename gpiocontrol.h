#ifndef GPIOCONTROL_H
#define GPIOCONTROL_H

#include <QObject>
#include <QNetworkAccessManager>
#include "gpioworkerthread.h"
#include "eventhandler.h"

class GPIOControl : public QObject
{
    Q_OBJECT
public:
    explicit GPIOControl(QObject *parent = 0);
private:
    void print (const QString &msg);
    void createWorkerThread();
    bool barcodeVerify(QString barcode);
    void readInput();
    void init_EventHandlers();
    bool authorize(const QString &barcode);
    GPIOWorkerThread* gpioThread;
    QString lastbarcode;
    QVector<EventHandler*> vecEV;

signals:

public slots:
    void workerClose();


};

#endif // GPIOCONTROL_H
