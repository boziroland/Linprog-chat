#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent), server_socket(new QTcpSocket(this)){

    connect(server_socket, &QTcpSocket::readyRead, this, &Server::Ready2Read);

    listen(QHostAddress::Any, 45732);
    std::cout << "Server created!" << std::endl;
}

void Server::ReceiveCommunication(Msg msg){
    //std::cout << "tell me its working" << std::endl;
    std::cout << msg.id.toStdString() << std::endl;
    std::cout << msg.username.toStdString() << std::endl;
    std::cout << msg.message.toStdString() << std::endl;
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
    Msg msg;// = new Msg() ;

    while(1){
        stream.startTransaction();
        //stream >> str;
        stream >> msg.id;
        stream >> msg.username;
        stream >> msg.message;

        //stream >> ;

        //ReceiveCommunication(str);
        if(stream.commitTransaction()){
            ReceiveCommunication(msg);
        }else{
            break;
        }
    }
}
