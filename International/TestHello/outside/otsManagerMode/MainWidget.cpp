#include "MainWidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QDateTime>
#include "signalgroupManageWidget.h"
#include "AddAccount.h"
#include <QDebug>
#include <QGridLayout>
#include "ScreenWidgetManage.h"
#include "otsXML/globalVar.h"

using namespace otsManagerWidget;

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    createUI();
}

void MainWidget::createUI()
{
	///** 去标题栏 */
	this->setWindowFlags(Qt::FramelessWindowHint);
	
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	hTitleLayout->addSpacing(10);

	/** 创建Logo显示 */
	_pLogoLabel = new QLabel(this);
	_pLogoLabel->setObjectName("M-LogoLabel");

	///** 创建时间 */
	_pTimeLabel = new QLabel(this);
	_pTimeLabel->setObjectName("M-TimeLabel");
	QDateTime current_date_time = QDateTime::currentDateTime();
	
	QString current_date = current_date_time.toString("yyyy-MM-dd ddd HH:mm");
	_pTimeLabel->setText(current_date);

	/** 创建最小化按钮 */
	_pMiniBT = new QPushButton(this);
	_pMiniBT->setObjectName("M-MiniButton");
	QObject::connect(_pMiniBT, SIGNAL(clicked()), this, SLOT(showMinimized()));

	/** 创建关闭按钮 */
	_pCloseBT = new QPushButton(this);
	_pCloseBT->setObjectName("M-CloseButton");
	QObject::connect(_pCloseBT, SIGNAL(clicked()), this, SLOT(close()));

	hTitleLayout->addWidget(_pLogoLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5,0,QSizePolicy::Expanding, QSizePolicy::Fixed));
	hTitleLayout->addWidget(_pTimeLabel);
	
	hTitleLayout->addWidget(_pMiniBT);
	hTitleLayout->addWidget(_pCloseBT);

	QHBoxLayout *hViewLayout = new QHBoxLayout;
	hViewLayout->setMargin(0);
	hViewLayout->setObjectName("M-hViewLayout");
	// 创建信号源树,数据录入
    _pSignalGroupTree = new SignalGroupManageWidget(this);
    hViewLayout->addWidget(_pSignalGroupTree);
	hViewLayout->addSpacing(20);

	//录入用户数据控件，实现增、删、改、查
	_pAddAccount = new AddAccountWidget(this);
	_pAddAccount->setObjectName("M-accountWidget");
	hViewLayout->addWidget(_pAddAccount);
	hViewLayout->addSpacing(20);

	//录入大屏数据控件，鼠标左键实现大屏数据预览，右键实现大屏数据的增、删、改
	_pScreenManageWidget = new ScreenManageWidget(this);
	hViewLayout->addWidget(_pScreenManageWidget);

	QVBoxLayout *vAllLayout = new QVBoxLayout;
	vAllLayout->setContentsMargins(0, 5, 0, 0);
	
	vAllLayout->addLayout(hTitleLayout);
	vAllLayout->addSpacerItem(new QSpacerItem(5, 35, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vAllLayout->addLayout(hViewLayout);

	this->setLayout(vAllLayout);
}


void MainWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
    QWidget::paintEvent(event);
}

void MainWidget::timerEvent(QTimerEvent *event)
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy-MM-dd ddd HH:mm");
	_pTimeLabel->setText(current_date);
}

void MainWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_pMiniBT->setText(_pMiniBT->text());
			_pCloseBT->setText(_pCloseBT->text());
			_pTimeLabel->setText(_pTimeLabel->text());
			_pLogoLabel->setText(_pLogoLabel->text());
		}
		break;
	default:
		break;
	}
}

void otsManagerWidget::MainWidget::createUIData()
{
    _pSignalGroupTree->createUI();
    _pAddAccount->initData();
    _pScreenManageWidget->createData();
}

