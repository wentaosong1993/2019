#include "customtoolbutton.h"
#include "ui_customtoolbutton.h"
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

CustomToolButton::CustomToolButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomToolButton)
{
    ui->setupUi(this);
    ui->toolBox->removeItem(0);
    for(int i = 0 ;i <10; i++)
    {
        QWidget *tempWidget = new QWidget;
        QVBoxLayout *vLayout = new QVBoxLayout;
        vLayout->setMargin(0);
        tempWidget->setLayout(vLayout);
        for(int j = 0; j < 2; j++)
        {
            QPushButton *btn = new QPushButton;
            btn->setFixedSize(180,100);
            btn->setText(QString("%1").arg(j));
            vLayout->addWidget(btn);
        }
        ui->toolBox->addItem(tempWidget, QString("%1").arg(i));
        QScrollArea* scrollArea = dynamic_cast<QScrollArea*>(ui->toolBox->currentWidget()->parent()->parent());
        if(scrollArea)
        {
            scrollArea->setFixedHeight(100);
        }
        QList<QAbstractButton*> buttonList = ui->toolBox->findChildren<QAbstractButton*>("qt_toolbox_toolboxbutton");
        for(int i = 0; i < buttonList.size();i++)
        {
            buttonList.at(i)->setCheckable(true);
            buttonList.at(i)->setFixedHeight(30);
            buttonList.at(i)->setStyleSheet("QAbstractButton{background-color: red;}");
            connect(buttonList.at(i), SIGNAL(toggled(bool)), this, SLOT(slotButton(bool)));
        }
    }
}

CustomToolButton::~CustomToolButton()
{
    delete ui;
}

void CustomToolButton::slotButton(bool isChecked)
{
    QScrollArea* scrollArea = dynamic_cast<QScrollArea*>(ui->toolBox->currentWidget()->parent()->parent());
    if(scrollArea)
    {
        scrollArea->setVisible(isChecked);
    }
}
