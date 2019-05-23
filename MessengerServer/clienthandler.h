#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include "msg.h"

class ClientHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ClientHandler)

private:
    QTcpSocket* socket;
    QString name;
public:
    explicit ClientHandler(QObject *parent = nullptr);
    virtual ~ClientHandler();
    QString userName() const;
    void setUserName(QString);
    void sendMsg();
    bool setSocketD(qintptr);

    void ReceiveMsg();
signals:
    void getDisconnected();
    void receivedMsg();
    void sendMsgToServer(Msg);
public slots:
    void disconnectFromClient();

};

#endif // CLIENTHANDLER_H
