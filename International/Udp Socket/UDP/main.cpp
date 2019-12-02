#include "qudpapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QUdpApp w;
    w.show();

    return a.exec();
}
