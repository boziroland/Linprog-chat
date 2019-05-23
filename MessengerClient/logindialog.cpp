#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), ui(new Ui::LoginDialog){
    ui->setupUi(this);

    setWindowTitle( tr("User Login") );
    setModal( true );
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_accepted()
{
    this->dialog_text[0] = ui->usernameLine->text();
    this->dialog_text[1] = ui->passwordLine->text();
    this->dialog_text[2] = ui->hostLine->text();
}
