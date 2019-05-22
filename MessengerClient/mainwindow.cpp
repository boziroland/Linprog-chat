#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QHostAddress>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), client_socket(new QTcpSocket(this)){

    connect(client_socket, &QTcpSocket::connected, this, &MainWindow::connected);
    connect(this, SIGNAL(signalConnectionStatus(QString)), this, SLOT(slotConnectionStatus(QString)));
    ui->setupUi(this);

}

MainWindow::~MainWindow() {

    delete ui;

}

void MainWindow::SendDataToServer(Msg msg){

    emit signalConnectionStatus(QString("Connecting"));

    if(client_socket->state() == QAbstractSocket::ConnectedState) {
        //QDataStream stream(client_socket);
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_7);

        if(msg.id == QString("001")) { //login esetén
            stream << QString(msg.id);
            stream << msg.username;
            stream << msg.message;
        }
        else stream << QString("asd");

        stream.device()->seek(0); //magic
        client_socket->write(block);

        QString successMsg = "You have successfully logged in";
        QMessageBox::information(this, "Log in", successMsg);

        emit signalConnectionStatus(QString("Connected"));
        return;
    }

    QString errorMsg = "Incorrect username or password";
    QMessageBox::information(this, "Log in", errorMsg);

    emit signalConnectionStatus(QString("Error"));
    return;

}

void MainWindow::on_actionLog_in_triggered() {

    LoginDialog* ld = new LoginDialog(this);
    //ld->exec();

    if(ld->exec() == QDialog::Accepted) {

        client_socket->connectToHost(ld->dialog_text[2], 45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);

        //connect( ld, SIGNAL (acceptLogin(QString&,QString&,int&)), this, SLOT (slotAcceptUserLogin(QString&,QString&)));

        if(!client_socket->waitForConnected()) {
            QString errorMsg = "Error, host not found";
            QMessageBox::information(this, "Error", errorMsg);
            emit signalConnectionStatus(QString("Error"));
            return;
        }

        Msg msg;
        msg.id = QString("001");
        msg.username = ld->dialog_text[0];
        msg.message = ld->dialog_text[1];

        SendDataToServer(msg);

    }
}

// A kapcsolat allapotanak kijelzese es a menuelemek allitasa.
// Attol fuggoen, hogy mi a halozati kapcsolat allapota kulonbozo szoveget irunk ki,
// illetve kulonbozo menupontokat engedelyezunk.
void MainWindow::slotConnectionStatus(QString status) {
    // A halozati kapcsolat allapotanak kiirasa a status bar-ra.
    if(status == QString("Error")) statusBar()->showMessage("Error");
    else if(status == QString("Connecting")) statusBar()->showMessage("Connecting...");
    else if(status == QString("Connected")) statusBar()->showMessage("Connected");
    else if(status == QString("Disconnected")) statusBar()->showMessage("Disconnected");

    if((status == QString("Error")) || (status == QString("Disconnected"))) {
        ui->actionLog_in->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    }
    else if((status == QString("Connecting")) || (status == QString("Connected"))) {
        ui->actionLog_in->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }
}

void MainWindow::on_actionDisconnect_triggered() {
    client_socket->disconnect();
    client_socket->abort();
    emit signalConnectionStatus(QString("Disconnected"));
}

//void MainWindow::slotAcceptUserLogin(QString& uname, QString& pw) {
//    QString msg = "Successfully logged in";
//    QMessageBox::information(this, "Log in", msg);
//}
