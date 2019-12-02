#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator * translator = new QTranslator;
    translator->load("D:/2019/Test/Test");
    a.installTranslator(translator);

    return a.exec();
}
