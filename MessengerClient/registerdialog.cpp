#include "registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    setWindowTitle( tr("Register") );
    setModal( true );
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_buttonBox_accepted()
{
    this->dialog_text[0] = ui->usernameLine->text();
    this->dialog_text[1] = ui->passwordLine->text();
    this->dialog_text[2] = ui->hostLine->text();
    this->dialog_text[3] = ui->emailLine->text();
}
