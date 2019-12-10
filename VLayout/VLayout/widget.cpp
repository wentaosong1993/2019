#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    QPushButton *button1 = new QPushButton("One");
    QPushButton *button2 = new QPushButton("Two");
    QPushButton *button3 = new QPushButton("Three");
    QPushButton *button4 = new QPushButton("Four");
    QPushButton *button5 = new QPushButton("Five");
    QPushButton *button6 = new QPushButton("One");
    QPushButton *button7 = new QPushButton("Two");
    QPushButton *button8 = new QPushButton("Three");
    QPushButton *button9 = new QPushButton("Four");
    QPushButton *button10 = new QPushButton("Five");

    QWidget* tes = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    ui->scrollAreaWidgetContents->setLayout(layout);
    tes->setLayout(layout);
    layout->addWidget(button1);
    layout->addWidget(button2);
    layout->addWidget(button3);
    layout->addWidget(button4);
    layout->addWidget(button5);
    layout->addWidget(button6);
    layout->addWidget(button7);
    layout->addWidget(button8);
    layout->addWidget(button9);
    layout->addWidget(button10);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    ui->scrollArea->setWidgetResizable(false);
//    ui->scrollArea->layout()->set;
    ui->scrollArea->show();

    QScrollArea * aa = new QScrollArea;
    aa->setWidget(tes);
aa->show();
}

Widget::~Widget()
{
    delete ui;
}
