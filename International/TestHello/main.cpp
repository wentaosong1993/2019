#include <QApplication>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator *translator = new QTranslator;
    bool state = translator->load("D:/2019/International/TestHello/TestHello.qm");
    qDebug() << "state:" << state;
    a.installTranslator(translator);

    return a.exec();
}
