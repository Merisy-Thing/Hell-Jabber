#include <QtGui/QApplication>
#include "main_frame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    new main_frame;

    return a.exec();
}
