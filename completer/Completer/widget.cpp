#include "widget.h"
#include "ui_widget.h"
#include <QCompleter>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QStringList wordList;
     wordList << QStringLiteral("信号源1") << QStringLiteral("信号源2") <<  QStringLiteral("无纸化") << QStringLiteral("左中右三屏台");

     QCompleter *completer = new QCompleter(wordList, this);
     completer->setCaseSensitivity(Qt::CaseInsensitive);
     completer->setFilterMode(Qt::MatchContains);
     ui->lineEdit->setCompleter(completer);
}

Widget::~Widget()
{
    delete ui;
}
