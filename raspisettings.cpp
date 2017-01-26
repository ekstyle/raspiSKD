#include "raspisettings.h"
#include <QSettings>
#include <QMetaEnum>
#include <QRegExp>
#include <QStringList>

RaspiSettings &RaspiSettings::instance()
{
    static RaspiSettings singleton;
    return singleton;
}

QVariant RaspiSettings::get(Key key)
{
    RaspiSettings &self = instance();
    QMetaEnum metaEnum = QMetaEnum::fromType<RaspiSettings::Key>();
    QString keyString=metaEnum.valueToKey(key);
    return self.settings.value(keyString,self.defaultSettings(key));
}

void RaspiSettings::set(RaspiSettings::Key key, QVariant value)
{
     RaspiSettings &self = instance();
     QMetaEnum metaEnum = QMetaEnum::fromType<RaspiSettings::Key>();
     QString keyString=metaEnum.valueToKey(key);
     self.settings.setValue(keyString,value);
}

QVariant RaspiSettings::defaultSettings(Key key)
{
    switch (key) {
    case LogFile:
        return QVariant("");
        break;
    case LogLevel:
        return QVariant("Debug");
        break;
    case Echo:
        return QVariant(true);
        break;
    case API_module:
        return QVariant("acs_ticket");
        break;
    case API_Terminal_id:
        return QVariant(); //NEED
        break;
    case API_Auth_id:
        return QVariant();  //NEED
        break;
    case API_SercetKey:
        return QVariant("");  //NEED
        break;
    case API_url:
        return QVariant(""); //NEED
        break;

    default:
        return QVariant();
        break;
    }
}

RaspiSettings::RaspiSettings(){
    //settings=QSettings("kassy.ru","raspiSKD");
}

