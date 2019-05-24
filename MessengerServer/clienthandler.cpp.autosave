#include "clienthandler.h"
#include <QDataStream>

ClientHandler::ClientHandler(QObject* parent) : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::ReceiveMsg);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::getDisconnected);
}

ClientHandler::~ClientHandler(){}

void ClientHandler::ReceiveMsg(){
    QDataStream stream(socket);

    stream.setVersion(QDataStream::Qt_5_7);

    QString str;
    Msg msg;

    while(1){
        stream.startTransaction();

        stream >> msg.id;
        stream >> msg.username;
        stream >> msg.message;
        stream >> msg.room;
        stream >> msg.email;

        if(stream.commitTransaction()){
            emit(sendMsgToServer(msg));
        }else{
            break;
        }
    }
}
/*
void ClientHandler::getDisconnected(){
    //socket->disconnectFromHost();
}*/

QString ClientHandler::userName() const
{
    return name;
}

void ClientHandler::setUserName(QString userName)
{
    name = userName;
}
bool ClientHandler::setSocketD(qintptr descriptor){
    return socket->setSocketDescriptor(descriptor);
}

void ClientHandler::disconnectFromClient(){

}
