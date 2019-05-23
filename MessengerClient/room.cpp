#include "room.h"

Room::Room(QString name, QObject *parent) : QObject(parent)
{
    roomName = name;
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
