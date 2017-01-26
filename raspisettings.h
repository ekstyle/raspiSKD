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
    static void set(Key key,QVariant value);

private:
    RaspiSettings();
    RaspiSettings(const RaspiSettings &);
    RaspiSettings & operator = (const RaspiSettings &);



    static RaspiSettings & instance();
    QSettings settings;
    QVariant defaultSettings (Key key);
};

inline QVariant get_set (RaspiSettings::Key key){
    return RaspiSettings::get(key);
}
inline void get_set (RaspiSettings::Key key, QVariant value){
    return RaspiSettings::set(key, value);
}
#endif
