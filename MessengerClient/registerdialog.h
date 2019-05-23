#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    QString dialog_text[3];
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
