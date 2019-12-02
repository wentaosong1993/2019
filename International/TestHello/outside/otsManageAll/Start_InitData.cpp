#include "Start_InitData.h"
#include "ui_Start_InitData.h"

Start_InitData::Start_InitData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Start_InitData)
{
    ui->setupUi(this);
}

Start_InitData::~Start_InitData()
{
    delete ui;
}

void Start_InitData::setDiaplayText(const QString &strDisplayText)
{
    ui->label->setText(strDisplayText);
}

void Start_InitData::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			ui->retranslateUi(this);
		}
		break;
	default:
		break;
	}
}
