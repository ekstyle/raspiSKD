#include "gpiocontrol.h"
#include "gpioworkerthread.h"
#include "apianswer.h"
#include "logger.h"
#include "raspisettings.h"
#include <QNetworkReply>
#include <QTextStream>
#include <QCryptographicHash>
#include <QFile>
#include <QMetaEnum>

GPIOControl::GPIOControl(QObject *parent) : QObject(parent)
{
    createWorkerThread();
    print(RaspiSettings::get( RaspiSettings::LogLevel ).toString() );
    readInput();
}

void GPIOControl::print(const QString &msg)
{
    QTextStream(stdout)<<msg;
}

void GPIOControl::createWorkerThread()
{
    gpioThread=new GPIOWorkerThread();
    connect(gpioThread, &GPIOWorkerThread::closingPin, [=](int pin){  Logger::instance()->log(QObject::tr("Closing pin #%1").arg(pin),Logger::Debug); });
    connect(gpioThread, &GPIOWorkerThread::stateChanged, [=](GPIOWorkerThread::State state){
        QMetaEnum metaEnum = QMetaEnum::fromType<GPIOWorkerThread::State>();
        Logger::instance()->log(tr("State worker changed to ") + metaEnum.valueToKey(state),Logger::Warning);
    });
    connect(gpioThread, &GPIOWorkerThread::finished, gpioThread, &QObject::deleteLater);
    gpioThread->start();
}

bool GPIOControl::barcodeVerify(QString barcode)
{
    if(lastbarcode==barcode) return false;  //TODO fix
    QString xml=QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                        "<request module=\"acs_ticket\" format=\"json\">"
                        "<action id=\"\" direction=\"%1\" type=\"scanner\" />"
                        "<ticket code=\"%2\" />"
                        "<terminal id=\"%3\" />"
                        "<auth id=\"%4\" />"
                        "</request>").arg("entry").arg(barcode).arg(RaspiSettings::get(RaspiSettings::API_Terminal_id).toInt()).arg(RaspiSettings::get(RaspiSettings::API_Auth_id).toInt());

    Logger::instance()->log("xml="+xml,Logger::Debug);
    Logger::instance()->log("url="+RaspiSettings::get(RaspiSettings::API_url).toString() ,Logger::Debug);
    QByteArray post_data;
    post_data.append("xml="+xml.toUtf8());
    post_data.append("&");
    post_data.append("sign="+QCryptographicHash::hash(QString(xml+RaspiSettings::get(RaspiSettings::API_SercetKey).toString()).toUtf8(),QCryptographicHash::Md5).toHex());

    QNetworkRequest request;
    request.setUrl(QUrl(RaspiSettings::get(RaspiSettings::API_url).toString()));
    request.setRawHeader("User-Agent", "raspiSKD");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QNetworkAccessManager *manager=new QNetworkAccessManager(this);

    Logger::instance()->log(QObject::tr("Scan ")+barcode,Logger::Warning);
    connect(manager,&QNetworkAccessManager::finished,[=](QNetworkReply* reply){  //ANSWER
        APIAnswer answ(reply->readAll(),APIAnswer::Json);
        if(!answ.isCorrect) Logger::instance()->log(tr("API error answer ") + answ.errorString,Logger::Error);

        if(answ.code==0) gpioThread->setState(GPIOWorkerThread::GateOpen);
        lastbarcode=barcode;
        manager->deleteLater();
        readInput();   //TODO fix that trash
    });
    QNetworkReply* reply = manager->post(request,post_data);
    QObject::connect(reply,static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),[=](){ //ERROR
        Logger::instance()->log(tr("Network error: ")+reply->errorString(),Logger::Error);
    });
}

void GPIOControl::readInput()
{
    QTextStream qin(stdin);
    QString in;
    in=qin.readLine();
    if(!in.isEmpty() && lastbarcode!=in) barcodeVerify(in);
    else readInput();

}

void GPIOControl::workerClose()
{
    print("worker close");

}

