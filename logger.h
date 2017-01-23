#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>

class Logger : public QObject
{
    Q_OBJECT
public:
    enum LogLevel{
        Debug,
        Warning,
        Error,
        Critical
    };
    Q_ENUM(LogLevel)
    static Logger* instance(); //singleton pattern
    explicit Logger( QString filename, bool echoMode = true, QObject *parent = 0);
private:
    static Logger* m_instance;
    QFile *file;
    bool echo;

signals:

public slots:
    void log(const QString &msg,LogLevel lvl);
};

#endif // LOGGER_H
