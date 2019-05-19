#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent), server_socket(new QTcpSocket(this)){

    connect(server_socket, &QTcpSocket::readyRead, this, &Server::Ready2Read);

    listen(QHostAddress::Any,45732);
    std::cout << "Server created!" << std::endl;
}

void Server::ReceiveCommunication(QString str){
    //std::cout << "tell me its working" << std::endl;
    std::cout << str.toStdString() << std::endl;
}

void Server::incomingConnection(qintptr socketDescriptor){
    std::cout << "A client would like to join!" << std::endl;
    server_socket->setSocketDescriptor(socketDescriptor);

}

void Server::Ready2Read(){
    std::cout << "Clients message:" << std::endl;

    QDataStream stream(server_socket);

    stream.setVersion(QDataStream::Qt_5_7);

    QString str;

    while(1){
        stream.startTransaction();
        stream >> str;

        //ReceiveCommunication(str);
        if(stream.commitTransaction()){
            ReceiveCommunication(str);
        }else{
            break;
        }
    }
}
