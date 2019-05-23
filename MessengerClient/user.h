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

    QString getCurrentRoom() const;
    void setCurrentRoom(const QString &value);

    std::vector<Room *> getRooms() const;

private:
    QString username;
    std::vector<Room*> rooms;
    QString currentRoom;

signals:

public slots:
};

#endif // USER_H
