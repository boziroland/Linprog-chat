#include "logindialog.h"
#include "ui_logindialog.h"

//LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent),
//    ui(new Ui::LoginDialog)
//{
//    ui->setupUi(this);
//}

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) , ui(new Ui::LoginDialog) {
    //setUpGUI();
    ui->setupUi(this);

    setWindowTitle( tr("User Login") );
    setModal( true );
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
