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

            qDebug() << "query has this many results: " << qry.value(0).toInt();
            if(qry.value(0).toInt() == 1){
                Msg back;
                back.id = "101";
                back.username = "";
                back.message = "";
                unicast(back, sender);
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


