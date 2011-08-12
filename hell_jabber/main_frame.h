#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include "login.h"
#include "mainwindow.h"


class main_frame : public QObject
{
    Q_OBJECT

public:
    main_frame();



private slots:
    void on_login(QString user_name, QString pwd, QString host);
    void on_exit();

private:
    login *p_login;
    MainWindow *p_main_win;

};

#endif // MAIN_FRAME_H
