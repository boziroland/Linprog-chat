#include "serverdatabase.h"
#include <iostream>

ServerDatabase::ServerDatabase()
{
    QDir path;
    QString pathString = path.currentPath() + "/myDb.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathString);
    std::cout << pathString.toStdString() << std::endl;
    if(db.open()) {
        //std::cout << "its open" << std::endl;
        QSqlQuery query;
        query.exec("create table users"
                   "(username varchar(20) primary key, "
                   "password varchar(20));");

        QSqlQuery q2;
        q2.exec("insert into users (username, password)"
                "values ('albert', 'rado');");

        rooms.push_back("General");
        rooms.push_back("Sports");
        rooms.push_back("Music");
        rooms.push_back("Movies");

        for(auto room: rooms) {
            QSqlQuery query;
            query.exec("create table " + room +
                       " (username varchar(20) primary key);");
        }

    } else {
        std::cout << "Could not open database. " << std::endl;
    }


}

ServerDatabase::~ServerDatabase() {
    db.close();
}

bool ServerDatabase::QueryDB(QString str){
    QSqlQuery query;
    query.exec(str);


    return true;
}

bool ServerDatabase::QueryDB(QSqlQuery qry){
    qry.exec();


    return true;
}

std::vector<QString> ServerDatabase::getRooms() const
{
    return rooms;
}

