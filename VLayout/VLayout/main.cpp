#include "widget.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
//    QWidget *window = new QWidget;
//    QScrollArea *window = new QScrollArea;
//    QPushButton *button1 = new QPushButton("One");
//    QPushButton *button2 = new QPushButton("Two");
//    QPushButton *button3 = new QPushButton("Three");
//    QPushButton *button4 = new QPushButton("Four");
//    QPushButton *button5 = new QPushButton("Five");
//    QPushButton *button6 = new QPushButton("One");
//    QPushButton *button7 = new QPushButton("Two");
//    QPushButton *button8 = new QPushButton("Three");
//    QPushButton *button9 = new QPushButton("Four");
//    QPushButton *button10 = new QPushButton("Five");

//    QVBoxLayout *layout = new QVBoxLayout;
//    window->setLayout(layout);
//    layout->addWidget(button1);
//    layout->addWidget(button2);
//    layout->addWidget(button3);
//    layout->addWidget(button4);
//    layout->addWidget(button5);
//    layout->addWidget(button6);
//    layout->addWidget(button7);
//    layout->addWidget(button8);
//    layout->addWidget(button9);
//    layout->addWidget(button10);

//    window->setLayout(layout);
//    window->show();

    return a.exec();
}
