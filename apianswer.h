#ifndef APIANSWER_H
#define APIANSWER_H
#include <QByteArray>
#include <QString>

class APIAnswer
{

public:
    enum Type{
        Json,
        Xml,
        Raw
    };
    APIAnswer(const QByteArray &data,const Type type=Json);

    bool isCorrect;
    QString errorString;

    QString msg;
    int code;
private:

    bool fromJson(const QByteArray &data);

};

#endif // APIANSWER_H
