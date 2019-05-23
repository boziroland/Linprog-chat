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
{
    //listen(QHostAddress::Any, 45732);
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);

    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }

    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&ChatServer::userDisconnected, this, worker));
    connect(worker, &ServerWorker::error, this, std::bind(&ChatServer::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&ChatServer::jsonReceived, this, worker, std::placeholders::_1));
    connect(worker, &ServerWorker::logMessage, this, &ChatServer::logMessage);

    m_clients.append(worker);
    emit logMessage(QStringLiteral("New client Connected"));
}
void ChatServer::sendJson(ServerWorker *destination, const Msg &message)
{
    Q_ASSERT(destination);
    destination->sendJson(message);
}

void ChatServer::broadcast(const Msg &message, ServerWorker *exclude)
{
    for (ServerWorker *worker : m_clients) {
        //Q_ASSERT(worker);
        if (worker == exclude)
            continue;
        sendJson(worker, message);
    }
}

void ChatServer::unicast(const Msg &message, ServerWorker *include)
{
    for (ServerWorker *worker : m_clients) {
        //Q_ASSERT(worker);
        if (worker == include)
            sendJson(worker, message);
    }
}

void ChatServer::jsonReceived(ServerWorker *sender, const Msg doc)
{
    //Q_ASSERT(sender);
    //emit logMessage("JSON received " + QString::fromUtf8(QJsonDocument(doc).toJson()));

    //if (sender->userName().isEmpty())
      //  return jsonFromLoggedOut(sender, doc);

    jsonFromLoggedIn(sender, doc);
}

void ChatServer::userDisconnected(ServerWorker *sender)
{
    m_clients.removeAll(sender);

    const QString userName = sender->userName();
/*
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = QStringLiteral("userdisconnected");
        disconnectedMessage["username"] = userName;
*/
        Msg msg;
        msg.id = "420";
        msg.username = userName;
        msg.message = " just disconnected!";

        broadcast(msg, nullptr);
        emit logMessage(userName + " disconnected");
    //}
    sender->deleteLater();
}

void ChatServer::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage("Error from " + sender->userName());
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

void ChatServer::jsonFromLoggedIn(ServerWorker *sender, const Msg msg)
{

    if(msg.id == QString("001")) { //login
    /*
            std::string qstr = "select username from users where username = '";
            qstr.append(msg.username.toStdString());
            qstr.append("' and password = '");
            qstr.append(msg.message.toStdString());
            qstr.append("';");
    */

            QString qstr = "select count(username) from users where username = :username and password = :password ;";

            QSqlQuery qry;

            qry.prepare(qstr);

            qry.bindValue(":username", msg.username);
            qry.bindValue(":password", msg.message);

            qry.exec();
            qry.first();


            emit logMessage(msg.username + " is trying to log in..");

            qDebug() << "query has this many results: " << qry.value(0).toInt();
            if(qry.value(0).toInt() == 1){
                Msg backGood;

                QString str[5] = {"101", msg.username, "","",""}; //sikeres login
                unicast(createMsg(str,&backGood), sender);


                emit logMessage(msg.username + " logged in!");
            }else{
                Msg backNotGood;
                QString str[5] = {"102", msg.username, "","",""}; //sikertelen login
                unicast(createMsg(str,&backNotGood), sender);
            }

            //if(users->QueryDB(QString(qstr.c_str()))){

            //qry.exec();

            if(qry.next()) {
                //ha igaz, akkor visszaküldés pozitív megerősítést
                QString addUserStr = "insert into General (username) values (:username);"; //general szobához hozzáadás
                QSqlQuery addUserQry;
                addUserQry.prepare(addUserStr);
                qry.bindValue(":username",msg.username);

                addUserQry.exec();

            } else {
                //hiba
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

            QString addUserStr = "insert into users (username, password, email) values (:username, :password, :email);";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            addUserQry.bindValue(":password", msg.message);
            addUserQry.bindValue(":email", msg.email);
            qDebug() << "the email: "<< msg.email;

            addUserQry.exec();

            Msg SuccessfulRegistrationMsg;
            QString str[5] = {"103", msg.username, "Success!", "", msg.email}; //sikeres regisztráció

            unicast(createMsg(str, &SuccessfulRegistrationMsg),sender);

            emit logMessage(msg.username + " succesfully registered!");

            return;
        }
        if(msg.id == QString("003")){
            broadcast(msg, sender);


            emit logMessage(msg.username + " sent a message.");
        }
        if(msg.id == QString("005")) { //feliratkozás szobára
            QString qstr = "select username from :room where username = :username ;";

            QSqlQuery qry;
            qry.prepare(qstr);
            qry.bindValue(":username",msg.username);
            qry.bindValue(":room", msg.message);
            qry.exec();

            if(qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már feliratkozott a szobára

            QString addUserStr = "insert into :room (username) values (:username);";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            addUserQry.bindValue(":room", msg.message);

            addUserQry.exec();

            return;
        }
        if(msg.id == QString("006")) { //leiratkozás szobáról
            QString qstr = "select username from :room where username = :username ;";

            QSqlQuery qry;
            qry.prepare(qstr);
            qry.bindValue(":username",msg.username);
            qry.bindValue(":room", msg.message);
            qry.exec();

            if(!qry.next()) return; //ha igaz, akkor visszatérünk, a felhasználó már nincs feliratkozva

            QString addUserStr = "delete from :room (username) where username= 'username;";
            QSqlQuery addUserQry;
            addUserQry.prepare(addUserStr);
            addUserQry.bindValue(":username", msg.username);
            addUserQry.bindValue(":room", msg.message);

            addUserQry.exec();

            return;
        }
        if(msg.id == QString("009")) { //kijelentkezés
            for(auto room: users->getRooms()) {
                QString addUserStr = "delete from :room (username) where username= :username;";
                QSqlQuery addUserQry;
                addUserQry.prepare(addUserStr);
                addUserQry.bindValue(":username", msg.username);
                addUserQry.bindValue(":room", msg.message);

                addUserQry.exec();
            }
            return;
        }

    broadcast(msg, sender);
}


