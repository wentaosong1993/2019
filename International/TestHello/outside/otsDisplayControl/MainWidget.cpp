#include "MainWidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QGridLayout>
#include "otsSignalPreview/signalpreviewwidget.h"
#include "otsSignalTree/SignalGroupTreeWidget.h"
#include "otsScreenSwitch/screenswitchwidget.h"
#include "otsScreenWidget/screenwidget.h"
#include "otsLayoutModel/LayoutModelWidget.h"
#include "otsScreenModel/ScreenModelWidget.h"
#include "OperationWidget.h"
#include "ots/Logger.h"
#include <QMessageBox>
#include "otsServer/DataSession.h"
#include <QRect>
#include <QRectF> 
#include "otsScreenWidget/screenlayoutwidget.h"
#include <QPushButton>
#include "otsPollWidget/PollWidget.h"
#include "otsPollWidget/MirrorWidget.h"
#include <QDebug>
#include "otsControler/ControlerData.h"
#include "otsXML/globalVar.h"
#include <QFileDialog>
#include <QLabel>
#include <QDateTime>
#include <QFile>
#include "otsLayoutModel/LayoutButton.h"
#include <QProcess>
#include <QCoreApplication>
#include "otsCommon/MessageBox.h"
#include "otsControler/mould_op.h"
#include "otsControler/net_globle_data.h"
#include <QTranslator>

using namespace otsCommon;

int _operationFrameIndex = 0;
using namespace otsDisplayControl;

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    _vecPollDatas.clear();
    _mapPollID2PollData.clear();
    _mapTimer2PollData.clear();
    _pUploadBT = 0;
    createUI();
    //this->startTimer(100000);
}

MainWidget::~MainWidget()
{
}
#define OLD_CALCULATE_

int MainWidget::getNumforT(int num, int T)
{
    int result = num;
    int temp = num % T;
    if (temp != 0)
    {
#ifdef OLD_CALCULATE
        result -= temp;
#else
        qreal median = T / 2.0;
        if (temp >= median)
        {
            result += (T - temp);
        }
        else
        {
            result -= temp;
        }
#endif
    }
    return result;
}

void MainWidget::createUI()
{
    /** 去标题栏 */
    this->setWindowFlags(Qt::FramelessWindowHint);

    /** 创建添加预案弹框 done!!! */
    _pAddModelWidget = new AddModelWidget(this);
    _pAddModelWidget->hide();

    /* 创建添加轮巡预案弹框 done!!! */
    _pAddPollWidget = new AddPollWidget(this);
    _pAddPollWidget->hide();

	/* 创建添加镜像通道弹框 done !!! */
	_pAddMirrorWidget = new AddMirrorWidget(this);
	_pAddMirrorWidget->hide();

    /** 创建中心背景窗口 */
    QWidget * pCenterWidget = new QWidget(this);
    pCenterWidget->setObjectName("CenterWidget");
    pCenterWidget->setGeometry(426,76,1068,990);

    QGridLayout* pGriLayout = new QGridLayout;
    pGriLayout->setMargin(0);
    pGriLayout->setSpacing(0);

    /** 创建Logo显示 */
    _pLogoLabel = new QLabel(this);
    _pLogoLabel->setObjectName("LogoLabel");
    _pLogoLabel->setVisible(g_LogoLabel.visible);
    pGriLayout->addWidget(_pLogoLabel,
        g_LogoLabel.yPos, g_LogoLabel.xPos,
        g_LogoLabel.height, g_LogoLabel.width);

    /** 创建时间 */
    _pTimeLabel = new QLabel(this);
    _pTimeLabel->setObjectName("TimeLabel");
    _pTimeLabel->setVisible(g_TimeLabel.visible);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm");//yyyy年MM月dd日 ddd HH:mm
    _pTimeLabel->setText(current_date);
    pGriLayout->addWidget(_pTimeLabel,
        g_TimeLabel.yPos, g_TimeLabel.xPos,
        g_TimeLabel.height, g_TimeLabel.width);

    /** 创建最小化按钮 */
    _pMiniBT = new QPushButton(this);
    _pMiniBT->setObjectName("MiniButton");
    _pMiniBT->setVisible(g_MiniButton.visible);
    pGriLayout->addWidget(_pMiniBT,
        g_MiniButton.yPos, g_MiniButton.xPos,
        g_MiniButton.height, g_MiniButton.width);
    QObject::connect(_pMiniBT, SIGNAL(clicked()), this, SLOT(showMinimized()));

    /** 创建关闭按钮 */
    _pCloseBT = new QPushButton(this);
    _pCloseBT->setObjectName("CloseButton");
    _pCloseBT->setVisible(g_CloseButton.visible);
    pGriLayout->addWidget(_pCloseBT,
        g_CloseButton.yPos, g_CloseButton.xPos,
        g_CloseButton.height, g_CloseButton.width);
	QObject::connect(_pCloseBT, SIGNAL(clicked()), this, SLOT(close()));
	//QObject::connect(this, SIGNAL(lastWindowClosed()), this, SLOT(slotLoggout()));

    /** 创建信号源树 done!!!*/
    _pSignalGroupTreeWidget = new SignalGroupTreeWidget(this);
    _pSignalGroupTreeWidget->setVisible(g_SignalGroupTreeWidgetConfigData.visible);
    pGriLayout->addWidget(_pSignalGroupTreeWidget,
        g_SignalGroupTreeWidgetConfigData.yPos, g_SignalGroupTreeWidgetConfigData.xPos,
        g_SignalGroupTreeWidgetConfigData.height, g_SignalGroupTreeWidgetConfigData.width);

    /** 创建信号源预览窗口 done!!! */
    _pSignalPreviewWidget = new SignalPreviewWidget(this);
    _pSignalPreviewWidget->setVisible(g_SignalPreviewWidgetConfigData.visible);
    pGriLayout->addWidget(_pSignalPreviewWidget, 
        g_SignalPreviewWidgetConfigData.yPos, g_SignalPreviewWidgetConfigData.xPos,
        g_SignalPreviewWidgetConfigData.height, g_SignalPreviewWidgetConfigData.width);

    /* 点击左侧树形信号源、信号源组, 初始化信号源预览窗口 */
    QObject::connect(_pSignalGroupTreeWidget, SIGNAL(signalItemClicked(int, int, QString)),
        _pSignalPreviewWidget, SLOT(slotCreatePreviewWidgetFromGroupID(int, int, QString)));

	/** 创建大屏选择菜单 done！！！ */ 
    _pScreenSwitchWidget = new ScreenSwitchWidget(this);
    _pScreenSwitchWidget->setVisible(g_ScreenSwitchWidgetConfigData.visible);
    //_pScreenSwitchWidget->createData();
    pGriLayout->addWidget(_pScreenSwitchWidget,
        g_ScreenSwitchWidgetConfigData.yPos, g_ScreenSwitchWidgetConfigData.xPos,
        g_ScreenSwitchWidgetConfigData.height, g_ScreenSwitchWidgetConfigData.width);

    /** 创建大屏操作界面 */
    _pScreenWidget = new ScreenWidget(this, false);
    _pScreenWidget->setVisible(g_ScreenWidgetConfigData.visible);
    _pScreenWidget->setScreenRowCol(1, 1);
    _pScreenWidget->setScreenModeRowCol(2, 2);
    pGriLayout->addWidget(_pScreenWidget,
        g_ScreenWidgetConfigData.yPos, g_ScreenWidgetConfigData.xPos,
        g_ScreenWidgetConfigData.height, g_ScreenWidgetConfigData.width);
	/* 点击大屏菜单，切换顶部大屏界面分割排布预览窗口*/
    QObject::connect(_pScreenSwitchWidget, SIGNAL(signalScreenID(int)),
        _pScreenWidget, SLOT(slotScreenChanged(int)));

    /* 创建界面排布菜单 done!!! */
    _pLayoutModelWidget = new LayoutModelWidget(this);
    _pLayoutModelWidget->setVisible(g_LayoutModelWidgetConfigData.visible);
    pGriLayout->addWidget(_pLayoutModelWidget,
        g_LayoutModelWidgetConfigData.yPos, g_LayoutModelWidgetConfigData.xPos,
        g_LayoutModelWidgetConfigData.height, g_LayoutModelWidgetConfigData.width);
    /* 点击界面排布，更新大屏预览窗口排布方式 */
    QObject::connect(_pLayoutModelWidget, SIGNAL(signalScreenLayoutID(int)),
        _pScreenWidget, SLOT(slotLayoutModelChanged(int)));
	//QObject::connect(_pLayoutModelWidget, SIGNAL(signalScreenModelID(int)),
	//	_pScreenWidget, SIGNAL(signalSaveFrameToDB()));

    /* 大屏发生变化信号/槽 */
    /* QObject::connect(_pScreenSwitchWidget, SIGNAL(signalScreenID(int)),
         _pLayoutModelWidget, SLOT(slotChangeItem()));*/

    /* 创建大屏预案界面 done！！！*/
    _pScreenModelWidget = new ScreenModelWidget(this);
    _pScreenModelWidget->setVisible(g_ScreenModelWidgetConfigData.visible);
    pGriLayout->addWidget(_pScreenModelWidget,
        g_ScreenModelWidgetConfigData.yPos, g_ScreenModelWidgetConfigData.xPos,
        g_ScreenModelWidgetConfigData.height, g_ScreenModelWidgetConfigData.width);
    /* 大屏切换时，更新预案列表 */
    QObject::connect(_pScreenSwitchWidget, SIGNAL(signalScreenID(int)),
        _pScreenModelWidget, SLOT(slotScreenChanged(int)));
    /* 大屏预案发生变化信号/槽 */
    /*QObject::connect(_pScreenModelWidget, SIGNAL(signalScreenModelID(int)),
        _pScreenWidget, SLOT(slotScreenModelChanged(int)));*/
    /* 大屏预案发生添加信号/槽 */
    QObject::connect(_pScreenModelWidget, SIGNAL(signalSaveModel()),
        this, SLOT(slotSaveModel()));
    QObject::connect(_pScreenModelWidget, SIGNAL(signalDelModel(int)),
        this, SLOT(slotDelModel(int)));
    QObject::connect(_pScreenModelWidget, SIGNAL(signalScreenModelID(int)),
        this, SLOT(slotModelChanged(int)));

    /* 创建应用排布 */
    QPushButton* pApplyScreenBT = new QPushButton(this);
    pApplyScreenBT->setCheckable(true);
    pApplyScreenBT->setVisible(g_ApplyButtonConfigData.visible);
    pGriLayout->addWidget(pApplyScreenBT,
        g_ApplyButtonConfigData.yPos, g_ApplyButtonConfigData.xPos,
        g_ApplyButtonConfigData.height, g_ApplyButtonConfigData.width);
    //pApplyScreenBT->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    //pApplyScreenBT->setCheckable(true);
	qInfo() << "state:" << g_ApplyButtonConfigData.visible;
    pApplyScreenBT->setText("Apply111");
    //button->setObjectName("ApplyScreenModelButon");
    ///* 大屏预案应用添加信号/槽 */
	//QObject::connect(_pScreenModelWidget, SIGNAL(signalApplyScreenModule()), _pScreenWidget, SLOT(slotApplyScreen()));
    QObject::connect(pApplyScreenBT, SIGNAL(clicked()), _pScreenWidget, SLOT(slotApplyScreen()));

    /* 创建轮巡通道 done!!!*/
    _pPollWidget = new PollWidget(this);
    _pPollWidget->setVisible(g_PollWidgetConfigData.visible);
    pGriLayout->addWidget(_pPollWidget,
        g_PollWidgetConfigData.yPos, g_PollWidgetConfigData.xPos,
        g_PollWidgetConfigData.height, g_PollWidgetConfigData.width);
    /* 启动/关闭轮巡信号/槽 */
    QObject::connect(_pPollWidget, SIGNAL(signalPollGroupID(int, bool)), this, SLOT(slotPollStartStop(int, bool)));
    /* 添加轮巡方案信号/槽 */
    QObject::connect(_pPollWidget, SIGNAL(signalAddPoll()), this, SLOT(slotAddPoll()));
	/* 删除轮巡方案信号/槽 */
	QObject::connect(_pPollWidget, SIGNAL(signalDelPoll()), this, SLOT(slotDelPoll()));
    /* 添加大屏发生变化时轮巡方案更新信号/槽 */
    QObject::connect(_pScreenSwitchWidget, SIGNAL(signalScreenID(int)), _pPollWidget, SLOT(slotScreenChanged(int)));

	/* 创建镜像通道 done!!!*/
	_pMirrorWidget = new MirrorWidget(this);
    _pMirrorWidget->setVisible(g_MirrorWidgetConfigData.visible);
	pGriLayout->addWidget(_pMirrorWidget,
		g_MirrorWidgetConfigData.yPos, g_MirrorWidgetConfigData.xPos,
        g_MirrorWidgetConfigData.height, g_MirrorWidgetConfigData.width);
	QObject::connect(_pMirrorWidget, SIGNAL(signalAddMirror()), this, SLOT(slotAddMirror()));
	QObject::connect(_pMirrorWidget, SIGNAL(signalDelMirror()), this, SLOT(slotDelMirror()));
	QObject::connect(_pMirrorWidget, SIGNAL(signalApplyMirror(bool, int)), this, SLOT(slotApplyMirror(bool,int)));
	QObject::connect(this, SIGNAL(saveFrameIndex()), _pScreenWidget, SLOT(slotSaveFrameToDB()));

	/* 添加大屏发生变化时镜像方案更新信号/槽 */
	QObject::connect(_pScreenSwitchWidget, SIGNAL(signalScreenID(int)), _pMirrorWidget, SLOT(slotScreenChanged(int)));

    /* 创建清屏按钮 */
    _pClearBT = new QPushButton(this);
	_pClearBT->setObjectName("clearScreen");
    _pClearBT->setVisible(g_ClearScreen.visible);
    _pClearBT->setText(tr("Clear Screen"));
    pGriLayout->addWidget(_pClearBT,
        g_ClearScreen.yPos, g_ClearScreen.xPos,
        g_ClearScreen.height, g_ClearScreen.width);
    QObject::connect(_pClearBT, SIGNAL(clicked()),
        _pScreenWidget, SLOT(slotClearScreen()));

    /* 创建撤销按钮 */
    _pUndoBT = new QPushButton(this);
    _pUndoBT->setObjectName("Undo");
    _pUndoBT->setVisible(g_Undo.visible);
	pGriLayout->addWidget(_pUndoBT,
        g_Undo.yPos, g_Undo.xPos,
        g_Undo.height, g_Undo.width);
	QObject::connect(_pUndoBT, SIGNAL(clicked()),
        this, SLOT(slotUndo()));

    /* 创建重做按钮 */
    _pRedoBT = new QPushButton(this);
    _pRedoBT->setObjectName("Redo");
    _pRedoBT->setVisible(g_Redo.visible);
	pGriLayout->addWidget(_pRedoBT,
        g_Redo.yPos, g_Redo.xPos,
        g_Redo.height, g_Redo.width);
	QObject::connect(_pRedoBT, SIGNAL(clicked()),
        this, SLOT(slotRedo()));
    
    /** 创建上传图片功能 */
    _pUploadBT = new QPushButton(this);
    _pUploadBT->setObjectName("UpLoad");
    _pUploadBT->setVisible(g_UpLoad.visible);
    pGriLayout->addWidget(_pUploadBT,
        g_UpLoad.yPos, g_UpLoad.xPos,
        g_UpLoad.height, g_UpLoad.width);
    QObject::connect(_pUploadBT, SIGNAL(clicked()),
        this, SLOT(slotUpLoadImage()));

    /** 信号跟槽链接 */
    /* 1>大屏预案变化发出排布方式变化信号供排布方式模块界面选中变化 */
    QObject::connect(_pScreenWidget, SIGNAL(signalCurrentScreenModelType(int)),
        _pLayoutModelWidget, SLOT(slotChangedItem(int)));

    /** 5列布局 */
    for (int i = 0; i < g_MainWidgetCol; i++)
    {
        pGriLayout->setColumnStretch(i, 1);
    }
    /** 20行布局 */
    for (int i = 0; i < g_MainWidgetRow; i++)
    {
        pGriLayout->setRowStretch(i, 1);
    }

    this->setLayout(pGriLayout);
}

void MainWidget::slotSaveModel()
{
#ifdef OLD
    OTS_LOG->debug("input save model...");
    /* 取消选中，使得按钮可以再次点击 */
    _pScreenModelWidget->setCurrentItem(NULL);
    /* 如果大屏没有LayoutItem 则提示无有效数据，无法保存 */
    QList<ScreenLayoutItem*> listItem;
    listItem = _pScreenWidget->getScreenLayoutItems();
    if (listItem.size() == 0)
    {
        qInfo() << QStringLiteral("当前大屏不存在排布信息，无法保存预案");
        SCMessageBox* activeModuleDlg = new SCMessageBox(QStringLiteral("新增预案失败"), QStringLiteral("请确认当前大屏是否存在预案！"), otsCommon::SCMessageBox::Information, this);
        activeModuleDlg->exec();
        activeModuleDlg->deleteLater();
        activeModuleDlg = nullptr;
        return;
    }

    int resoult = _pAddModelWidget->exec();
    switch (resoult)
    {
    case QDialog::Accepted:
    {
        /* save DB */
        QString modelName = _pAddModelWidget->getModelName();
        int screenID = _pScreenWidget->getScreenID();
        int layoutType = _pScreenWidget->getScreenLayouType();

        int modelID = 0;
        std::tuple<int, std::string> mouldInfo(layoutType, modelName.toStdString());
        std::vector<std::tuple<std::string, int, int, int, int, int>> layoutInfo;
        for (int i = 0; i < listItem.size(); i++)
        {
            layoutInfo.push_back(std::make_tuple("test", listItem[i]->getRealityRect().left(), listItem[i]->getRealityRect().top(),
                listItem[i]->getRealityRect().right(), listItem[i]->getRealityRect().bottom(), listItem[i]->getEncoderID()));
        }
        int retCode = otsServer::DataSession::instance()->saveMouldData2DB(screenID, modelID, mouldInfo, layoutInfo);
        if (1 == retCode)
        {
            if (modelID != 0)
            {
                _pScreenModelWidget->slotAddModelItem(modelID, modelName);
            }
            qInfo() << "add Module success";
        }
        else if (-99 == retCode)
        {
            //重复
            qInfo() << "add Module data repeated!!!";
            SCMessageBox* addModuleRepeatDlg = new SCMessageBox(QStringLiteral("新增预案重复"), QStringLiteral("请确认预案名称是否重复！"), otsCommon::SCMessageBox::Information, this);
            addModuleRepeatDlg->exec();
            addModuleRepeatDlg->deleteLater();
            addModuleRepeatDlg = nullptr;
        }
        else
        {
            qInfo() << "add Module data failed!!!";
            //失败
            SCMessageBox* addModuleFailDlg = new SCMessageBox(QStringLiteral("新增预案失败"), QStringLiteral("请重新添加大屏预案!"), otsCommon::SCMessageBox::Information, this);
            addModuleFailDlg->exec();
            addModuleFailDlg->deleteLater();
            addModuleFailDlg = nullptr;
        }
        _pAddModelWidget->clearContent();
    }
        break;
    case QDialog::Rejected:
    {
        _pAddModelWidget->clearContent();
    }
        break;
    default:
        break;
    }
    OTS_LOG->debug("end save model...");
#else
    OTS_LOG->debug("input save model...");
    /* 取消选中，使得按钮可以再次点击 */
    _pScreenModelWidget->setCurrentItem(NULL);
    /* 如果大屏没有LayoutItem 则提示无有效数据，无法保存 */
    QList<ScreenLayoutItem*> listItem;
    listItem = _pScreenWidget->getScreenLayoutItems();
    if (listItem.size() == 0)
    {
        //qInfo() << tr("当前大屏不存在排布信息，无法保存预案");
		//qInfo() << tr("The current screen does not have the layout information, and the module can't be saved.");
        //SCMessageBox* activeModuleDlg = new SCMessageBox(QStringLiteral("新增预案失败"), QStringLiteral("请确认当前大屏是否存在预案！"), otsCommon::SCMessageBox::Information, this);
		SCMessageBox* activeModuleDlg = new SCMessageBox(tr("Add Screen Module Failed Hint"), tr("Please confirm that there is a module for the current screen!"), otsCommon::SCMessageBox::Information, this);
        activeModuleDlg->exec();
        activeModuleDlg->deleteLater();
        activeModuleDlg = nullptr;
        return;
    }

    int resoult = _pAddModelWidget->exec();
    switch (resoult)
    {
    case QDialog::Accepted:
    {
        /* save DB */
        QString modelName = _pAddModelWidget->getModelName();
        int screenID = _pScreenWidget->getScreenID();
        int layoutType = _pScreenWidget->getScreenLayouType();

        int modelID = 0;
        std::tuple<int, std::string> mouldInfo(layoutType, modelName.toStdString());
        std::vector<std::tuple<std::string, int, int, int, int, int>> layoutInfo;
        for (int i = 0; i < listItem.size(); i++)
        {
            layoutInfo.push_back(std::make_tuple("test", listItem[i]->getRealityRectF().left() + 0.5, listItem[i]->getRealityRectF().top() + 0.5,
                listItem[i]->getRealityRectF().right() + 0.5, listItem[i]->getRealityRectF().bottom() + 0.5, listItem[i]->getEncoderID()));
        }
        /** 
        * Date: 2019-11-7
        * Author: John_Yang
        */
        if (g_masterID == g_self_ID)
        {
            send_save_mould_by_master(g_self_ID, screenID, mouldInfo, layoutInfo, std::vector<int>(), -1);
            _pScreenModelWidget->slotScreenChanged(screenID);
        }
        else
        {
            send_save_mould_by_slave(g_self_ID, screenID, mouldInfo, layoutInfo, std::vector<int>(), -1);
        }

        /*int retCode = otsServer::DataSession::instance()->saveMouldData2DB(screenID, modelID, mouldInfo, layoutInfo);
        if (1 == retCode)
        {
            if (modelID != 0)
            {
                _pScreenModelWidget->slotAddModelItem(modelID, modelName);
            }
            qInfo() << "add Module success";
        }
        else if (-99 == retCode)
        {
            //重复
            qInfo() << "add Module data repeated!!!";
            SCMessageBox* addModuleRepeatDlg = new SCMessageBox(QStringLiteral("新增预案重复"), QStringLiteral("请确认预案名称是否重复！"), otsCommon::SCMessageBox::Information, this);
            addModuleRepeatDlg->exec();
            addModuleRepeatDlg->deleteLater();
            addModuleRepeatDlg = nullptr;
        }
        else
        {
            qInfo() << "add Module data failed!!!";
            //失败
            SCMessageBox* addModuleFailDlg = new SCMessageBox(QStringLiteral("新增预案失败"), QStringLiteral("请重新添加大屏预案!"), otsCommon::SCMessageBox::Information, this);
            addModuleFailDlg->exec();
            addModuleFailDlg->deleteLater();
            addModuleFailDlg = nullptr;
        }*/
        _pAddModelWidget->clearContent();
    }
        break;
    case QDialog::Rejected:
    {
        _pAddModelWidget->clearContent();
    }
        break;
    default:
        break;
    }
    OTS_LOG->debug("end save model...");
#endif
}

void MainWidget::slotPollStartStop(int groupID, bool start)
{
    OTS_LOG->debug("start or stop :{},groupID:{}", start, groupID);
    if (start)
    {
        int currentScreenID = _pScreenModelWidget->getCurrentScreenID();
        std::vector<std::tuple<int, int, int>> allPoll;
        PollData pollData;
        pollData._currnetIndex = 0;
        otsServer::DataSession::instance()->getPollMouldsByGroupID(groupID, allPoll);
        for (int i = 0; i < allPoll.size(); i++)
        {
            pollData._vecModels.push_back(std::get<0>(allPoll[i]));
            if (pollData._time == 0)
            {
                pollData._time = std::get<2>(allPoll[i]);
            }
        }
        if (pollData._vecModels.size() == 0)
        {
            return;
        }
        pollData._screenID = currentScreenID;
        pollData._timer = new QTimer(this);
        pollData._timer->start(pollData._time * 1000);
        QObject::connect(pollData._timer, SIGNAL(timeout()), this, SLOT(slotTimerOut()));
        _mapPollID2PollData[groupID] = pollData;
        _mapTimer2PollData[pollData._timer] = pollData;
		processPollData(_mapTimer2PollData[pollData._timer]);
    }
    else
    {
        QTimer* pTimer = _mapPollID2PollData.find(groupID)->_timer;
        pTimer->stop();
        QObject::disconnect(pTimer, SIGNAL(timeout()), this, SLOT(slotTimerOut()));
        pTimer->deleteLater();
        _mapPollID2PollData.remove(groupID);
        _mapTimer2PollData.remove(pTimer);
    }
}

void MainWidget::slotAddPoll()
{
	if (!_pAddPollWidget)
    {
        _pAddPollWidget = new AddPollWidget(this);
    }
    int currentScreenID = _pScreenModelWidget->getCurrentScreenID();
    _pAddPollWidget->creatSrcList(currentScreenID);
    int resoult = _pAddPollWidget->exec();
    switch (resoult)
    {
    case QDialog::Accepted:
    {
        /* save DB */
        QString strPollName = _pAddPollWidget->getPollName();
        int time = _pAddPollWidget->getTimes();
        std::vector<int> models = _pAddPollWidget->getPollModels();
        int pollID = 0;
        int retCode = otsServer::DataSession::instance()->addPoll(pollID, currentScreenID, strPollName.toStdString(), models, time);
		if (1 == retCode)
		{
			if (pollID != 0)
			{
				_pPollWidget->addItem(pollID, strPollName);
			}
			qInfo() << "add Poll success";
		}
		else if (-99 == retCode)
		{
			//重复
			qInfo() << QStringLiteral("新增轮巡通道重复");
			SCMessageBox* addPollRepeatDlg = new SCMessageBox(tr("Add Poll Channel Repeated Hint"), tr("Please confirm whether the name of the Poll Channel is repeated!"), otsCommon::SCMessageBox::Information, this);
			addPollRepeatDlg->exec();
			addPollRepeatDlg->deleteLater();
			addPollRepeatDlg = nullptr;
		}
		else
		{
			//失败
			qInfo() << QStringLiteral("新增轮巡通道失败");
			SCMessageBox* addPollFailDlg = new SCMessageBox(tr("Add Poll Channel Failed Hint"), tr("Sorry,Please add the Poll channel again!"), otsCommon::SCMessageBox::Information, this);
			addPollFailDlg->exec();
			addPollFailDlg->deleteLater();
			addPollFailDlg = nullptr;
		}
    }
        break;
    case QDialog::Rejected:
    {
    }
        break;
    default:
        break;
    }
}

void MainWidget::slotDelPoll()
{
	//轮训预案ID
	int pollGroupId = _pPollWidget->getSelectedPollId();
	if (pollGroupId > 0)
	{
		qInfo() << QStringLiteral("删除轮巡通道");
		SCMessageBox* myBox0 = new SCMessageBox(tr("Delete Poll Channel Hint"), tr("Are you sure to delete current Poll Channel?"), otsCommon::SCMessageBox::Warning, this);
		myBox0->exec();

		if (myBox0->result() == QDialog::Accepted)
		{
			int retCode = otsServer::DataSession::instance()->delPoll(pollGroupId);
			if (retCode)
			{
				_pPollWidget->delSelectedItem();
				OTS_LOG->debug("del poll mould success!!!, groupID:{}", pollGroupId);
			}
			else
			{
				OTS_LOG->error("del poll mould fail!!!, groupID:{}", pollGroupId);
			}
		}
		myBox0->deleteLater();
		myBox0 = nullptr;
	}
}

//extern int getNumforT(int num, int T);

void MainWidget::slotTimerOut()
{
    QTimer* pSender = qobject_cast<QTimer*>(sender());
    if (!pSender)
    {
        return;
    }

    if (_mapTimer2PollData.find(pSender) != _mapTimer2PollData.end())
    {
        PollData& pollData = *(_mapTimer2PollData.find(pSender));

        processPollData(pollData);
  //      int nextModel = pollData._vecModels[pollData._currnetIndex];
  //      /* 找到大屏信息 (行、列、解码器ID以及解码板所在行列)*/
  //      std::tuple<int, int, int> rowColPro; //大屏属性：行、列
  //      std::vector<std::tuple<int, int, int> > vecDecoderID;
  //      std::map<int, int> mapIndex2Decoder;
  //      otsServer::DataSession::instance()->getScreenDecoderInfo(pollData._screenID, rowColPro, vecDecoderID);
  //      /* 通过model找到排布数据(编码器ID、布局名称、堆叠顺序索引、布局Rect) */
  //      std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;

  //      /** 如果轮巡中的预案已被删除是否跳过，还是说在删除预案的时候旧主动将轮巡中的预案删除 */
  //      otsServer::DataSession::instance()->getLayoutInfo(nextModel, vecTupleLayout);


  //      /** 如果大屏ID是当前显示操作的大屏则进行显示操作，否则进行下面的操作 */
  //      if (pollData._screenID == _pScreenWidget->getScreenID())
  //      {
  //          pollData._currnetIndex++;
  //          pollData._currnetIndex %= pollData._vecModels.size();//循环
  //          _pScreenWidget->slotScreenModelChanged(nextModel);
  //          return;
  //      }

  //      std::vector<ots::PairDecodeToIRect> vecDecodeToIRect; //<解码器ID、目的矩形、编码器ID、源矩形>vector
  //      int _row = std::get<0>(rowColPro);
  //      int _col = std::get<1>(rowColPro);
  //      mapIndex2Decoder.clear();
  //
		////解析vecDecoderID,根据解码板所在行列数，计算后将位置Index与decoderID对应起来
  //      for (std::size_t i = 0; i < vecDecoderID.size(); ++i)
  //      {
  //          int decoderId = std::get<2>(vecDecoderID[i]);
  //          int num = std::get<0>(vecDecoderID[i]) * _col +std::get<1>(vecDecoderID[i]);
  //          mapIndex2Decoder[num] = decoderId;
  //      }

  //      QVector<QRect> temp_vectItem;
  //      for (int i = 0; i < _row; ++i)
  //      {
  //          for (int j = 0; j < _col; ++j)
  //          {
  //              ots::PairDecodeToIRect pairInfo;
  //              int num = i * _col +j;
  //              pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID
  //              int layout = 0;/* 层级从0层开始 */

  //              std::vector<std::tuple<ots::IRect, int, ots::IRect,int,int> > vecIRect;
  //              QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
  //              temp_vectItem.clear();
  //              for (int itemLayout = 0; itemLayout < vecTupleLayout.size();itemLayout++)
  //              {
  //                  QRect layRect = std::get<3>(vecTupleLayout[itemLayout]); //布局Rect

  //                  QRect rectInter = layRect.intersected(devRect);//重叠区域

  //                  if (rectInter.width() > 1 && rectInter.height() > 1)
  //                  {
		//				//计算解码器画面在拼接大屏上的位置信息
  //                      QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
  //                      ots::IRect destRt;
  //                      destRt.left = getNumforT(rtDest.left(), 4);
  //                      destRt.top = getNumforT(rtDest.top(), 4);
  //                      destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
  //                      destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

  //                      QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

  //                      /*
  //                      如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
  //                      */
  //                      for (int temp = 0; temp < temp_vectItem.size(); temp++)
  //                      {
  //                          if (tempDesRect.intersected(temp_vectItem[temp]) != QRect())
  //                          {
  //                              layout++;
  //                              temp_vectItem.clear();
  //                              break;
  //                          }
  //                      }
		//				
		//				//计算编码器画面在拼接大屏上的位置
  //                      QRect rtran = rectInter.translated(layRect.topLeft() * -1);
  //                      float fx = (float(layRect.width())) / 1920.0;
  //                      float fy = (float(layRect.height())) / 1080.0;
  //                      ots::IRect srcRt;
  //                      srcRt.left = (long)(rtran.left() / fx);
  //                      srcRt.top = (long)(rtran.top() / fy);
  //                      srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
  //                      srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

  //                      vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt,layout,0));//目的矩形、编码器ID、源矩形

  //                      /*Todo:保存矩形大小*/
  //                      temp_vectItem.push_back(tempDesRect);
  //                  }
  //              }
  //              pairInfo.second = vecIRect;
  //              vecDecodeToIRect.push_back(pairInfo);
  //          }
  //      }

  //      // 发送应用命令
  //      otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);
  //      pollData._currnetIndex++;
  //      pollData._currnetIndex %= pollData._vecModels.size();//循环
    }
}

//回退功能
void MainWidget::slotUndo()
{
	++_undoButtonClickCnt;
	std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> vecLayoutInfo;
	std::tuple<int, int, std::string, int, int, int > mouldInfo;
	int screenID = _pScreenWidget->getScreenID();
#if 0
	if (_undoButtonClickCnt == 1)
	{
		int retCode = otsServer::DataSession::instance()->getMouldFrameFromDB(_operationFrameIndex, -999, -1, 0, screenID, vecLayoutInfo, mouldInfo); //第一次
		if (1 == retCode)
		{
			qInfo() << "vecLayoutInfoSize:" << vecLayoutInfo.size();
			int mouldID = std::get<1>(mouldInfo);
			std::string mouldName = std::get<2>(mouldInfo);
			
			//接口参数需要删除一些，冗余了！！！
			//设置界面布局
			int mouldType = std::get<3>(mouldInfo);
			int frameIndex = std::get<5>(mouldInfo);
			_pScreenWidget->setScreenLayoutWidget(mouldType, vecLayoutInfo);
			qInfo() << "mouldType:" << mouldType << "frameIndex:" << frameIndex << "curFrameIndex:" << _operationFrameIndex;
		}
		else if (-2 == retCode)
		{
			_pScreenWidget->setScreenLayoutWidget(0, vecLayoutInfo);
			_pUndoBT->setEnabled(false);
			_pRedoBT->setEnabled(true);
		}
		else if (-3 == retCode)
		{
			_pUndoBT->setEnabled(true);
			_pRedoBT->setEnabled(false);
		}
		else 
		{
			qInfo() << QStringLiteral("数据异常");
			return;
		}
	}
	else
	{
		int tempIndex = _operationFrameIndex;
		int retCode = otsServer::DataSession::instance()->getMouldFrameFromDB(_operationFrameIndex, tempIndex, -1, 0, screenID, vecLayoutInfo, mouldInfo); //第一次
		if (1 == retCode)
		{
			qInfo() << "vecLayoutInfoSize:" << vecLayoutInfo.size();
			int mouldType = std::get<3>(mouldInfo);
			int frameIndex = std::get<5>(mouldInfo);
			_pScreenWidget->setScreenLayoutWidget(mouldType, vecLayoutInfo);
			_pRedoBT->setEnabled(true);
			qInfo() << "mouldType:" << mouldType << "frameIndex:" << frameIndex << "curFrameIndex:" << _operationFrameIndex;
		}
		else if (-2 == retCode)
		{
			_pScreenWidget->setScreenLayoutWidget(0, vecLayoutInfo);
			_pUndoBT->setEnabled(false);
			_pRedoBT->setEnabled(true);
		}
		else if (-3 == retCode)
		{
			_pUndoBT->setEnabled(true);
			_pRedoBT->setEnabled(false);
		}
		else
		{
			qInfo() << QStringLiteral("数据异常");
			return;
		}
	}
	if (_operationFrameIndex < 0)
	{
		return;
	}
#endif

    int curIndex = -1;
    int parentIndex = -1;
    int retCode = otsServer::DataSession::instance()->getIndexByScreenID(screenID, curIndex);

    if (-1 == curIndex)
    {
        qInfo() << QString::fromUtf8("当前帧不存在，无法进行撤销操作");
        return;
    }

    retCode = otsServer::DataSession::instance()->getMouldFrameFromDB(parentIndex, curIndex, -1, 0, screenID, vecLayoutInfo, mouldInfo);
    if (-1 == parentIndex)
    {
        qInfo() << QString::fromUtf8("已经是最后一帧，无法进行撤销操作");
        //_pUndoBT->setEnabled(false);
        //_pRedoBT->setEnabled(true);
        return;
    }
    if (1 == retCode)
    {
        qInfo() << "vecLayoutInfoSize:" << vecLayoutInfo.size();
        int mouldType = std::get<3>(mouldInfo);
        int frameIndex = std::get<5>(mouldInfo);
        _pScreenWidget->setScreenLayoutWidget(mouldType, vecLayoutInfo);
        _pRedoBT->setEnabled(true);
        qInfo() << "mouldType:" << mouldType << "parentIndex:" << parentIndex;
        retCode = otsServer::DataSession::instance()->setIndexByScreenID(screenID, parentIndex);
    }
}

//重做功能
void MainWidget::slotRedo()
{
#if 0
	++_redoButtonClickCnt;
	std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> vecLayoutInfo;
	std::tuple<int, int, std::string, int, int, int > mouldInfo;
	int screenID = _pScreenWidget->getScreenID();
	
	int retCode = otsServer::DataSession::instance()->getMouldFrameFromDB(_operationFrameIndex, _operationFrameIndex, 1, 0, screenID, vecLayoutInfo, mouldInfo); //第一次
	if (1 == retCode)
	{
		int mouldType = std::get<3>(mouldInfo);
		int frameIndex = std::get<5>(mouldInfo);
		_pScreenWidget->setScreenLayoutWidget(mouldType, vecLayoutInfo);
		_pUndoBT->setEnabled(true);
		qInfo() << "mouldType:" << mouldType << "frameIndex:" << frameIndex << "curFrameIndex:" << _operationFrameIndex;
	}
	else if (-2 == retCode)
	{
		_pUndoBT->setEnabled(false);
		_pRedoBT->setEnabled(true);
	}
	else if (-3 == retCode)
	{
		_pUndoBT->setEnabled(true);
		_pRedoBT->setEnabled(false);
	}
	else
	{
		qInfo() << QStringLiteral("数据异常");
		return;
	}
#endif
    int curIndex = -1;
    int frameIndex = -1;
    std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> vecLayoutInfo;
    std::tuple<int, int, std::string, int, int, int > mouldInfo;

    int screenID = _pScreenWidget->getScreenID();
    otsServer::DataSession::instance()->getIndexByScreenID(screenID, curIndex);
    if (-1 == curIndex)
    {
        qInfo() << QString::fromUtf8("当前帧不存在，无法进行撤销操作");
        return;
    }

    /*otsServer::DataSession::instance()->getNextIndex_Spe(curIndex, frameIndex);*/



    int retCode = otsServer::DataSession::instance()->getMouldFrameFromDB(frameIndex, curIndex, 1, 0, screenID, vecLayoutInfo, mouldInfo); //第一次
    //if (frameIndex == -99)
    //{
    //    qInfo() << QString::fromUtf8("当前已经重做到最新状态，无法继续重做！");
    //    return;
    //}
    //if (frameIndex == -1)
    //{
    //    qInfo() << QString::fromUtf8("当前已经重做到最新状态，无法继续重做！");
    //    return;
    //}
    if (1 == retCode)
    {
        int mouldType = std::get<3>(mouldInfo);
        int frameIndex = std::get<5>(mouldInfo);
        _pScreenWidget->setScreenLayoutWidget(mouldType, vecLayoutInfo);
        retCode = otsServer::DataSession::instance()->setIndexByScreenID(screenID, frameIndex);
    }
}

/* 上传图片弹窗处理 */
void MainWidget::slotUpLoadImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), ".", ("Images (*.png *.xpm *.jpg)"), 0);
    if (filePath.isEmpty())
    {
        return;
    }
    int pos = filePath.lastIndexOf('/');
    QString fileName = filePath.right(filePath.length() - pos - 1);
    int screenID = _pScreenWidget->getScreenID();
    otsControler::ControlerData::instance()->updateImg2Decoder(std::string((const char *)filePath.toLocal8Bit().constData()),
        std::string((const char *)fileName.toLocal8Bit().constData()), screenID);
    QString screenBgName = "./resource/"+QString::number(screenID) + ".png";
    QFile::remove(screenBgName);
    if (QFile::copy(filePath, screenBgName))
    {
        _pScreenWidget->resetBgPixmap();
    }
    //int pos = path.find_last_of('/');
    //string s(path.substr(pos + 1));
}

//添加镜像通道
void MainWidget::slotAddMirror()
{
	if (!_pAddMirrorWidget)
	{
		_pAddMirrorWidget = new AddMirrorWidget(this);
	}
	int currentScreenID = _pScreenModelWidget->getCurrentScreenID();
	_pAddMirrorWidget->creatSrcList(currentScreenID);
	int result = _pAddMirrorWidget->exec();
	if (result == QDialog::Accepted)
	{
		int mirrorChannelId = 0; 
		QString mirrorName = _pAddMirrorWidget->getMirrorName();
		std::vector<int> vecMirrors = _pAddMirrorWidget->getMirrorData();
		int retCode = otsServer::DataSession::instance()->addMirror(mirrorChannelId, mirrorName.toStdString(), vecMirrors);
		if (1 == retCode)
		{
			if (mirrorChannelId != 0)
			{
				_pMirrorWidget->addItem(mirrorChannelId, mirrorName);
			}
			qInfo() << "add Mirror success!!!";
		}
		else if (-99 == retCode)
		{
			//重复
			qInfo() << "add Mirror add repeated!!!";
			SCMessageBox* addMirrorRepeatDlg = new SCMessageBox(tr("Add Mirror Channel Repeated Hint"), tr("Please verify that the Mirror Channel name is duplicated!"), otsCommon::SCMessageBox::Information, this);
			addMirrorRepeatDlg->exec();
			addMirrorRepeatDlg->deleteLater();
			addMirrorRepeatDlg = nullptr;
		}
		else
		{
			qInfo() << "add Mirror add failed!!!";
			//失败
			SCMessageBox* addMirrorFailDlg = new SCMessageBox(tr("Add Mirror Channel Failed"), tr("Sorry,Please add the Mirror Channel again!"), otsCommon::SCMessageBox::Information, this);
			addMirrorFailDlg->exec();
			addMirrorFailDlg->deleteLater();
			addMirrorFailDlg = nullptr;
		}
	}
	if (_pAddMirrorWidget)
	{
		_pAddMirrorWidget->deleteLater();
		_pAddMirrorWidget = nullptr;
	}

}

//删除镜像通道
void MainWidget::slotDelMirror()
{
	//镜像通道方案ID
	int mirrorSchemeId = _pMirrorWidget->getSelectedMirrorSchemeId();
	if (mirrorSchemeId > 0)
	{
		SCMessageBox* myBox1 = new SCMessageBox(tr("Delete Mirror Channel Hint"), tr("Are you sure to delete the current Mirror Channel?"), otsCommon::SCMessageBox::Warning, this);
		myBox1->exec();
		if (myBox1->result() == QDialog::Accepted)
		{
			int retCode = otsServer::DataSession::instance()->delMirror(mirrorSchemeId);
			if (retCode)
			{
				_pMirrorWidget->delSelectedItem();
				OTS_LOG->debug("del poll mould success!!!, groupID:{}", mirrorSchemeId);
			}
			else
			{
				OTS_LOG->error("del poll mould fail!!!, groupID:{}", mirrorSchemeId);
			}
		}
		myBox1->deleteLater();
		myBox1 = nullptr;
	}
}

//应用镜像通道
void MainWidget::slotApplyMirror(bool state, int mirrorID)
{
	//其他屏幕投放相同信号画面
	//根据已有大屏ID数组，获取大屏(行数、列数、大屏ID)
	//根据当前自己选择大屏排布给其他大屏设置排布
	//根据当前自己选择的大屏预案，获取编码器ID、布局名称、布局Rect、布局排序，将这些信息经过计算应用到其他的大屏上去。
	//投放视频信号接口；

	if (!state)
	{
		qInfo() << QString::fromUtf8("镜像通道关闭");
		return;
	}
	int screenID = _pScreenWidget->getScreenID(); //当前大屏ID
	int mirrorId = _pMirrorWidget->getSelectedMirrorSchemeId(); //镜像通道方案ID
	int moduleId = _pScreenWidget->getScreenModuleID(); //预案ID
	int moduleType = _pScreenWidget->getScreenLayouType(); //排布类型
	if (mirrorId <= 0) 
	{
		return;
	}

	/* 通过镜像通道ID拉取该方案下的大屏ID */
	std::vector<int> vecAllScreenID;
	otsServer::DataSession::instance()->getScreenByMirrorID(mirrorId, vecAllScreenID);

	/* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
	std::tuple<int, int, int> rowColPro; 
	std::vector<std::tuple<int, int, int> > vecDecoderID; 

	bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(screenID, rowColPro, vecDecoderID);

	int row = std::get<0>(rowColPro);
	int col = std::get<1>(rowColPro);

#if 0
	std::map<int, int> mapIndex2Decoder;
	mapIndex2Decoder.clear();
	/*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
	for (std::size_t i = 0; i < vecDecoderID.size(); ++i)
	{
		int decoderId = std::get<2>(vecDecoderID[i]);
		int num = std::get<0>(vecDecoderID[i]) * col + std::get<1>(vecDecoderID[i]);
		mapIndex2Decoder[num] = decoderId;
	}
#endif

#ifdef OLD
    int totalWidth = col * 1920;
    int totalHeight = row * 1080;
    //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
    std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;
    std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

    //从当前界面获取排布信息
    QList<ScreenLayoutItem*> listScreenLayout = _pScreenWidget->getScreenLayoutItems();
    qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
    for (int i = 0; i < listScreenLayout.size(); i++)
    {
        //if (!_pScreenWidget->getScreenLayoutWidget()->existItem(listScreenLayout[i]))
        //{
        //	listScreenLayout[i]->deleteLater();
        //	continue;
        //}

        int left = listScreenLayout[i]->getRealityRect().left();
        int top = listScreenLayout[i]->getRealityRect().top();
        int right = listScreenLayout[i]->getRealityRect().right();
        int bottom = listScreenLayout[i]->getRealityRect().bottom();
        int sequence = i;
        int encoderId = listScreenLayout[i]->getEncoderID();
        qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
        std::tuple<int, std::string, int, QRect> layoutInfo;
        layoutInfo = std::make_tuple(encoderId, "", sequence, QRect(left, top, right - left, bottom - top));
        vecTupleLayout.push_back(layoutInfo);
    }
    //error retCode = otsServer::DataSession::instance()->getLayoutInfo(moduleId, vecTupleLayout);

    for (int i = 0; i < vecTupleLayout.size(); i++)
    {
        //计算当前编码器信号在当前大屏上的屏幕占比
        int encoderId = std::get<0>(vecTupleLayout[i]);
        std::string layoutName = std::get<1>(vecTupleLayout[i]);
        int sequence = std::get<2>(vecTupleLayout[i]);
        QRect curRect = std::get<3>(vecTupleLayout[i]);
        QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

        vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
        qInfo() << std::get<3>(vecNewTupleLayout[i]);
    }

    //通过大屏ID获取大屏信息
    for (int k = 0; k < vecAllScreenID.size(); k++)
    {
        if (vecAllScreenID[k] == screenID)
        {
            continue;
        }

        std::map<int, int> mapIndex2Decoder;
        mapIndex2Decoder.clear();

        std::tuple<int, int, int> rowCol;
        std::vector<std::tuple<int, int, int> > vecDecoderInfo;
        otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
        //大屏的行列，宽高信息
        int newRow = std::get<0>(rowCol);
        int newCol = std::get<1>(rowCol);
        int newTotalWidth = newCol * 1920;
        int newTotalHeight = newRow * 1080;

        /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
        std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;

        /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
        for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
        {
            int decoderId = std::get<2>(vecDecoderInfo[i]);
            int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
            mapIndex2Decoder[num] = decoderId;
        }

        //保存帧数据
        bool tempState = true;
        std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;

        for (int i = 0; i < newRow; ++i)
        {
            for (int j = 0; j < newCol; ++j)
            {
                ots::PairDecodeToIRect pairInfo;
                int num = i * newCol + j;
                pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

                std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
                for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                {
                    QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                    layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                    QRect newLayRect = layRectF.toRect();


                    if (tempState)
                    {
                        std::tuple<int, int, int, int, int, int > layoutInfo;
                        layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                        vecLayoutInfo.push_back(layoutInfo);
                    }

                    QRect rectInter = newLayRect.intersected(devRect);//重叠区域

                    if (rectInter.width() > 1 && rectInter.height() > 1)
                    {
                        //计算解码器画面在拼接大屏上的位置信息
                        QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
                        ots::IRect destRt;
                        destRt.left = getNumforT(rtDest.left(), 4);
                        destRt.top = getNumforT(rtDest.top(), 4);
                        destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
                        destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

                        //计算编码器画面在拼接大屏上的位置
                        QRect rtran = rectInter.translated(newLayRect.topLeft() * -1);
                        float fx = (float(newLayRect.width())) / 1920.0;
                        float fy = (float(newLayRect.height())) / 1080.0;
                        ots::IRect srcRt;
                        srcRt.left = (long)(rtran.left() / fx);
                        srcRt.top = (long)(rtran.top() / fy);
                        srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                        srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                        vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, 0, 0));//目的矩形、编码器ID、源矩形
                    }
                }
                tempState = false;

                pairInfo.second = vecIRect;
                vecDecodeToIRect.push_back(pairInfo);
            }
        }

        // 发送投屏命令
        otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

        //镜像开启后,保存每块大屏的帧数据
        int tempIndex = -1;
        otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
        qInfo() << "index:" << tempIndex;
        int backIndexOfOtherScreen = -1;
        retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, moduleType);/*<开启镜像，排布类型相同>*/
        retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
    }
#else
    int totalWidth = col * 1920;
    int totalHeight = row * 1080;
    //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
    std::vector<std::tuple<int, std::string, int, QRectF> > vecTupleLayout;
    std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

    //从当前界面获取排布信息
    QList<ScreenLayoutItem*> listScreenLayout = _pScreenWidget->getScreenLayoutItems();
    qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
    for (int i = 0; i < listScreenLayout.size(); i++)
    {
        //if (!_pScreenWidget->getScreenLayoutWidget()->existItem(listScreenLayout[i]))
        //{
        //	listScreenLayout[i]->deleteLater();
        //	continue;
        //}

        qreal left = listScreenLayout[i]->getRealityRectF().left();
        qreal top = listScreenLayout[i]->getRealityRectF().top();
        qreal right = listScreenLayout[i]->getRealityRectF().right();
        qreal bottom = listScreenLayout[i]->getRealityRectF().bottom();
        int sequence = i;
        int encoderId = listScreenLayout[i]->getEncoderID();
        qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
        std::tuple<int, std::string, int, QRectF> layoutInfo;
        layoutInfo = std::make_tuple(encoderId, "", sequence, QRectF(left, top, right - left, bottom - top));
        vecTupleLayout.push_back(layoutInfo);
    }
    //error retCode = otsServer::DataSession::instance()->getLayoutInfo(moduleId, vecTupleLayout);

    for (int i = 0; i < vecTupleLayout.size(); i++)
    {
        //计算当前编码器信号在当前大屏上的屏幕占比
        int encoderId = std::get<0>(vecTupleLayout[i]);
        std::string layoutName = std::get<1>(vecTupleLayout[i]);
        int sequence = std::get<2>(vecTupleLayout[i]);
        QRectF curRect = std::get<3>(vecTupleLayout[i]);
        QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

        vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
        qInfo() << std::get<3>(vecNewTupleLayout[i]);
    }

    //通过大屏ID获取大屏信息
    for (int k = 0; k < vecAllScreenID.size(); k++)
    {
        if (vecAllScreenID[k] == screenID)
        {
            continue;
        }

        std::map<int, int> mapIndex2Decoder;
        mapIndex2Decoder.clear();

        std::tuple<int, int, int> rowCol;
        std::vector<std::tuple<int, int, int> > vecDecoderInfo;
        otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
        //大屏的行列，宽高信息
        int newRow = std::get<0>(rowCol);
        int newCol = std::get<1>(rowCol);
        int newTotalWidth = newCol * 1920;
        int newTotalHeight = newRow * 1080;

        /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
        std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;

        /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
        for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
        {
            int decoderId = std::get<2>(vecDecoderInfo[i]);
            int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
            mapIndex2Decoder[num] = decoderId;
        }

		QVector<QRectF> temp_vectItem;

        //保存帧数据
        bool tempState = true;
        std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;

        for (int i = 0; i < newRow; ++i)
        {
            for (int j = 0; j < newCol; ++j)
            {
                ots::PairDecodeToIRect pairInfo;
                int num = i * newCol + j;
                pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID
				int layout = 0;/* 层级从0层开始 */

                std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
				temp_vectItem.clear();

                for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                {
                    QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                    layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                    QRectF newLayRect = layRectF;


                    if (tempState)
                    {
                        std::tuple<int, int, int, int, int, int > layoutInfo;
                        layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                        vecLayoutInfo.push_back(layoutInfo);
                    }

                    QRectF rectInter = newLayRect.intersected(devRect);//重叠区域

                    if (rectInter.width() > 1 && rectInter.height() > 1)
                    {
                        //计算解码器画面在拼接大屏上的位置信息
                        QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                        ots::IRect destRt;
                        destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                        destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                        destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                        destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

						QRectF tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

						for (int temp = 0; temp < temp_vectItem.size(); temp++)
						{
							if (tempDesRect.intersected(temp_vectItem[temp]) != QRectF())
							{
								layout++;
								temp_vectItem.clear();
								break;
							}
						}

                        //计算编码器画面在拼接大屏上的位置
                        QRectF rtran = rectInter.translated(newLayRect.topLeft() * -1);
                        float fx = (float(newLayRect.width())) / 1920.0;
                        float fy = (float(newLayRect.height())) / 1080.0;
                        ots::IRect srcRt;
                        srcRt.left = (long)(rtran.left() / fx);
                        srcRt.top = (long)(rtran.top() / fy);
                        srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                        srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

						vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

						/*Todo:保存矩形大小*/
						temp_vectItem.push_back(tempDesRect);
                    }
                }
                tempState = false;

                pairInfo.second = vecIRect;
                vecDecodeToIRect.push_back(pairInfo);
            }
        }

        // 发送投屏命令
        otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

        //镜像开启后,保存每块大屏的帧数据
        int tempIndex = -1;
        otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
        qInfo() << "index:" << tempIndex;
        int backIndexOfOtherScreen = -1;
        retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, moduleType);/*<开启镜像，排布类型相同>*/
        retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
    }
#endif
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
	QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm");
	_pTimeLabel->setText(current_date);
}

void otsDisplayControl::MainWidget::initUIData()
{
    _pScreenSwitchWidget->createData();
    _pSignalGroupTreeWidget->initUIData();

    /* 初始化选择一个大屏 */
    _pScreenSwitchWidget->setDefaultScreen();
}

void otsDisplayControl::MainWidget::setScreenLayoutWidget_net(int moduleType, std::vector<std::tuple<std::string, int, int, int, int, int>> &vecLayoutInfo)
{
    _pScreenWidget->setScreenLayoutWidget_net(moduleType, vecLayoutInfo);
}

int otsDisplayControl::MainWidget::getCurrentScreenID()
{
    return _pScreenWidget->getScreenID();
}

void otsDisplayControl::MainWidget::updateMouldItem(const int mouldid)
{
    _pScreenModelWidget->slotScreenChanged(_pScreenWidget->getScreenID());
}

void otsDisplayControl::MainWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_pClearBT->setText(_pClearBT->text());
			_pUndoBT->setText(_pUndoBT->text());
			_pRedoBT->setText(_pRedoBT->text());
			_pUploadBT->setText(_pUploadBT->text());
			_pMiniBT->setText(_pMiniBT->text());
			_pCloseBT->setText(_pCloseBT->text());
			qInfo() << "otsDisplayControl MainWidget:---LangChang-----";
		}
		break;
	default:
		break;
	}
}

void otsDisplayControl::MainWidget::slotDelModel(int modelID)
{
	if (modelID > 0)
	{
		SCMessageBox* myBox2 = new SCMessageBox(tr("Delete Module Hint"), tr("Are you Sure to delete current module?"), otsCommon::SCMessageBox::Warning, this);
		myBox2->exec();
		if (myBox2->result() == QDialog::Accepted)
		{
            if (g_masterID == g_self_ID)
            {
                send_del_mould_from_master(_pScreenWidget->getScreenID(), modelID, g_self_ID);
                _pScreenModelWidget->delSelectedItem();
                OTS_LOG->debug("del modelID success!!!, groupID:{}", modelID);
            }
            else
            {
                send_del_mould_from_slave(_pScreenWidget->getScreenID(), modelID, g_self_ID);
            }

            /*int retCode = otsServer::DataSession::instance()->delMould(modelID);
            if (retCode)
            {
            _pScreenModelWidget->delSelectedItem();
            OTS_LOG->debug("del modelID success!!!, groupID:{}", modelID);
            }
            else
            {
            OTS_LOG->error("del modelID fail!!!, groupID:{}", modelID);
            }*/
		}
		myBox2->deleteLater();
		myBox2 = nullptr;
	}
}

void otsDisplayControl::MainWidget::slotModelChanged(int modelID)
{
    if (!_pScreenModelWidget->isAutoPreview())
    {
        PreviewDialog* dialog = new PreviewDialog(this);
        dialog->setScreen(_pScreenWidget->getScreenID());
        dialog->setModel(modelID);
        int resoult = dialog->exec();
        switch (resoult)
        {
        case QDialog::Accepted:
        {
            _pScreenWidget->slotScreenModelChanged(modelID);
        }
            break;
        case QDialog::Rejected:
        {
        }
            break;
        default:
            break;
        }
        dialog->deleteLater();
    }
    else
    {
        _pScreenWidget->slotScreenModelChanged(modelID);
    }
}

void otsDisplayControl::MainWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    /* 初始化选择一个大屏 */
    _pScreenSwitchWidget->setDefaultScreen();
    /* 初始化选择一个信号源组 */
    _pSignalGroupTreeWidget->setDefaultSelectItem();
}

void otsDisplayControl::MainWidget::processPollData(PollData& pollData)
{
    int nextModel = pollData._vecModels[pollData._currnetIndex];
    /* 找到大屏信息 (行、列、解码器ID以及解码板所在行列)*/
    std::tuple<int, int, int> rowColPro; //大屏属性：行、列
    std::vector<std::tuple<int, int, int> > vecDecoderID;
    std::map<int, int> mapIndex2Decoder;
    otsServer::DataSession::instance()->getScreenDecoderInfo(pollData._screenID, rowColPro, vecDecoderID);
    int type = 1;
    otsServer::DataSession::instance()->getMouldTypeByID(nextModel, type);
    /* 通过model找到排布数据(编码器ID、布局名称、堆叠顺序索引、布局Rect) */
    std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;

    /** 如果轮巡中的预案已被删除是否跳过，还是说在删除预案的时候旧主动将轮巡中的预案删除 */
    otsServer::DataSession::instance()->getLayoutInfo(nextModel, vecTupleLayout);

    pollData._currnetIndex++;
    pollData._currnetIndex %= pollData._vecModels.size();//循环

    /** 如果大屏ID是当前显示操作的大屏则进行显示操作，否则进行下面的操作 */
    if (pollData._screenID == _pScreenWidget->getScreenID())
    {
        _pScreenWidget->slotScreenModelChanged(nextModel);
        return;
    }

    std::vector<ots::PairDecodeToIRect> vecDecodeToIRect; //<解码器ID、目的矩形、编码器ID、源矩形>vector
    int _row = std::get<0>(rowColPro);
    int _col = std::get<1>(rowColPro);
    mapIndex2Decoder.clear();

    //解析vecDecoderID,根据解码板所在行列数，计算后将位置Index与decoderID对应起来
    for (std::size_t i = 0; i < vecDecoderID.size(); ++i)
    {
        int decoderId = std::get<2>(vecDecoderID[i]);
        int num = std::get<0>(vecDecoderID[i]) * _col + std::get<1>(vecDecoderID[i]);
        mapIndex2Decoder[num] = decoderId;
    }

    QVector<QRectF> temp_vectItem;
    //保存帧数据
    bool tempState = true;
    std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
    for (int i = 0; i < _row; ++i)
    {
        for (int j = 0; j < _col; ++j)
        {
            ots::PairDecodeToIRect pairInfo;
            int num = i * _col + j;
            pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID
            int layout = 0;/* 层级从0层开始 */

            std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
            QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
            temp_vectItem.clear();
            for (int itemLayout = 0; itemLayout < vecTupleLayout.size(); itemLayout++)
            {
                QRectF layRect = std::get<3>(vecTupleLayout[itemLayout]); //布局Rect

                if (tempState)
                {
                    std::tuple<int, int, int, int, int, int > layoutInfo;
                    layoutInfo = std::make_tuple(layRect.left(), layRect.top(), layRect.right(), layRect.bottom(), itemLayout, std::get<0>(vecTupleLayout[itemLayout]));
                    vecLayoutInfo.push_back(layoutInfo);
                }

                QRectF rectInter = layRect.intersected(devRect);//重叠区域

                if (rectInter.width() > 1 && rectInter.height() > 1)
                {
                    //计算解码器画面在拼接大屏上的位置信息
                    QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                    ots::IRect destRt;
                    destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                    destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                    destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                    destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

                    QRectF tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                    /*
                    如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
                    */
                    for (int temp = 0; temp < temp_vectItem.size(); temp++)
                    {
                        if (tempDesRect.intersected(temp_vectItem[temp]) != QRectF())
                        {
                            layout++;
                            temp_vectItem.clear();
                            break;
                        }
                    }

                    //计算编码器画面在拼接大屏上的位置
                    QRectF rtran = rectInter.translated(layRect.topLeft() * -1);
                    float fx = (float(layRect.width())) / 1920.0;
                    float fy = (float(layRect.height())) / 1080.0;
                    ots::IRect srcRt;
                    srcRt.left = (long)(rtran.left() / fx);
                    srcRt.top = (long)(rtran.top() / fy);
                    srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                    srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                    vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

                    /*Todo:保存矩形大小*/
                    temp_vectItem.push_back(tempDesRect);
                }
            }
            tempState = false;
            pairInfo.second = vecIRect;
            vecDecodeToIRect.push_back(pairInfo);
        }
    }

    // 发送应用命令
    otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

    {
        //轮巡开启后,保存轮巡大屏的帧数据
        int tempIndex = -1;
        otsServer::DataSession::instance()->getIndexByScreenID(pollData._screenID, tempIndex);
        qInfo() << "index:" << tempIndex;
        int backIndexOfOtherScreen = -1;
        otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, pollData._screenID, vecLayoutInfo, type);/*<开启镜像，排布类型相同>*/
        otsServer::DataSession::instance()->setIndexByScreenID(pollData._screenID, backIndexOfOtherScreen);
    }

	//当开启了轮巡预案，切换大屏，也就是说，当大屏发生切换后，如果镜像通道开启了，就投屏一次
    int curMirrorId = 0;
    otsServer::DataSession::instance()->check_MirrorScreenStatus_byScreenID(curMirrorId, pollData._screenID);

	//bool isMirrorStart = _pMirrorWidget->getMirrorState();
    bool isMirrorStart = curMirrorId != 0 ? true : false;
	qInfo() << QStringLiteral("镜像通道打开状态：") << isMirrorStart;
	//若开启了镜像通道，则当前选择镜像Id一定存在
	if (isMirrorStart)
	{
		/*int curMirrorId = _pMirrorWidget->getSelectedMirrorSchemeId();
		int screenId = _pScreenWidget->getScreenID();
		int screenModuleId = _pScreenWidget->getScreenModuleID();*/
        int screenId = pollData._screenID;
        int screenModuleId = nextModel;

		QVector<QRect> temp_vectRectItem;

		/* 通过镜像通道ID拉取该方案下的大屏ID */
		std::vector<int> vecAllScreenID;
		otsServer::DataSession::instance()->getScreenByMirrorID(curMirrorId, vecAllScreenID);

		/* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
		std::tuple<int, int, int> rowColPro;
		std::vector<std::tuple<int, int, int> > vecDecoderID;

		bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(screenId, rowColPro, vecDecoderID);
		if (!retCode)
		{
			qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
			return;
		}

		int row = std::get<0>(rowColPro);
		int col = std::get<1>(rowColPro);
		int totalWidth = col * 1920;
		int totalHeight = row * 1080;
		//param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
		std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;
		std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;
		retCode = otsServer::DataSession::instance()->getLayoutInfo(screenModuleId, vecTupleLayout);

		for (int i = 0; i < vecTupleLayout.size(); i++)
		{
			//计算当前编码器信号在当前大屏上的屏幕占比
			int encoderId = std::get<0>(vecTupleLayout[i]);
			std::string layoutName = std::get<1>(vecTupleLayout[i]);
			int sequence = std::get<2>(vecTupleLayout[i]);
			QRect curRect = std::get<3>(vecTupleLayout[i]);
			QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

			vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
			qInfo() << std::get<3>(vecNewTupleLayout[i]);
		}


		/* <解码器ID、目的矩形、编码器ID、源矩形>vector */
		std::vector<ots::PairDecodeToIRect> vecDecoderToIRect;

		//通过大屏ID获取大屏信息
        for (int screenItem = 0; screenItem < vecAllScreenID.size(); screenItem++)
		{
            if (vecAllScreenID[screenItem] == pollData._screenID)
            {
                continue;
            }
			std::map<int, int> mapIndex2Decoder;
			mapIndex2Decoder.clear();

			std::tuple<int, int, int> rowCol;
			std::vector<std::tuple<int, int, int> > vecDecoderInfo;
            bool state_save = true;
            bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[screenItem], rowCol, vecDecoderInfo);
			if (!retCode)
			{
				qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
				return;
			}
			//大屏的行列，宽高信息
			int newRow = std::get<0>(rowCol);
			int newCol = std::get<1>(rowCol);
			int newTotalWidth = newCol * 1920;
			int newTotalHeight = newRow * 1080;

			/*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
			for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
			{
				int decoderId = std::get<2>(vecDecoderInfo[i]);
				int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
				mapIndex2Decoder[num] = decoderId;
			}
            vecLayoutInfo.clear();
            std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> vecLayoutInfo_2;
			for (int i = 0; i < newRow; ++i)
			{
				for (int j = 0; j < newCol; ++j)
				{
					int lay = 0;/* 层级从0层开始 */
					temp_vectRectItem.clear();

					ots::PairDecodeToIRect pairInfo;
					int num = i * newCol + j;
					pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

					std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
					QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
					for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
					{
						QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
						layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
						QRectF newLayRect = layRectF;
                        if (state_save)
                        {
                            //LayoutID, Type, Left, Top, Right, Bottom, MouldID, Seq, EncoderID, UserID
                            vecLayoutInfo_2.push_back(std::make_tuple(0, 0, layRectF.left(), layRectF.top(), layRectF.right(), layRectF.bottom(),
                                0, itemLayout, std::get<0>(vecTupleLayout[itemLayout]), 0));
                            std::tuple<int, int, int, int, int, int > layoutInfo;
                            layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecTupleLayout[itemLayout]));
                            vecLayoutInfo.push_back(layoutInfo);
                        }
						QRectF rectInter = newLayRect.intersected(devRect);//重叠区域

						if (rectInter.width() > 1 && rectInter.height() > 1)
						{
							//计算解码器画面在拼接大屏上的位置信息
							QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
							ots::IRect destRt;
                            destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                            destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                            destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                            destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);


							QRectF tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

							/*
							如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
							*/
							for (int temp = 0; temp < temp_vectItem.size(); temp++)
							{
								if (tempDesRect.intersected(temp_vectItem[temp]) != QRectF())
								{
									lay++;
									temp_vectItem.clear();
									break;
								}
							}

							//计算编码器画面在拼接大屏上的位置
							QRectF rtran = rectInter.translated(newLayRect.topLeft() * -1);
							float fx = (float(newLayRect.width())) / 1920.0;
							float fy = (float(newLayRect.height())) / 1080.0;
							ots::IRect srcRt;
							srcRt.left = (long)(rtran.left() / fx);
							srcRt.top = (long)(rtran.top() / fy);
							srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
							srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

							vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, lay, 0));//目的矩形、编码器ID、源矩形
						}
					}
                    state_save = false;
					pairInfo.second = vecIRect;
					vecDecoderToIRect.push_back(pairInfo);
				}
			}
            if (vecAllScreenID[screenItem] == _pScreenWidget->getScreenID())
            {
                _pScreenWidget->onlySetScreenLayoutWidget(type, vecLayoutInfo_2);
            }
            {
                //轮巡开启后,保存轮巡大屏的帧数据
                int tempIndex = -1;
                otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[screenItem], tempIndex);
                qInfo() << "index:" << tempIndex;
                int backIndexOfOtherScreen = -1;
                otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[screenItem], vecLayoutInfo, type);/*<开启镜像，排布类型相同>*/
                otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[screenItem], backIndexOfOtherScreen);
            }
		}

		// 发送应用命令
		otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecoderToIRect);
	}

#if 0
	//保存当前大屏界面帧数据
	//时间： 2019-10-16 15：44 解决插销重做异常问题！！！！ by songwentao
	//保存当前镜像通道中所有大屏界面帧数据
	std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
	QList<ScreenLayoutItem*> listScreenLayout = _pScreenWidget->getScreenLayoutItems();
	qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
	for (int i = 0; i < listScreenLayout.size(); i++)
	{
		int left = listScreenLayout[i]->getRealityRect().left();
		int top = listScreenLayout[i]->getRealityRect().top();
		int right = listScreenLayout[i]->getRealityRect().right();
		int bottom = listScreenLayout[i]->getRealityRect().bottom();
		int sequence = i;
		int encoderId = listScreenLayout[i]->getEncoderID();
		qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
		std::tuple<int, int, int, int, int, int> layoutInfo;
		layoutInfo = std::make_tuple(left, top, right, bottom, sequence, encoderId);
		vecLayoutInfo.push_back(layoutInfo);
	}

	//保存帧数据
	int tempIndex = 0;
	otsServer::DataSession::instance()->saveMouldFrame2DB(tempIndex, -1, 0, pollData._screenID, vecLayoutInfo, _pScreenWidget->getScreenLayouType());
#endif 
}

//void MainWidget::slotLoggout()
//{
//	//close();
//	QProcess *myProcess = new QProcess(this);
//	QString strCurPath = QCoreApplication::applicationDirPath();
//	qInfo() << "curPath:" << strCurPath;
//	QStringList arguments;
//	arguments << "/ScreenLogin_Test.exe";
//
//	myProcess->start(strCurPath, arguments);
//	strCurPath += "/ScreenLogin_Test.exe";
//	qInfo() << "WholePath: " << strCurPath;
//	int state = myProcess->waitForStarted();
//	qInfo() << "state:" << state;
//}