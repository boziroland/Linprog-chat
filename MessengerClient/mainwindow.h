#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDataStream>
#include "user.h"
#include "registerdialog.h"

namespace Ui {
    class MainWindow;
}

//ilyeneket kene majd kuldozgetni imo
struct Msg {
    QString id; //001: login
    QString username;
    QString message;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SendDataToServer(Msg msg);

private:
    Ui::MainWindow *ui;
    QTcpSocket* client_socket;
    User* currentUser;

public slots:
    void slotConnectionStatus(QString status);
    //void connect();

private slots:
    void on_actionLog_in_triggered();
    void on_actionDisconnect_triggered();
    //void slotAcceptUserLogin(QString& uname, QString& pw);

    void on_pushButton_clicked();

    void on_actionRegister_triggered();

signals:
    void connected();
    void signalConnectionStatus(QString status);

};

#endif // MAINWINDOW_H
