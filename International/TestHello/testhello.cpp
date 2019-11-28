#include "testhello.h"
#include "ui_testhello.h"

TestHello::TestHello(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestHello)
{
    ui->setupUi(this);
}

TestHello::~TestHello()
{
    delete ui;
}
