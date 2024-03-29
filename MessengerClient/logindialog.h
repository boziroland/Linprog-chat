#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    QString dialog_text[3];
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::LoginDialog *ui;

};

#endif // LOGINDIALOG_H
