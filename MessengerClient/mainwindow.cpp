#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QHostAddress>

//ilyeneket kene majd kuldozgetni imo
struct Msg {
    QString id; //001: login
    QString username;
    QString message;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), client_socket(new QTcpSocket(this)){

    connect(client_socket, &QTcpSocket::connected, this, &MainWindow::connected);
    ui->setupUi(this);

}

MainWindow::~MainWindow(){

    delete ui;

}

void MainWindow::SendDataToServer(Msg msg){

    if(client_socket->state() == QAbstractSocket::ConnectedState){
        //QDataStream stream(client_socket);
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_7);

        //switch

        stream << msg.id;
        stream << msg.username;
        stream << msg.message;
        stream.device()->seek(0); //magic
        client_socket->write(block);
    }

}

void MainWindow::on_actionLog_in_triggered() {

    LoginDialog* ld = new LoginDialog(this);
    //ld->exec();

    if(ld->exec() == QDialog::Accepted) {

        client_socket->connectToHost(ld->dialog_text[2],45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);
        client_socket->waitForConnected();

        Msg msg;
        msg.messages = new std::vector<QString>();
        msg.username = ld->dialog_text[0];
        msg.message = ld->dialog_text[1];

        SendDataToServer(msg);

    }
}
