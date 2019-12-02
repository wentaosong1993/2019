#include "langswitch.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(argc<2)
    {
        return 0;
    }

    LangSwitch w;
    w.show();

    return a.exec();
}
