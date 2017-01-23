#ifndef RASPISETTINGS_H
#define RASPISETTINGS_H

#include <QVariant>
#include <QSettings>
#include <QMetaEnum>

class RaspiSettings :public QObject{
    Q_OBJECT
public:
    enum Key{
        LogFile,
        LogLevel,
        Echo,
        API_url,
        API_module,
        API_Terminal_id,
        API_Auth_id,
        API_SercetKey

    };
    Q_ENUM(Key)

    static QVariant get(Key key);

private:
    RaspiSettings();
    RaspiSettings(const RaspiSettings &);
    RaspiSettings & operator = (const RaspiSettings &);



    static RaspiSettings & instance();
    QSettings settings;
    QVariant defaultSettings (Key key);
};

#endif
