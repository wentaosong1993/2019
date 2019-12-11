#include "toolbox.h"
#include "customtoolbutton.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CustomToolButton w;
    w.show();

    return a.exec();
}
