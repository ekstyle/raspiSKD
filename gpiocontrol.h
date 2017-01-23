#ifndef GPIOCONTROL_H
#define GPIOCONTROL_H

#include <QObject>
#include <QNetworkAccessManager>
#include "gpioworkerthread.h"

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
    GPIOWorkerThread* gpioThread;
    QString lastbarcode;

signals:

public slots:
    void workerClose();


};

#endif // GPIOCONTROL_H
