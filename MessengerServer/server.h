#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <iostream>
#include <QDataStream>
#include "database.h"
#include "msg.h"
#include "clienthandler.h"

class Server : public QTcpServer
{
    Q_OBJECT

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    QTcpSocket* server_socket;
    Database* users;
    QVector<ClientHandler*> client_list;

    explicit Server(QObject *parent = nullptr);
    virtual ~Server() override;
    void ReceiveCommunication(Msg);

//signals:

public slots:
    void Ready2Read();
    void userDisconnected();
};

#endif // SERVER_H
