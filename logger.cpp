#include "logger.h"
#include "raspisettings.h"
#include <QDateTime>
#include <QTextStream>
#include <QMetaEnum>
#include <QCoreApplication>

Logger* Logger::m_instance = 0;

Logger* Logger::instance(){
    QString dir=QCoreApplication::applicationDirPath()+"/";
    if(m_instance==0) m_instance= new Logger(RaspiSettings::get(RaspiSettings::LogFile).toString().isEmpty() ? dir + QDateTime::currentDateTime().toString("dd-MM-yyyy.log") : RaspiSettings::get(RaspiSettings::LogFile).toString() );
    return m_instance;
}
Logger::Logger(QString filename, bool echoMode, QObject *parent) : QObject(parent)
{
    echo=echoMode;
    file=new QFile(filename);
    file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

void Logger::log(const QString &msg, LogLevel lvl)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Logger::LogLevel>();
    if(RaspiSettings::get(RaspiSettings::Echo).toBool()) QTextStream(stdout) << msg << "\n";
    if( RaspiSettings::get(RaspiSettings::LogLevel).toString().compare(metaEnum.valueToKey(lvl),Qt::CaseInsensitive) ||  RaspiSettings::get(RaspiSettings::LogLevel).toString().compare("All",Qt::CaseInsensitive) || RaspiSettings::get(RaspiSettings::LogLevel).toString().compare("Debug",Qt::CaseInsensitive));
    {
        if (file->isOpen()){
            QTextStream out(file);
            out.setCodec("UTF-8");
            out << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss\t") << metaEnum.valueToKey(lvl) << "\t" << msg << "\n";
            out.flush();
        }
    }
}
