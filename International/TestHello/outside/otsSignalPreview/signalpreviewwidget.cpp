#include "signalpreviewwidget.h"
#include "previewwidget.h"
#include "otsServer/DataSession.h"
#include "otsXML/globalVar.h"
#include <QDebug>
#include "ui_SignalPreviewWidget.h"
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QPainter>

SignalPreviewWidget::SignalPreviewWidget(QWidget *parent) : QWidget(parent)
, ui(new Ui::SignalPreviewWidget)
, _currentValue(0), _pageValue(0), _maxValue(0)
{
    ui->setupUi(this);
    QBoxLayout* pLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout* pLayout_1 = new QBoxLayout(QBoxLayout::LeftToRight);
    QHBoxLayout* pHLayout = new QHBoxLayout;
    pHLayout->setMargin(0);
    pHLayout->setSpacing(0);
    pLayout->setMargin(0);
    pLayout->setSpacing(10);
    pLayout_1->setMargin(0);
    pLayout_1->setSpacing(10);
    /* 设置方向 */
    QScrollBar *pScrollBar;
    switch (g_SignalPreviewWidgetConfigData.direction)
    {
    case 0:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pLayout_1->setDirection(QBoxLayout::LeftToRight);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 1:
        pLayout->setDirection(QBoxLayout::TopToBottom);
        pLayout_1->setDirection(QBoxLayout::TopToBottom);
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    case 2:
        pLayout->setDirection(QBoxLayout::RightToLeft);
        pLayout_1->setDirection(QBoxLayout::RightToLeft);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 3:
        pLayout->setDirection(QBoxLayout::BottomToTop);
        pLayout_1->setDirection(QBoxLayout::BottomToTop);
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    default:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pLayout_1->setDirection(QBoxLayout::LeftToRight);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    }
    pLayout_1->addWidget(ui->left_pushButton);
    pLayout_1->addWidget(ui->scrollArea);
    pLayout_1->addWidget(ui->right_pushButton);
    pHLayout->addWidget(ui->up_pushButton);
    pHLayout->addWidget(ui->down_pushButton);
    //pLayout_1->addItem(pHLayout);
    ui->scrollAreaWidgetContents->setLayout(pLayout);
    ui->widget_center->setLayout(pLayout_1);

    QObject::connect(ui->left_pushButton, SIGNAL(clicked()), this, SLOT(slotForward()));
    QObject::connect(ui->up_pushButton, SIGNAL(clicked()), this, SLOT(slotForward()));
    QObject::connect(ui->right_pushButton, SIGNAL(clicked()), this, SLOT(slotBackward()));
    QObject::connect(ui->down_pushButton, SIGNAL(clicked()), this, SLOT(slotBackward()));
    QObject::connect(pScrollBar, SIGNAL(rangeChanged(int, int)), this, SLOT(slotRangeChanged(int, int)));
}

SignalPreviewWidget::~SignalPreviewWidget()
{
    delete ui;
}

void SignalPreviewWidget::createTestData()
{
    QList<PreviewWidget*> objectLists = ui->scrollAreaWidgetContents->findChildren<PreviewWidget*>();
    qDeleteAll(objectLists);
    if (_pSpacerItem)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
        _pSpacerItem = 0;
    }

    for (int i = 0; i < 5; i++)
    {
        PreviewWidget* button = new PreviewWidget(ui->scrollAreaWidgetContents);
        button->setMinimumSize(200, 100);
        button->setID(i);
        ui->scrollAreaWidgetContents->layout()->addWidget(button);
    }
    _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
}

void SignalPreviewWidget::changeEvent(QEvent* event)
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

//点击左侧树形信号源、信号源组,初始化信号源预览窗口
void SignalPreviewWidget::slotCreatePreviewWidgetFromGroupID(int ID, int type, QString name)
{
    std::vector<std::tuple<int, std::string, int> > vecAllEncoder;
	//信号源节点
    if (type == 1)
    {
        vecAllEncoder.push_back(std::make_tuple(ID, name.toStdString(), 0));
    }
	//信号源组节点
    else if (type == 0)
    {
		/*
		通过groupid 找到下属节点所有信号源
		parm in parentid 节点ID
		parm out vecAllEncoder 结果信号数组  ID NAME ,MODULE4K
		*/
		//void DataSession::GetAllEncoderByGroupID(int parentid, std::vector<std::tuple<int, std::string, int> >& vecAllEncoder)
        otsServer::DataSession::instance()->GetAllEncoderByGroupID(ID, vecAllEncoder);
    }

    createListData(vecAllEncoder);
}


/* 冗余接口
void SignalPreviewWidget::createListData(std::vector<std::tuple<int, int, std::string>>& vecInfo)
{
    this->clear();
    for (int i = 0; i < vecInfo.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem;

        PreviewWidget* button = new PreviewWidget;
        button->setID(std::get<0>(vecInfo[i]));
        this->insertItem(0, item);
        this->setItemWidget(item, button);
    }
}
*/

//创建信号源预览列表
void SignalPreviewWidget::createListData(std::vector<std::tuple<int, std::string, int>>& vecInfo)
{
    QList<PreviewWidget*> objectLists = ui->scrollAreaWidgetContents->findChildren<PreviewWidget*>();
    qDeleteAll(objectLists);
    if (_pSpacerItem)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
        _pSpacerItem = 0;
    }

    std::vector<std::pair<int, std::string> > vecScreensData;
    otsServer::DataSession::instance()->getAllScreenData(vecScreensData);
    for (int i = 0; i < vecInfo.size(); i++)
    {
        PreviewWidget* button = new PreviewWidget(ui->scrollAreaWidgetContents);
        //button->setStyleSheet("background-color:red;");
        button->setMinimumSize(200, 100);
        button->setID(std::get<0>(vecInfo[i]));
        button->setName(std::get<1>(vecInfo[i]).c_str());
        ui->scrollAreaWidgetContents->layout()->addWidget(button);
    }
    _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
}

void SignalPreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
    QWidget::paintEvent(event);
}

void SignalPreviewWidget::slotForward()
{
    QScrollBar *pScrollBar;
    switch (g_SignalPreviewWidgetConfigData.direction)
    {
    case 0:
    case 2:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 1:
    case 3:
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    default:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    } 
    if (pScrollBar != NULL)
    {
        _currentValue -= _pageValue;
        _currentValue = _currentValue < 0 ? 0 : _currentValue;
        pScrollBar->setValue(_currentValue);
        setShowButton();
    }
}

void SignalPreviewWidget::slotBackward()
{
    QScrollBar *pScrollBar;
    switch (g_SignalPreviewWidgetConfigData.direction)
    {
    case 0:
    case 2:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 1:
    case 3:
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    default:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    }
    if (pScrollBar != NULL)
    {
        _currentValue += _pageValue;
        _currentValue = _currentValue > _maxValue ? _maxValue : _currentValue;
        pScrollBar->setValue(_currentValue);
        setShowButton();
    }
}

void SignalPreviewWidget::slotRangeChanged(int min, int max)
{
    resetValue();
    setShowButton();
}

void SignalPreviewWidget::setShowButton()
{
    if (_maxValue == 0)
    {
        ui->left_pushButton->setEnabled(false);
        ui->right_pushButton->setEnabled(false);
    }
    else if (_currentValue == 0)
    {
        ui->left_pushButton->setEnabled(false);
        ui->right_pushButton->setEnabled(true);
    }
    else if (_currentValue == _maxValue)
    {
        ui->left_pushButton->setEnabled(true);
        ui->right_pushButton->setEnabled(false);
    }
    else
    {
        ui->left_pushButton->setEnabled(true);
        ui->right_pushButton->setEnabled(true);
    }
}

void SignalPreviewWidget::resetValue()
{
    QScrollBar *pScrollBar;
    switch (g_SignalPreviewWidgetConfigData.direction)
    {
    case 0:
    case 2:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 1:
    case 3:
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    default:
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    }
    if (pScrollBar != NULL)
    {
        _currentValue = pScrollBar->value();
        _maxValue = pScrollBar->maximum();
        _pageValue = pScrollBar->pageStep();
    }
    else
    {
        _currentValue = 0;
        _maxValue = 0;
        _pageValue = 0;
    }
}
