#include "screenswitchwidget.h"
#include <QPushButton>
#include "otsServer/DataSession.h"
#include <QDebug>
#include "otsXML/globalVar.h"
#include "ui_ScreenSwitchWidget.h"
#include <QBoxLayout>
#include <QScrollBar>
#include <QSettings>

/******************************** ScreenSwitchWidget_ ******************************************/
//设置大屏列表默认的排布方式
ScreenSwitchWidget::ScreenSwitchWidget(QWidget *parent) :QWidget(parent), ui(new Ui::ScreenSwitchWidget)
, _currentValue(0), _pageValue(0), _maxValue(0)
{
    _oldButton = 0;
    _pSpacerItem = 0;
    _mapButton2ID.clear();
    ui->setupUi(this);
    QBoxLayout* pLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    pLayout->setMargin(0);
    pLayout->setSpacing(50);
    /* 设置方向 */
    QScrollBar *pScrollBar;
    switch (g_ScreenSwitchWidgetConfigData.direction)
    {
    case 0:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 1:
        pLayout->setDirection(QBoxLayout::TopToBottom);
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    case 2:
        pLayout->setDirection(QBoxLayout::RightToLeft);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    case 3:
        pLayout->setDirection(QBoxLayout::BottomToTop);
        pScrollBar = ui->scrollArea->verticalScrollBar();
        break;
    default:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pScrollBar = ui->scrollArea->horizontalScrollBar();
        break;
    }

    ui->scrollAreaWidgetContents->setLayout(pLayout);
    QObject::connect(pScrollBar, SIGNAL(rangeChanged(int, int)), this, SLOT(slotRangeChanged(int, int)));
}

ScreenSwitchWidget::~ScreenSwitchWidget()
{
    delete ui;
}

//测试程序
void ScreenSwitchWidget::createTestData()
{
    QList<QPushButton*> objectLists = ui->scrollAreaWidgetContents->findChildren<QPushButton*>();
    qDeleteAll(objectLists);
    if (_pSpacerItem)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
        _pSpacerItem = 0;
    }
    _oldButton = 0;
    _mapButton2ID.clear();
    for (int i = 0; i < 5; i++)
    {
        QPushButton* button = new QPushButton(this);
        button->setText("test");
        button->setCheckable(true);
        _mapButton2ID[button] = i;
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
        ui->scrollAreaWidgetContents->layout()->addWidget(button);
    }
    _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
}
//获取大屏信息(ID,Name)，列表方式呈现
void ScreenSwitchWidget::createData()
{
    QList<QPushButton*> objectLists = ui->scrollAreaWidgetContents->findChildren<QPushButton*>();
    qDeleteAll(objectLists);
    if (_pSpacerItem)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
        _pSpacerItem = 0;
    }
    _oldButton = 0;
    _mapButton2ID.clear();

    std::vector<std::pair<int, std::string> > vecScreensData;
    otsServer::DataSession::instance()->getAllScreenData(vecScreensData);
    for (int i = 0; i < vecScreensData.size(); i++)
    {
        QPushButton* button = new QPushButton(ui->scrollAreaWidgetContents);
        button->setObjectName("ScreenSwitchButton");
        button->setCheckable(true);
        int id = vecScreensData[i].first;
        QString name = vecScreensData[i].second.c_str();
        button->setText(name);
        ui->scrollAreaWidgetContents->layout()->addWidget(button);
        _mapButton2ID[button] = id;
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    }
    _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
}
//默显示第一个大屏,新版有问题，MAP排序了
void ScreenSwitchWidget::setDefaultScreen()
{
    if ((!_oldButton) && (!_mapButton2ID.isEmpty()))
    {
        QSettings setting("lastScreen.ini", QSettings::IniFormat);
        int lastScreenID = setting.value("LastScreenID", 0).toInt();
        _oldButton = _mapButton2ID.key(lastScreenID, NULL);
        if (_oldButton == NULL)
        {
            _oldButton = _mapButton2ID.firstKey();
            setting.setValue("LastScreenID", _mapButton2ID[_oldButton]);
        }
        _oldButton->click();
        emit signalScreenID(_mapButton2ID[_oldButton]);
        resetValue();
        setShowButton();
    }
}

void ScreenSwitchWidget::changeEvent(QEvent *event)
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

//切换大屏，触发大屏切换信号 signalScreenID(int screenID)
void ScreenSwitchWidget::slotButtonClicked()
{
    QPushButton* pSender = qobject_cast<QPushButton*>(sender());
    if (pSender)
    {
        if (_oldButton != pSender)
        {
            if (_oldButton)
            {
                _oldButton->setChecked(false);
            }
            _oldButton = pSender;
            _oldButton->setChecked(true);
            emit signalScreenID(_mapButton2ID[pSender]);
            QSettings setting("lastScreen.ini", QSettings::IniFormat);
            setting.setValue("LastScreenID", _mapButton2ID[pSender]);
        }
        if (!_oldButton->isChecked())
        {
            _oldButton->setChecked(true);
        }
    }
}

void ScreenSwitchWidget::slotForward()
{
    QScrollBar *pScrollBar;
    switch (g_ScreenSwitchWidgetConfigData.direction)
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

void ScreenSwitchWidget::slotBackward()
{
    QScrollBar *pScrollBar;
    switch (g_ScreenSwitchWidgetConfigData.direction)
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

void ScreenSwitchWidget::slotRangeChanged(int min, int max)
{
    resetValue();
    setShowButton();
}

void ScreenSwitchWidget::setShowButton()
{
    if (_maxValue == 0)
    {
        ui->left_toolButton->setEnabled(false);
        ui->right_toolButton->setEnabled(false);
    }
    else if (_currentValue == 0)
    {
        ui->left_toolButton->setEnabled(false);
        ui->right_toolButton->setEnabled(true);
    }
    else if (_currentValue == _maxValue)
    {
        ui->left_toolButton->setEnabled(true);
        ui->right_toolButton->setEnabled(false);
    }
    else
    {
        ui->left_toolButton->setEnabled(true);
        ui->right_toolButton->setEnabled(true);
    }
}

void ScreenSwitchWidget::resetValue()
{
    QScrollBar *pScrollBar;
    switch (g_ScreenSwitchWidgetConfigData.direction)
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
