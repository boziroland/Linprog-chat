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
    roomsModel = nullptr;

    ui->setupUi(this);

    setWindowTitle( tr("Messenger Application") );

}

MainWindow::~MainWindow() {
    delete currentUser;
    delete roomsModel;
    delete ui;

}

void MainWindow::SendDataToServer(Msg msg) {

    if(client_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray block;
        QDataStream stream(&block, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_5_7);

        if(msg.id == QString("001")) emit signalConnectionStatus(QString("Connecting"));

        stream << QString(msg.id);
        stream << msg.username;
        stream << msg.message;
        stream << msg.room;
        stream << msg.email;

        stream.device()->seek(0); //magic
        client_socket->write(block);

        return;
    }

    QString errorMsg = "Successfully disconnected.";
    QMessageBox::information(this, "Disconnect", errorMsg);

    return;

}

void MainWindow::on_actionLog_in_triggered() {
    LoginDialog* ld = new LoginDialog(this);

    if(ld->exec() == QDialog::Accepted) {

        if(ld->dialog_text[0].isNull() || ld->dialog_text[1].isNull()) {
            QString errorMsg = "Please enter your username and password.";
            QMessageBox::information(this, "Error", errorMsg);
            delete ld;
            return;
        }

        client_socket->connectToHost(ld->dialog_text[2], 45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);

        if(!client_socket->waitForConnected(3000)) {
            QString errorMsg = "Error, host not found";
            QMessageBox::information(this, "Error", errorMsg);
            emit signalConnectionStatus(QString("Error"));
            delete ld;
            return;
        }

        Msg msg;
        msg.id = QString("001");
        msg.username = ld->dialog_text[0];
        msg.message = ld->dialog_text[1];
        msg.room = "";
        msg.email = "";

        SendDataToServer(msg);

    }
    delete ld;
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
    Msg msg;
    msg.id = QString("009");
    if(currentUser!=nullptr) msg.username = currentUser->getUsername();
    else msg.username = "";
    msg.message = "";
    msg.room = "";
    msg.email = "";

    SendDataToServer(msg);

    client_socket->disconnect();
    client_socket->disconnectFromHost();
    //client_socket->abort();

    if(currentUser!=nullptr) delete currentUser;
    if(roomsModel!=nullptr) delete roomsModel;
    ui->logTextEdit->clear();


    emit signalConnectionStatus(QString("Disconnected"));

    QApplication::quit();
}

void MainWindow::on_pushButton_clicked() {
    if(client_socket->state() != QAbstractSocket::ConnectedState) return;

    Msg msg;
    msg.id = QString("003");
    msg.username = currentUser->getUsername();
    msg.message = ui->msgLineEdit->text();
    msg.room = currentUser->getCurrentRoom();
    msg.email = "";

    SendDataToServer(msg);
    ui->msgLineEdit->clear();
}

void MainWindow::on_actionRegister_triggered() {
    RegisterDialog* rd = new RegisterDialog(this);

    if(rd->exec() == QDialog::Accepted) {

        if(rd->dialog_text[0] == NULL || rd->dialog_text[1] == NULL || rd->dialog_text[3] == NULL) {
            QString errorMsg = "Invalid username, password or e-mail.";
            QMessageBox::information(this, "Error", errorMsg);
            delete rd;
            return;
        }

        client_socket->connectToHost(rd->dialog_text[2], 45732);
        //client_socket->connectToHost(QHostAddress::LocalHost,45732);

        if(!client_socket->waitForConnected(3000)) {
            QString errorMsg = "Error, host not found";
            QMessageBox::information(this, "Error", errorMsg);
            delete rd;
            return;
        }

        Msg msg;
        msg.id = QString("002");
        msg.username = rd->dialog_text[0];
        msg.message = rd->dialog_text[1];
        msg.room = "";
        msg.email = rd->dialog_text[3];

        SendDataToServer(msg);

    }
    delete rd;
}

void MainWindow::onReadyRead() {
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
        socketStream >> msg.room;
        socketStream >> msg.email;

        if (socketStream.commitTransaction()) {
            if(msg.id == QString("101")) { //login elfogadva
                QString successMsg = "You have successfully logged in";
                QMessageBox::information(this, "Log in", successMsg);

                if(currentUser!=nullptr) delete currentUser;
                currentUser = new User(msg.username);

                getRooms();

                ui->unsubscribeButton->setEnabled(true);

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
            if(msg.id == QString("105")) { //üzenet érkezett

                QString roomName = msg.room;
                for(auto room: currentUser->getRooms()) {
                    if(room->getRoomName() == roomName) {
                        room->addToLog(msg.username + ": " + msg.message);
                        if(roomName == currentUser->getCurrentRoom()) refreshLog();
                        break;
                    }
                }
            }
            if(msg.id == QString("201")) { //szobaadatok érkeztek
                QStringList roomNames = msg.message.split(",");

                for(auto name: roomNames) {
                    currentUser->addRoom(new Room(name));
                }
                updateRooms();
            }
        } else {
            break;
        }
    }
}

void MainWindow::getRooms() {
    if(client_socket->state() != QAbstractSocket::ConnectedState) return;
    qDebug() << "lekérdezés";
    Msg msg;
    msg.id = QString("200");
    msg.username = "";
    msg.message = "";
    msg.room = "";
    msg.email = "";

    SendDataToServer(msg);

}

void MainWindow::updateRooms() {
    if(roomsModel!=nullptr) delete roomsModel;
    roomsModel = new QStringListModel(this);

    QStringList list;

    for(auto room: currentUser->getRooms()) list << room->getRoomName();

    roomsModel->setStringList(list);
    ui->roomsListView->setModel(roomsModel);
}

void MainWindow::refreshLog() {
    ui->logTextEdit->clear();
    for(auto room: currentUser->getRooms()) {
        if(room->getRoomName() == currentUser->getCurrentRoom()) {
            QString text = "";

            for(auto fragment: room->getLog()) {
                text = text + fragment + "\n";
            }
            ui->logTextEdit->setText(text);
            break;
        }
    }
}

void MainWindow::on_roomsListView_clicked(const QModelIndex &index)
{
    currentUser->setCurrentRoom( roomsModel->data(index).toString() );//roomsModel->stringList().at(index) );

    refreshLog();

    for(auto room: currentUser->getRooms()) {
        if(room->getRoomName() == currentUser->getCurrentRoom()) {
            room->subscribeTo();

            Msg msg;
            msg.id = QString("005");
            msg.username = currentUser->getUsername();
            msg.message = "";
            msg.room = room->getRoomName();
            msg.email = "";

            SendDataToServer(msg);

            break;
        }
    }
}

void MainWindow::on_unsubscribeButton_clicked()
{
    if(currentUser->getCurrentRoom().isNull()) return;

    for(auto room: currentUser->getRooms()) {
        if(room->getRoomName() == currentUser->getCurrentRoom()) {
            room->unsubscribeFrom();

            currentUser->setCurrentRoom("");

            Msg msg;
            msg.id = QString("006");
            msg.username = currentUser->getUsername();
            msg.message = "";
            msg.room = room->getRoomName();
            msg.email = "";

            SendDataToServer(msg);

            break;
        }
    }
}
