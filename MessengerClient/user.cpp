#include "user.h"

User::User(QString username, QObject *parent) : QObject(parent) {
    this->username = username;
}

QString User::getUsername() const
{
    return username;
}

User::~User() {
    for(auto room: rooms) {
        delete(room);
    }
}
