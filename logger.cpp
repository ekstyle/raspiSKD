#include "logger.h"
#include "raspisettings.h"
#include <QDateTime>
#include <QTextStream>
#include <QMetaEnum>
#include <QCoreApplication>

Logger* Logger::m_instance = 0;

Logger* Logger::instance(){
    QString dir=QCoreApplication::applicationDirPath()+"/";
    if(m_instance==0) m_instance= new Logger(get_set(RaspiSettings::LogFile).toString().isEmpty() ? dir + QDateTime::currentDateTime().toString("dd-MM-yyyy.log") : RaspiSettings::get(RaspiSettings::LogFile).toString() );
    return m_instance;
}
Logger::Logger(QString filename, bool echoMode, QObject *parent) : QObject(parent)
{
    echo=echoMode;
    file=new QFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

void Logger::writeToLog(const QString &msg, LogLevel lvl)
{
    if (file->isOpen()){
        QMetaEnum metaEnum = QMetaEnum::fromType<Logger::LogLevel>();
        QTextStream out(file);
        out.setCodec("UTF-8");
        out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss\t") << metaEnum.valueToKey(lvl) << "\t" << msg << "\n";
        out.flush();
    }
}

void Logger::log(const QString &msg, LogLevel lvl)
{
    //QString loglvl=RaspiSettings::get(RaspiSettings::LogLevel).toString();
    QString loglvl=get_set(RaspiSettings::LogLevel).toString();
    if(get_set(RaspiSettings::Echo).toBool()) QTextStream(stdout) << msg << "\n";
    switch (lvl) {
    case Error:
        writeToLog(msg, lvl);
        break;
    case Warning:
        if(loglvl == "Warning" || loglvl == "All" || loglvl == "Debug") writeToLog(msg, lvl);
        break;
    case Debug:
        if(loglvl == "Debug") writeToLog(msg, lvl);
        break;
    default:
        break;
    };
}
