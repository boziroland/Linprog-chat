#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <iostream>
#include <QDataStream>

class Server : public QTcpServer
{
    Q_OBJECT

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    QTcpSocket* server_socket;

    explicit Server(QObject *parent = nullptr);
    void ReceiveCommunication(QString);

//signals:

public slots:
    void Ready2Read();
};

#endif // SERVER_H
