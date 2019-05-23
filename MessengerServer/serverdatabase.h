#ifndef SERVERDATABASE_H
#define SERVERDATABASE_H

#include <QObject>
#include <QtSql>

class ServerDatabase
{
    QSqlDatabase db;

public:
    ServerDatabase();
    ~ServerDatabase();
    bool QueryDB(QString);
    bool QueryDB(QSqlQuery);

    std::vector<QString> getRooms() const;

private:
    std::vector<QString> rooms;
};

#endif // SERVERDATABASE_H
