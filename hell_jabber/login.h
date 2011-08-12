#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
    class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

signals:
    void on_login(QString name, QString pwd, QString host);
    void on_exit();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::login *ui;
};

#endif // LOGIN_H
