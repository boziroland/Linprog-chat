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
};

#endif // DATABASE_H
