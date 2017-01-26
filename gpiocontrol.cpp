#include "gpiocontrol.h"
#include "gpioworkerthread.h"
#include "apianswer.h"
#include "logger.h"
#include "raspisettings.h"
#include <QNetworkReply>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QFile>
#include <QMetaEnum>
#include <sstream>

#if (defined (LINUX) || defined (__linux__))
#define INPUT_EVENT_PATH  "/dev/input/"
std::string execute(const char* cmd)
{
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return result; //error(EXIT_FAILURE, errno, "Pipe error");
    char buffer[128];
    while(!feof(pipe))
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    pclose(pipe);
    return result;
}
#endif

GPIOControl::GPIOControl(QObject *parent) : QObject(parent)
{
    init_EventHandlers();
    createWorkerThread();
    if(vecEV.isEmpty()) readInput();
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
        Logger::instance()->log(tr("State worker changed to ") + metaEnum.valueToKey(state),Logger::Debug);
    });
    connect(gpioThread, &GPIOWorkerThread::finished, gpioThread, &QObject::deleteLater);
    gpioThread->start();
}

bool GPIOControl::barcodeVerify(QString barcode)
{

    //if(lastbarcode==barcode) return false;  //TODO fix

    if (authorize(barcode)) return true;
    QString xml=QString("<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                        "<request module=\"acs_ticket\" format=\"json\">"
                        "<action id=\"\" direction=\"%1\" type=\"scanner\" />"
                        "<ticket code=\"%2\" />"
                        "<terminal id=\"%3\" />"
                        "<auth id=\"%4\" />"
                        "</request>").arg("entry").arg(barcode).arg(RaspiSettings::get(RaspiSettings::API_Terminal_id).toInt()).arg(RaspiSettings::get(RaspiSettings::API_Auth_id).toInt());

    //Logger::instance()->log("xml="+xml,Logger::Debug);
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
        if(!answ.isCorrect) {
            Logger::instance()->log(tr("API error answer ") + answ.errorString,Logger::Error);
            return false;
        }

        switch (answ.code) {
        case 0: //Ticket not found
            gpioThread->setState(GPIOWorkerThread::GateOpen);
            break;
        default:
            break;
        }

        lastbarcode=barcode;
        manager->deleteLater();
        if(vecEV.isEmpty()) readInput();
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
    if(!in.isEmpty()) barcodeVerify(in);
    else readInput();

}

void GPIOControl::init_EventHandlers()
{
    #if (defined (LINUX) || defined (__linux__))
    const char* cmd = "/bin/grep -E 'Handlers|EV=' /proc/bus/input/devices | "
                      "/bin/grep -B1 'EV=1[02]001[3Ff]' | "
                      "/bin/grep -Eo 'event[0-9]+' ";
    std::stringstream output(execute(cmd));
    std::vector<std::string> results;
    std::string line;
    while(std::getline(output, line)) {
        std::string::size_type i = line.find("event");
        if (i != std::string::npos) i += 5; // "event".size() == 5
        if (i < line.size()) {
            int index = atoi(&line.c_str()[i]);
            if (index != -1) {
                std::stringstream input_dev_path;
                input_dev_path << INPUT_EVENT_PATH;
                input_dev_path << "event";
                input_dev_path << index;
                results.push_back(input_dev_path.str());
              }
            }
          }
          if (results.size() == 0) {
              Logger::instance()->log(QObject::tr("Couldn't determine keyboard device."),Logger::Error);
              return;
          }
          for ( size_t i = 0 ; i < results.size() ; i++ ){
              EventHandler* handler=new EventHandler(results[i].data());
              Logger::instance()->log(QObject::tr("New event device %1").arg(handler->device),Logger::Debug);
              vecEV.append(handler);
              QObject::connect(handler,&EventHandler::inputEvent,[=](QString input){
                  //EventHandler* hnd=qobject_cast<EventHandler*>(QObject::sender());
                  Logger::instance()->log(QObject::tr("Device %1 scan %2").arg(handler->device).arg(input),Logger::Debug);
                  barcodeVerify(input);
              });
          }
    #endif

}

bool GPIOControl::authorize(const QString &barcode)
{
    QJsonDocument d = QJsonDocument::fromJson(barcode.toLatin1());
    if(!d.isObject()) return false;
    //Logger::instance()->log(QObject::tr("Terminal haz"),Logger::Warning);
    QJsonObject main = d.object();
    QJsonObject auth = main["auth"].toObject();
    QJsonObject terminal = main["terminal"].toObject();
    if(auth.isEmpty() || terminal.isEmpty()) {
        Logger::instance()->log(QObject::tr("Falied parse JSON from authorize QR raw:%1").arg(barcode),Logger::Error );
    }

    get_set(RaspiSettings::API_url,auth.value("url").toString());
    get_set(RaspiSettings::API_Auth_id,auth.value("id").toString());
    get_set(RaspiSettings::API_SercetKey,auth.value("secret-key").toString());
    get_set(RaspiSettings::API_Terminal_id,terminal.value("id").toString());


//    QJsonValue message = result.value(QString("message"));
//    QJsonValue codeJson = result.value(QString("code"));
//    code=codeJson.toInt();
    Logger::instance()->log(QObject::tr("Terminal authorized for :%1 ").arg(get_set(RaspiSettings::API_url).toString()), Logger::Warning);
    return true;

}

void GPIOControl::workerClose()
{
    print("worker close");

}

