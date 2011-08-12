#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

void login::on_buttonBox_accepted()
{
    QString name = ui->cb_name->currentText();
    QString pwd = ui->le_pwd->text();
    QString host = ui->cb_host->currentText();

    qDebug("%s %s %s",name.toAscii().data(), pwd.toAscii().data(), host.toAscii().data());
    emit on_login(name, pwd, host);
}

void login::on_buttonBox_rejected()
{
    emit on_exit();
}
