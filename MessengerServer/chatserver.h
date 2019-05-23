#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
#include "msg.h"
#include <QSqlQuery>
#include "serverdatabase.h"

//class QThread;
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
    void broadcast(const Msg &message, ServerWorker *exclude);
    void unicast(const Msg &message, ServerWorker *include);
    void jsonReceived(ServerWorker *sender, const Msg doc);
    void userDisconnected(ServerWorker *sender);
    void userError(ServerWorker *sender);
private:
    //void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const Msg);
    void sendJson(ServerWorker *destination, const Msg &message);
    QVector<ServerWorker *> m_clients;
    ServerDatabase* users;
};

#endif // CHATSERVER_H
