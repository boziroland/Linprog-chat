#include "serverworker.h"
#include <QDataStream>
#include "QTcpServer"

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveMsg);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
}


bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendMsg(const Msg &message)
{
    emit logMessage("Communicating with " + userName() + " about something.. " + message.message);

    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << message.id;
    socketStream << message.username;
    socketStream << message.message;
    socketStream << message.room;
    socketStream << message.email;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->close();
}

QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
}

void ServerWorker::receiveMsg()
{
    Msg msg;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    while (1) {
        socketStream.startTransaction();

        socketStream >> msg.id;
        socketStream >> msg.username;
        socketStream >> msg.message;
        socketStream >> msg.room;
        socketStream >> msg.email;

        if (socketStream.commitTransaction()) {
            emit msgReceived(msg);
        } else {
            break;
        }
    }
}


