#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDataStream>

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

public slots:
    void slotConnectionStatus(QString status);
    //void connect();

private slots:
    void on_actionLog_in_triggered();
    void on_actionDisconnect_triggered();
    //void slotAcceptUserLogin(QString& uname, QString& pw);

signals:
    void connected();
    void signalConnectionStatus(QString status);

};

#endif // MAINWINDOW_H
