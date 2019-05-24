#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
#include "msg.h"
#include <QSqlQuery>
#include "serverdatabase.h"

class ServerWorker;
class ChatServer : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatServer)
public:
    explicit ChatServer(QObject *parent = nullptr);
    Msg createMsg(QString*, Msg*);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void logMessage(const QString &msg);
public slots:
    void stopServer();
private slots:
    void broadcast(const Msg &message);
    void unicast(const Msg &message, ServerWorker *include);
    void msgReceived(ServerWorker *sender, const Msg doc);
    void userDisconnected(ServerWorker *sender);
    void DisconnectMsg(ServerWorker *sender);
private:
    void msgFromUser(ServerWorker *sender, Msg);
    void sendMsg(ServerWorker *destination, const Msg &message);
    QVector<ServerWorker *> clientList;
    ServerDatabase* users;
};

#endif // CHATSERVER_H
