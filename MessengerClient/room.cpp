#include "room.h"

Room::Room(QString name, QObject *parent) : QObject(parent)
{
    roomName = name;
    if(name=="General") isSubscribedTo = true;
    else isSubscribedTo = false;
}

bool Room::getIfSubscribedTo() const
{
    return isSubscribedTo;
}

void Room::subscribeTo() {
    isSubscribedTo = true;
}
void Room::unsubscribeFrom() {
    isSubscribedTo = false;
}

QString Room::getRoomName() const
{
    return roomName;
}

void Room::addToLog(QString str) {
    log.push_back(str);
}

std::vector<QString> Room::getLog() const
{
    return log;
}
