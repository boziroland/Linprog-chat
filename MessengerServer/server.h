#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <iostream>
#include <QDataStream>


struct Msg{
    QString id;
    QString username;
    QString message;
};

class Server : public QTcpServer
{
    Q_OBJECT

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    QTcpSocket* server_socket;

    explicit Server(QObject *parent = nullptr);
    void ReceiveCommunication(Msg);

//signals:

public slots:
    void Ready2Read();
};

#endif // SERVER_H
