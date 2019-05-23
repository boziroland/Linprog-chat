#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QtSql>

class Database
{

    QSqlDatabase db;

public:
    Database();
    ~Database();
    bool QueryDB(QString);
    bool QueryDB(QSqlQuery);

    std::vector<QString> getRooms() const;

private:
    std::vector<QString> rooms;
};

#endif // DATABASE_H
