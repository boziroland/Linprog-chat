#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include <QHostAddress>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), client_socket(new QTcpSocket(this)){

    connect(client_socket, &QTcpSocket::connected, this, &MainWindow::connected);
    connect(this, SIGNAL(signalConnectionStatus(QString)), this, SLOT(slotConnectionStatus(QString)));
    connect(client_socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);

    currentUser = nullptr;

    ui->setupUi(this);

}

MainWindow::~MainWindow() {
    delete currentUser;
    delete ui;

}

void MainWindow::SendDataToServer(Msg msg) {

    if(client_socket->state() == QAbstractSocket::ConnectedState) {
        //QDataStream stream(client_socket);
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_7);

        if(msg.id == QString("001")) { //login esetén
            emit signalConnectionStatus(QString("Connecting"));

            stream << QString(msg.id);
            stream << msg.username;
            stream << msg.message;

            //várunk

//            QString successMsg = "You have successfully logged in";
//            QMessageBox::information(this, "Log in", successMsg);

//            if(currentUser!=nullptr) delete(currentUser);
//            currentUser = new User(msg.username);

//            emit signalConnectionStatus(QString("Connected"));
        }
        else if(msg.id == QString("002")) { //regisztráció esetén
            stream << QString(msg.id);
            stream << msg.username;
            stream << msg.message;
        }
        else if(msg.id == QString("003")) { //üzenetküldés esetén
            stream << QString(msg.id);
            stream << msg.username;
            stream << msg.message;
        }
        else if(msg.id == QString("009")) { //üzenetküldés esetén
            stream << QString(msg.id);
            stream << msg.username;
            stream << msg.message;
        }
        else stream << QString("asd");

        stream.device()->seek(0); //magic
        client_socket->write(block);

        return;
    }

    QString errorMsg = "Incorrect username or password";
    QMessageBox::information(this, "Log in", errorMsg);

    emit signalConnectionStatus(QString("Error"));
    return;

}

void MainWindow::on_actionLog_in_triggered() {
    LoginDialog* ld = new LoginDialog(this);

    if(ld->exec() == QDialog::Accepted) {

        if(ld->dialog_text[0].isNull() || ld->dialog_text[1].isNull()) {
            QString errorMsg = "Please enter your username and password.";
            QMessageBox::information(this, "Error", errorMsg);
            delete(ld);
            return;
        }

        client_socket->connectToHost(ld->dialog_text[2], 45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);

        if(!client_socket->waitForConnected(3000)) {
            QString errorMsg = "Error, host not found";
            QMessageBox::information(this, "Error", errorMsg);
            emit signalConnectionStatus(QString("Error"));
            delete(ld);
            return;
        }

        Msg msg;
        msg.id = QString("001");
        msg.username = ld->dialog_text[0];
        msg.message = ld->dialog_text[1];

        SendDataToServer(msg);

    }
    delete(ld);
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

    Msg msg;
    msg.id = QString("009");
    if(currentUser!=nullptr) msg.username = currentUser->getUsername();
    else msg.username = "";
    msg.message = "";

    SendDataToServer(msg);

    if(currentUser!=nullptr) delete(currentUser);

    emit signalConnectionStatus(QString("Disconnected"));
}

void MainWindow::on_pushButton_clicked() {
    if(client_socket->state() != QAbstractSocket::ConnectedState) return;

    Msg msg;
    msg.id = QString("003");
    msg.username = currentUser->getUsername();
    msg.message = ui->msgLineEdit->text();

    //QMessageBox::information(this, "Debug", msg.message);

    SendDataToServer(msg);
    ui->msgLineEdit->clear();
}

void MainWindow::on_actionRegister_triggered() {
    RegisterDialog* rd = new RegisterDialog(this);

    if(rd->exec() == QDialog::Accepted) {

        if(rd->dialog_text[0] == NULL || rd->dialog_text[1] == NULL) {
            QString errorMsg = "Please enter a valid username and password.";
            QMessageBox::information(this, "Error", errorMsg);
            delete(rd);
            return;
        }

        client_socket->connectToHost(rd->dialog_text[2], 45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);

        if(!client_socket->waitForConnected(3000)) {
            QString errorMsg = "Error, host not found";
            QMessageBox::information(this, "Error", errorMsg);
            delete(rd);
            return;
        }

        Msg msg;
        msg.id = QString("002");
        msg.username = rd->dialog_text[0];
        msg.message = rd->dialog_text[1];

        SendDataToServer(msg);

    }
    delete(rd);
}

void MainWindow::onReadyRead() {
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    //QByteArray jsonData;
    Msg msg;
    // create a QDataStream operating on the socket
    QDataStream socketStream(client_socket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    while (1) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();

        socketStream >> msg.id;
        socketStream >> msg.username;
        socketStream >> msg.message;

        if (socketStream.commitTransaction()) {
            if(msg.id == QString("101")) { //login elfogadva
                QString successMsg = "You have successfully logged in";
                QMessageBox::information(this, "Log in", successMsg);

                if(currentUser!=nullptr) delete(currentUser);
                currentUser = new User(msg.username);

                emit signalConnectionStatus(QString("Connected"));
            }
            if(msg.id == QString("102")) { //login elutasítva
                QString errorMsg = "Invalid username or password";
                QMessageBox::information(this, "Error", errorMsg);

                emit signalConnectionStatus(QString("Error"));
            }
            if(msg.id == QString("103")) { //regisztráció sikeres
                QMessageBox::information(this, "Register", "Thank you for joining, please log in.");
            }
            if(msg.id == QString("104")) { //regisztráció sikertelen
                QMessageBox::information(this, "Register", "Invalid username or password.");
            }
        } else {
            break;
        }
    }
}
