#include "chatserver.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>
#include <QtDebug>

ChatServer::ChatServer(QObject *parent)
    : QTcpServer(parent), users(new ServerDatabase())
{}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);

    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }

    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&ChatServer::DisconnectMsg, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&ChatServer::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);

    m_clients.append(worker);
    emit logMessage(QStringLiteral("New client Connected"));
}
void ChatServer::sendJson(ServerWorker *destination, const Msg &message)
{
    destination->sendJson(message);
}

void ChatServer::broadcast(const Msg &message)
{
    QString qstr2 = "select username from " + message.room + ";";
    QSqlQuery qry;
    qry.exec(qstr2);
    while(qry.next()){

        for (ServerWorker *worker : m_clients) {
            qDebug() << "current worker: " << worker->userName();
            if (worker->userName() == qry.value(0).toString()){
                    qDebug() << "msg is being sent to: " << worker->userName();
                    sendJson(worker, message);}
        }
        qDebug() << " end of list ";
        //if(qry.next()) break;
    }

}

void ChatServer::unicast(const Msg &message, ServerWorker *include)
{
    for (ServerWorker *worker : m_clients) {
        if (worker == include)
            sendJson(worker, message);
    }
}

void ChatServer::jsonReceived(ServerWorker *sender, const Msg doc)
{

    jsonFromLoggedIn(sender, doc);
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);

    const QString userName = sender->userName();

    emit logMessage(userName + " disconnected");
    sender->deleteLater();
}

void ChatServer::DisconnectMsg(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage("Person disconnected: " + sender->userName());
}

void ChatServer::stopServer()
{
    for (ServerWorker *worker : m_clients) {
        worker->disconnectFromClient();
    }
    close();
}

Msg ChatServer::createMsg(QString* str, Msg* msg){
    msg->id = str[0];
    msg->username = str[1];
    msg->message = str[2];
    msg->room = str[3];
    msg->email = str[4];

    return *msg;
}

void ChatServer::jsonFromLoggedIn(ServerWorker *sender, Msg msg)
{

    if(msg.id == QString("001")) { //login

            QString qstr = "select count(username) from users where username = :username and password = :password ;";

            QSqlQuery qry;

            qry.prepare(qstr);

            qry.bindValue(":username", msg.username);
            qry.bindValue(":password", msg.message);

            qry.exec();
            qry.first();

            emit logMessage(msg.username + " is trying to log in..");

            //qDebug() << "query has this many results: " << qry.value(0).toInt();
            if(qry.value(0).toInt() == 1){
                Msg backGood;
                sender->setUserName(msg.username);
                QString str[5] = {"101", msg.username, "","",""}; //sikeres login
                unicast(createMsg(str,&backGood), sender);

                QString addUserStr = "insert into General (username) values (:username);"; //general szobához hozzáadás
                QSqlQuery addUserQry;
                addUserQry.prepare(addUserStr);
                addUserQry.bindValue(":username",msg.username);
                addUserQry.exec();

                emit logMessage(msg.username + " logged in!");
            }else{
                Msg backNotGood;
                QString str[5] = {"102", msg.username, "","",""}; //sikertelen login
                unicast(createMsg(str,&backNotGood), sender);
            }

            return;
        }
        if(msg.id == QString("002")) { //regisztráció
            QString qstr = "select count(username) from users where username = :username ;";

            QSqlQuery qry;
            qry.prepare(qstr);
            qry.bindValue(":username", msg.username);
            qry.exec();
            qry.first();
            qDebug() << "this many people have the to be registered name: " << qry.value(0).toInt();
            if(qry.value(0).toInt() > 0) {
                Msg UserExistsMsg;
                QString str[5] = {"104", msg.username, "Error", "","" }; //sikertelen regisztráció
                unicast(createMsg(str, &UserExistsMsg), sender);
                //ha igaz, akkor hibát küldünk, ilyen nevű felhasználó már van
                return;
            }

            QString addUserStr = "insert into users (username, password, email, pm) values (:username, :password, :email, 0);";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            addUserQry.bindValue(":password", msg.message);
            addUserQry.bindValue(":email", msg.email);
            //qDebug() << "the email: "<< msg.email;

            addUserQry.exec();

            Msg SuccessfulRegistrationMsg;
            QString str[5] = {"103", msg.username, "Success!", "", msg.email}; //sikeres regisztráció

            unicast(createMsg(str, &SuccessfulRegistrationMsg),sender);

            emit logMessage(msg.username + " succesfully registered!");

            return;
        }
        if(msg.id == QString("003")){
            if(msg.message.front() == "/"){
                QString qstr = "update users set pm = 1 where username = :username ;";

                QSqlQuery qry;
                qry.prepare(qstr);
                qry.bindValue(":username", msg.username);
                qry.exec();
            }else if(msg.message.front() == "@"){
                msg.id = "105";
                QString copy = msg.message;
                QString copy2 = copy.split(" ").first();
                copy2.remove(0,1);
                qDebug() << "copy2 is: " << copy2;
                QString qstr = "select count(username) from " + msg.room + " where username = :username ;";
                QSqlQuery qry;
                qry.prepare(qstr);
                qry.bindValue(":username",copy2);
                qry.exec();
                qry.first();

                if(qry.value(0).toInt() == 1){

                    QString qstr = "select count(username) from users where username = :user and pm = 1;";
                    QSqlQuery qry2;
                    qry2.prepare(qstr);
                    qry2.bindValue(":user", copy2);
                    qry2.exec();
                    qry2.first();

                    if(qry2.value(0).toInt() == 1){

                        for(auto w : m_clients){
                            if (w->userName() == copy2){
                                unicast(msg, w);
                                break;
                            }
                        }
                        unicast(msg, sender);
                    }
                }
            }else{
                msg.id = "105";
                broadcast(msg);
            }

            emit logMessage(msg.username + " sent a message.");
        }

        if(msg.id == QString("005")) { //feliratkozás szobára
            QString qstr = "select username from " + msg.room + " where username = :username ;";

            QSqlQuery qry;
            qry.prepare(qstr);
            qry.bindValue(":username",msg.username);
            qry.exec();

            if(qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már feliratkozott a szobára

            QString addUserStr = "insert into " + msg.room +" (username) values (:username);";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            //addUserQry.bindValue(":room", msg.message);

            addUserQry.exec();

            return;
        }

        if(msg.id == QString("006")) { //leiratkozás szobáról
            QString qstr = "select username from " + msg.room + " where username = :username ;";

            QSqlQuery qry;
            qry.prepare(qstr);
            qry.bindValue(":username",msg.username);
            //qry.bindValue(":room", msg.message);
            qry.exec();

            if(!qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már nincs feliratkozva

            QString addUserStr = "delete from "+ msg.room + " (username) where username = :username;";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            //addUserQry.bindValue(":room", msg.message);

            addUserQry.exec();

            return;
        }
        if(msg.id == QString("009")) { //kijelentkezés
            for(auto room: users->getRooms()) {
                QString addUserStr = "delete from " + msg.room + " (username) where username= :username;";
                QSqlQuery addUserQry;
                addUserQry.prepare(addUserStr);
                addUserQry.bindValue(":username", msg.username);

                addUserQry.exec();
            }
            return;
        }
        if(msg.id == QString("200")){
            std::vector<QString> vec =  users->getRooms();
            QString str = "";
            for(auto elem : vec){
                str.append(elem);
                str.append(",");
            }

            str.truncate(str.lastIndexOf(QChar(',')));
            Msg listMsg;
            QString strt[5] = {"201", "", str, "", ""};
            unicast(createMsg(strt, &listMsg), sender);
        }

}


