#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QHostAddress>

//ilyeneket kene majd kuldozgetni imo
struct Msg{
    QString id;
    QString message;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), client_socket(new QTcpSocket(this)){

    connect(client_socket, &QTcpSocket::connected, this, &MainWindow::connected);
    ui->setupUi(this);

}

MainWindow::~MainWindow(){

    delete ui;

}

void MainWindow::SendDataToServer(){

    if(client_socket->state() == QAbstractSocket::ConnectedState){
        //QDataStream stream(client_socket);
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_7);

        stream << QString("nezeset meg a jarasat");
        stream.device()->seek(0); //magic
        client_socket->write(block);
    }

}

void MainWindow::on_actionLog_in_triggered() {

    LoginDialog* ld = new LoginDialog(this);
    //ld->exec();

    if(ld->exec() == QDialog::Accepted){

        //client_socket->connectToHost(ld->dialog_text[2],45732);
        client_socket->connectToHost(QHostAddress::LocalHost,45732);
        client_socket->waitForConnected();

        SendDataToServer();

    }
}
