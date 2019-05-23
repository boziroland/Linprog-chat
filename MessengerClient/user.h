#ifndef USER_H
#define USER_H

#include <QObject>
#include "room.h"

class User : public QObject
{
    Q_OBJECT
public:
    explicit User(QString username, QObject *parent = nullptr);

    QString getUsername() const;
    ~User();

private:
    QString username;
    std::vector<Room*> rooms;

signals:

public slots:
};

#endif // USER_H
