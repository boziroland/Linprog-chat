#include "database.h"
#include <iostream>

Database::Database()
{
    QDir path;
    QString pathString = path.currentPath() + "/myDb.db";
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathString);
    std::cout << pathString.toStdString() << std::endl;
    if(db.open()){
        //std::cout << "its open" << std::endl;
        QSqlQuery query;
        query.exec("create table users"
                   "(id integer primary key, "
                   "username varchar(20), "
                   "password varchar(20));");

        QSqlQuery q2;
        q2.exec("insert into users (id, username, password)"
                "values (5, 'albert', 'rado');");
    }else {
        std::cout << "Could not open database. " << std::endl;
    }


}

Database::~Database() {
    db.close();
}

bool Database::QueryDB(QString str){
    QSqlQuery query;
    query.exec(str);



    return true;
}
