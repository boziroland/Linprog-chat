#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
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
    QByteArray jsonData;
    Msg msg;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_serverSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_7);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> msg.id;
        socketStream >> msg.username;
        socketStream >> msg.message;
        if (socketStream.commitTransaction()) {
            emit jsonReceived(msg);
            /*
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    emit jsonReceived(jsonDoc.object()); // send the message to the central server
                else
                    emit logMessage("Invalid message: " + QString::fromUtf8(jsonData)); //notify the server of invalid data
            } else {
                emit logMessage("Invalid message: " + QString::fromUtf8(jsonData)); //notify the server of invalid data
            }*/
             // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}


