#include "LayoutModelWidget.h"
#include <QListWidgetItem>
#include <QDebug>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <vector>
#include <QVariant>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QStyleOption>
#include "ots/Logger.h"
#include "otsXML/globalVar.h"
#include "OperationLayoutWidget.h"
#include <QFile>
#include "ui_LayoutModelWidget.h"
#include "LayoutButton.h"

/******************************** LayoutModelWidget_ **************************************/
LayoutModelWidget::LayoutModelWidget(QWidget *parent)
:QWidget(parent), ui(new Ui::LayoutModelWidget), _oldButtonID(-1), _pSpacerItem(0)
{
    ui->setupUi(this);
    QBoxLayout* pLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout* pLayout_1 = new QBoxLayout(QBoxLayout::LeftToRight);
    /* 设置方向 */
    switch (g_LayoutModelWidgetConfigData.direction)
    {
    case 0:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pLayout_1->setDirection(QBoxLayout::LeftToRight);
        break;
    case 1:
        pLayout->setDirection(QBoxLayout::TopToBottom);
        pLayout_1->setDirection(QBoxLayout::TopToBottom);
        break;
    case 2:
        pLayout->setDirection(QBoxLayout::RightToLeft);
        pLayout_1->setDirection(QBoxLayout::RightToLeft);
        break;
    case 3:
        pLayout->setDirection(QBoxLayout::BottomToTop);
        pLayout_1->setDirection(QBoxLayout::BottomToTop);
        break;
    default:
        pLayout->setDirection(QBoxLayout::LeftToRight);
        pLayout_1->setDirection(QBoxLayout::LeftToRight);
        break;
    }
    ui->scrollAreaWidgetContents->setLayout(pLayout_1);
    pLayout->addWidget(ui->label);
    pLayout->addWidget(ui->scrollArea);
    pLayout->addWidget(ui->pull_pushButton);
    pLayout->addWidget(ui->set_pushButton);
    pLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    //ui->pull_pushButton->setText("More");
	ui->pull_pushButton->setObjectName("More");

    ui->label->setText(tr("Screen Layouts"));

    _pCommonLayoutDlg = new SetCommonLayoutDialog(this);
    _pCommonLayoutDlg->hide();

    QObject::connect(ui->pull_pushButton, SIGNAL(clicked()), this, SLOT(createAllModelListData()));
    /*点击设置会弹框*/
    QObject::connect(ui->set_pushButton, SIGNAL(clicked()), _pCommonLayoutDlg, SLOT(createSrcList()));
    /*点击设置弹框确认按钮*/
    QObject::connect(_pCommonLayoutDlg, SIGNAL(saveModuleSignal()), this, SLOT(createCommonModelListData()));
    createCommonModelListData();

    this->setLayout(pLayout);
}

LayoutModelWidget::~LayoutModelWidget()
{
    delete ui;
}

void LayoutModelWidget::changeEvent(QEvent *event)
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

void LayoutModelWidget::slotChangeItem()
{
    if (_oldButtonID != -1 && _oldButtonID!=1)
    {
        _mapID2Button[_oldButtonID]->setChecked(false);
    }
	_oldButtonID = _mapID2Button.firstKey();
	_mapID2Button[_oldButtonID]->setChecked(true);
	_mapID2Button[_oldButtonID]->click();
    emit signalScreenLayoutID(_oldButtonID);
}

void LayoutModelWidget::createAllModelListData()
{
    QPushButton *pSender = qobject_cast<QPushButton *> (sender());
    if (!pSender)
    {
        return;
    }

    if (pSender->objectName() == QStringLiteral("More"))
    {
        pSender->setObjectName(QStringLiteral("Common"));
        QList<LayoutButton*> objectLists = ui->scrollAreaWidgetContents->findChildren<LayoutButton*>();
        qDeleteAll(objectLists);
        if (_pSpacerItem)
        {
            ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
            _pSpacerItem = 0;
        }

        _mapID2Button.clear();
        _oldButtonID = -1;
        std::vector<Model_Type> vecModelType;
        vecModelType.push_back(Model_1);
        vecModelType.push_back(Model_2H);
        vecModelType.push_back(Model_2V);
        vecModelType.push_back(Model_3);
        vecModelType.push_back(Model_4);
        vecModelType.push_back(Model_6);
        vecModelType.push_back(Model_6L);
        vecModelType.push_back(Model_6L2);
        vecModelType.push_back(Model_9);
        vecModelType.push_back(Model_13);
        vecModelType.push_back(Model_16);

        //倒序访问……
        for (int i = 0; i < vecModelType.size(); i++)
        {
            LayoutButton* button = new LayoutButton((LayoutButton::LayoutType)vecModelType[i], this);
            button->setCheckable(true);
            _mapID2Button[vecModelType[i]] = button;
            QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
            ui->scrollAreaWidgetContents->layout()->addWidget(button);
        }
        _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
    }
    else if (pSender->objectName() == QStringLiteral("Common"))
    {
        pSender->setObjectName(QStringLiteral("More"));
        createCommonModelListData();
    }
}

void LayoutModelWidget::createCommonModelListData()
{
    //界面排布
    std::vector<Model_Type> vecModelType;
    QFile file("module.ini");
    bool fileActive = file.exists();
    if (fileActive)
    {
        qInfo() << "file moudule.ini exist: " << fileActive;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        while (!in.atEnd())
        {
            int  moduleType = in.readLine().toInt();
            vecModelType.push_back((Model_Type)moduleType);
        }
    }
    else
    {
        vecModelType.push_back(Model_1);
        vecModelType.push_back(Model_2H);
        vecModelType.push_back(Model_2V);
        vecModelType.push_back(Model_3);
        qInfo() << "File does't exist";
    }
    QList<LayoutButton*> objectLists = ui->scrollAreaWidgetContents->findChildren<LayoutButton*>();
    qDeleteAll(objectLists);
    if (_pSpacerItem)
    {
        ui->scrollAreaWidgetContents->layout()->removeItem(_pSpacerItem);
        _pSpacerItem = 0;
    }
    _mapID2Button.clear();
    _oldButtonID = -1;

    //倒序访问
    for (int i = 0; i < vecModelType.size(); i++)
    {
        LayoutButton* button = new LayoutButton((LayoutButton::LayoutType)vecModelType[i], this);
        button->setCheckable(true);
        _mapID2Button[vecModelType[i]] = button;
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
        ui->scrollAreaWidgetContents->layout()->addWidget(button);
    }
    _pSpacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->scrollAreaWidgetContents->layout()->addItem(_pSpacerItem);
}

void LayoutModelWidget::slotChangedItem(int type)
{
    if (_oldButtonID != -1 && _oldButtonID != type)
    {
        _mapID2Button[_oldButtonID]->setChecked(false);
    }
    if (_mapID2Button.contains(type))
    {
        _oldButtonID = type;
        _mapID2Button[_oldButtonID]->setChecked(true);
        _mapID2Button[_oldButtonID]->click();
    }
	else
	{
		_oldButtonID=-1;
	}
}

void LayoutModelWidget::slotButtonClicked()
{
    LayoutButton* pSender = qobject_cast<LayoutButton*>(sender());
    if (pSender)
    {
        int senderID = pSender->getLayoutButtonType();
        LayoutButton* oldButton = 0;
        if (_oldButtonID != -1)
        {
            oldButton = _mapID2Button[_oldButtonID];
        }
        if (_oldButtonID != senderID)
        {
            if (oldButton)
            {
                oldButton->setChecked(false);
            }
            _oldButtonID = senderID;
            oldButton = pSender;
            pSender->setChecked(true);
            emit signalScreenLayoutID(_oldButtonID);
        }
        if (oldButton&&!oldButton->isChecked())
        {
            oldButton->setChecked(true);
        }
    }
}
