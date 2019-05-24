#include "user.h"

User::User(QString username, QObject *parent) : QObject(parent) {
    this->username = username;
    currentRoom = "General";
}

QString User::getUsername() const
{
    return username;
}

User::~User() {
    for(auto room: rooms) {
        delete room;
    }
}

QString User::getCurrentRoom() const
{
    return currentRoom;
}

void User::setCurrentRoom(const QString &value)
{
    currentRoom = value;
}

std::vector<Room *> User::getRooms() const
{
    return rooms;
}

void User::addRoom(Room *r) {
    rooms.push_back(r);
}
