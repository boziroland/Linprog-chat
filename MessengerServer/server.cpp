#include "server.h"
#include <functional>

Server::Server(QObject *parent) : QTcpServer(parent), server_socket(new QTcpSocket(this)), users(new Database()){

    connect(server_socket, &QTcpSocket::readyRead, this, &Server::Ready2Read);

    listen(QHostAddress::Any, 45732);
    std::cout << "Server created!" << std::endl;

    Database people;
}

Server::~Server(){
    delete server_socket;
    delete users;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    std::cout << "A client would like to join!" << std::endl;
    /*
    ClientHandler* handler = new ClientHandler(this);

    if(handler->setSocketD(socketDescriptor)){
        handler->deleteLater();
        return;
    }

    connect(handler, &ClientHandler::sendMsgToServer, this, &Server::ReceiveCommunication);
    //connect(handler, &ClientHandler::getDisconnected, this, std::bind(&Server::userDisconnected,this,handler));

    client_list.append(handler);
    */
    server_socket->setSocketDescriptor(socketDescriptor);

}

void Server::ReceiveCommunication(Msg msg) {
    std::cout << msg.id.toStdString() << std::endl;
    std::cout << msg.username.toStdString() << std::endl;
    std::cout << msg.message.toStdString() << std::endl;

    if(msg.id == QString("001")) { //login
/*
        std::string qstr = "select username from users where username = '";
        qstr.append(msg.username.toStdString());
        qstr.append("' and password = '");
        qstr.append(msg.message.toStdString());
        qstr.append("';");
*/

        QString qstr = "select username from users where username = :username and password = :password ;";

        QSqlQuery qry;

        qry.prepare(qstr);

        qry.bindValue(":username",msg.username);
        qry.bindValue(":password",msg.message);

        //if(users->QueryDB(QString(qstr.c_str()))){

        qry.exec();

        if(qry.next()) {
            //ha igaz, akkor visszaküldés pozitív megerősítést
            QString addUserStr = "insert into :room (username) values (':username');"; //general szobához hozzáadás
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);

            addUserQry.exec();

        }

        return;
    }
    if(msg.id == QString("002")) { //regisztráció
        QString qstr = "select username from users where username = :username ;";

        QSqlQuery qry;
        qry.prepare(qstr);
        qry.bindValue(":username",msg.username);
        qry.exec();

        if(qry.next()) {
            //ha igaz, akkor hibát küldünk, ilyen nevű felhasználó már van
            return;
        }

        QString addUserStr = "insert into users (username, password) values (:username, :password);";
        QSqlQuery addUserQry;
        addUserQry.prepare(addUserStr);
        addUserQry.bindValue(":username", msg.username);
        addUserQry.bindValue(":password", msg.message);

        addUserQry.exec();

        return;
    }
    if(msg.id == QString("005")) { //feliratkozás szobára
        QString qstr = "select username from :room where username = ':username' ;";

        QSqlQuery qry;
        qry.prepare(qstr);
        qry.bindValue(":username",msg.username);
        qry.bindValue(":room", msg.message);
        qry.exec();

        if(qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már feliratkozott a szobára

        QString addUserStr = "insert into :room (username) values (':username');";
        QSqlQuery addUserQry;
        addUserQry.prepare(addUserStr);
        addUserQry.bindValue(":username", msg.username);
        addUserQry.bindValue(":room", msg.message);

        addUserQry.exec();

        return;
    }
    if(msg.id == QString("006")) { //leiratkozás szobáról
        QString qstr = "select username from :room where username = ':username' ;";

        QSqlQuery qry;
        qry.prepare(qstr);
        qry.bindValue(":username",msg.username);
        qry.bindValue(":room", msg.message);
        qry.exec();

        if(!qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már nincs feliratkozva

        QString addUserStr = "delete from :room (username) where username= ':username';";
        QSqlQuery addUserQry;
        addUserQry.prepare(addUserStr);
        addUserQry.bindValue(":username", msg.username);
        addUserQry.bindValue(":room", msg.message);

        addUserQry.exec();

        return;
    }
    if(msg.id == QString("009")) { //kijelentkezés
        for(auto room: users->getRooms()) {
            QString addUserStr = "delete from :room (username) where username= ':username';";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            addUserQry.bindValue(":room", msg.message);

            addUserQry.exec();
        }
        return;
    }
}

void Server::Ready2Read(){
    std::cout << "Clients message:" << std::endl;

    QDataStream stream(server_socket);

    stream.setVersion(QDataStream::Qt_5_7);

    QString str;
    Msg msg;

    while(1){
        stream.startTransaction();

        stream >> msg.id;
        stream >> msg.username;
        stream >> msg.message;

        if(stream.commitTransaction()){
            ReceiveCommunication(msg);
        }else{
            break;
        }
    }
}

void Server::userDisconnected(){

}
