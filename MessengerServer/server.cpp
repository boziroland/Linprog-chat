#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent), server_socket(new QTcpSocket(this)), users(new Database()){

    connect(server_socket, &QTcpSocket::readyRead, this, &Server::Ready2Read);

    listen(QHostAddress::Any, 45732);
    std::cout << "Server created!" << std::endl;

    Database people;
}

Server::~Server(){
    delete server_socket;
    delete users;
}

void Server::incomingConnection(qintptr socketDescriptor){
    std::cout << "A client would like to join!" << std::endl;
    server_socket->setSocketDescriptor(socketDescriptor);

}

void Server::ReceiveCommunication(Msg msg){
    std::cout << msg.id.toStdString() << std::endl;
    std::cout << msg.username.toStdString() << std::endl;
    std::cout << msg.message.toStdString() << std::endl;

    if(msg.id == QString("001")){ //login

        std::string qstr = "select username from users where username = '";
        qstr.append(msg.username.toStdString());
        qstr.append("' and password = '");
        qstr.append(msg.message.toStdString());
        qstr.append("';");

        if(users->QueryDB(QString(qstr.c_str()))){
            //ha igaz, akkor visszaküldés pozitív megerősítést
        }
    }

}

void Server::Ready2Read(){
    std::cout << "Clients message:" << std::endl;

    QDataStream stream(server_socket);

    stream.setVersion(QDataStream::Qt_5_7);

    QString str;
    Msg msg;

    while(1){
        stream.startTransaction();

        stream >> msg.id;
        stream >> msg.username;
        stream >> msg.message;

        if(stream.commitTransaction()){
            ReceiveCommunication(msg);
        }else{
            break;
        }
    }
}
