#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDataStream>
#include "user.h"
#include "registerdialog.h"
#include <QStringListModel>

namespace Ui {
    class MainWindow;
}

//ilyeneket kene majd kuldozgetni imo
struct Msg {
    QString id; //001: login
    QString username;
    QString message;
    QString room;
    QString email;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SendDataToServer(Msg msg);
    void getRooms();
    void refreshLog();
    void updateRooms();

private:
    Ui::MainWindow *ui;
    QTcpSocket* client_socket;
    User* currentUser;
    QStringListModel* roomsModel;

public slots:
    void slotConnectionStatus(QString status);
    //void connect();

private slots:
    void on_actionLog_in_triggered();
    void on_actionDisconnect_triggered();
    //void slotAcceptUserLogin(QString& uname, QString& pw);

    void on_pushButton_clicked();
    void onReadyRead();
    void on_actionRegister_triggered();

    void on_roomsListView_doubleClicked(const QModelIndex &index);

    void on_roomsListView_clicked(const QModelIndex &index);

signals:
    void connected();
    void signalConnectionStatus(QString status);


};

#endif // MAINWINDOW_H
