#include "apianswer.h"
#include "logger.h"
#include "raspisettings.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QTextStream>

APIAnswer::APIAnswer(const QByteArray &data,const Type type)
{
    isCorrect=false;
    //raw print answer for debug
    //if(RaspiSettings::get(RaspiSettings::LogLevel)=="Debug")
    Logger::instance()->log(QObject::tr("API answer raw :") + QString(data).simplified(), Logger::Debug);
    switch (type) {

    case Json:    
        isCorrect=fromJson(data);
        break;

    default:
        break;
    }
}

bool APIAnswer::fromJson(const QByteArray &data)
{
    QJsonDocument d = QJsonDocument::fromJson(data);
    if(!d.isObject()) return false;
    QJsonObject main = d.object();
    QJsonValue module = main.value(QString("module"));
    if(module.toString()!="acs_ticket"){
        errorString=QObject::tr("API answer unknown format");
        return false;
    };
    QJsonObject result = main["result"].toObject();
    QJsonValue message = result.value(QString("message"));
    QJsonValue codeJson = result.value(QString("code"));
    code=codeJson.toInt();
    Logger::instance()->log(QObject::tr("Result: ") + message.toString() + QObject::tr(" Code: %1").arg(code), code < 0 ? Logger::Error : Logger::Warning);
    return true;

}
