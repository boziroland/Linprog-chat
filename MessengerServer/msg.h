#ifndef MSG_H
#define MSG_H

#include <QString>
#include <QtDebug>

struct Msg{
    QString id;
    QString username;
    QString message;
    QString room;
    QString email;
};

#endif // MSG_H
