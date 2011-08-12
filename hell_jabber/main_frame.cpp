#include "main_frame.h"

#define DEBUG_MODE      1

main_frame::main_frame()
{
#if DEBUG_MODE

    p_main_win = new MainWindow(NULL, "debug", "debug", "192.168.2.168");
    p_main_win->show();
#else
    p_login = new login;
    connect(p_login, SIGNAL(on_login(QString, QString, QString)),
            this, SLOT(on_login(QString, QString, QString)));
    connect(p_login, SIGNAL(on_exit()), this, SLOT(on_exit()));
    p_login->show();
#endif
}

void main_frame::on_login(QString name, QString pwd, QString host)
{
    qDebug("on_login: name = %s, pwd = %s, host = %s",
           name.toAscii().data(), pwd.toAscii().data(), host.toAscii().data());

    if(name.isEmpty() || pwd.isEmpty() || pwd.isEmpty()) {
        p_login->show();
        qDebug("some message is empty");
        return;
    }

    p_main_win = new MainWindow(NULL, name, pwd, host);
    p_main_win->show();
}

void main_frame::on_exit()
{
    qDebug("on_exit");
}
