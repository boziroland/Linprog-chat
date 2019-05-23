#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include "QTcpServer"

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    //QTcpServer::listen(Q)
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);
    // forward the disconnected and error signals coming from the socket
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
}


bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendJson(const Msg &message)
{
    // we crate a temporary QJsonDocument forom the object and then convert it
    // to its UTF-8 encoded version. We use QJsonDocument::Compact to save bandwidth
    //const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);


    // we notify the central server we are about to send the message
    emit logMessage("Sending to " + userName() + " - " + message.message);
    // we send the message to the socket in the exact same way we did in the client


    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << message.id;
    socketStream << message.username;
    socketStream << message.message;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
}

void ServerWorker::receiveJson()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    //QByteArray jsonData;
    Msg msg;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    while (1) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();

        socketStream >> msg.id;
        socketStream >> msg.username;
        socketStream >> msg.message;

        if (socketStream.commitTransaction()) {
            emit jsonReceived(msg);
        } else {
            break;
        }
    }
}


