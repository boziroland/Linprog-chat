#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QDataStream>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SendDataToServer(Msg msg);

private:
    Ui::MainWindow *ui;
    QTcpSocket* client_socket;
/*
public slots:
    void connect();
*/
private slots:
    void on_actionLog_in_triggered();

signals:
    void connected();

};

#endif // MAINWINDOW_H
