#include "PollWidget.h"
#include <QListWidgetItem>
#include <QPushButton>
#include "otsServer/DataSession.h"
#include <QDebug>
#include "otsXML/globalVar.h"
#include "ui_ChannelWidget.h"
#include <QPainter>
#include "otsCommon/MessageBox.h"
#include <QMouseEvent>
#include <QEvent>

PollWidget::PollWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ChannelWidget), _isChanged(false)
{
    ui->setupUi(this);
	ui->label->setText(tr("Poll Channel")); //轮巡通道
    QObject::connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(slotListItemChange(QListWidgetItem*, QListWidgetItem*)));
    QObject::connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(slotListItemClicked(QListWidgetItem*)));

	ui->auto_pushButton->setObjectName("poll-start");
	ui->del_pushButton->setObjectName("poll-del");
	ui->add_pushButton->setObjectName("poll-add");
    QObject::connect(ui->add_pushButton, SIGNAL(clicked()), this, SIGNAL(signalAddPoll()));
    QObject::connect(ui->del_pushButton, SIGNAL(clicked()), this, SLOT(slotDelClicked()));
    QObject::connect(ui->auto_pushButton, SIGNAL(clicked()), this, SLOT(slotStartStopButtonClicked()));

    ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
    ui->auto_pushButton->style()->polish(ui->auto_pushButton);

    /* 将listWidget的事件注册到本窗口进行过滤鼠标点击事件 */
    ui->listWidget->installEventFilter(this);
}

void PollWidget::createTestData()
{
    ui->listWidget->clear();
    ui->del_pushButton->setEnabled(true);
    //ui->auto_pushButton->setText(tr("Start"));
	ui->auto_pushButton->setObjectName("poll-start");
    ui->listWidget->setEnabled(true);
    for (int i = 0; i < 15; i++)
    {
        QListWidgetItem* item = new QListWidgetItem;
        QString name = "test";
        item->setData(Qt::UserRole, i);
        item->setData(Qt::DisplayRole, name);
        ui->listWidget->addItem(item);
    }

    ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
    ui->auto_pushButton->style()->polish(ui->auto_pushButton);
}

void PollWidget::createData(int currentScreenID)
{
    ui->listWidget->clear();
    //ui->del_pushButton->setEnabled(true);
	ui->auto_pushButton->setObjectName("poll-start");
    ui->listWidget->setEnabled(true);
    _mirrorApplyState = false;
    std::vector<std::tuple<int, std::string, int>> allPoll;
    otsServer::DataSession::instance()->getAllScreenPollGroup(currentScreenID, allPoll);
    for (int i = 0; i < allPoll.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem;
        int id = std::get<0>(allPoll[i]);
        QString name = std::get<1>(allPoll[i]).c_str();
        item->setData(Qt::UserRole, id);
        item->setData(Qt::DisplayRole, name);
        ui->listWidget->addItem(item);
        if (std::get<2>(allPoll[i]) == 1)
        {
            //ui->del_pushButton->setEnabled(false);
			ui->auto_pushButton->setObjectName("poll-stop");
            ui->listWidget->setEnabled(false);
            ui->listWidget->setCurrentItem(item);
            _mirrorApplyState = true;
        }
    }

    ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
    ui->auto_pushButton->style()->polish(ui->auto_pushButton);
}

void PollWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
        QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
    QWidget::paintEvent(event);
}

void PollWidget::addItem(int ID, QString strName)
{
    QListWidgetItem* item = new QListWidgetItem;
    item->setData(Qt::UserRole, ID);
    item->setData(Qt::DisplayRole, strName);
    ui->listWidget->insertItem(ui->listWidget->count(), item);
}

int PollWidget::getSelectedPollId()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (item)
    {
        return item->data(Qt::UserRole).toInt();
    }
    return 0;
}

void PollWidget::delSelectedItem()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (item)
    {
        ui->listWidget->takeItem(ui->listWidget->row(item));
        ui->listWidget->setCurrentItem(NULL);
        delete item;
    }
}

bool PollWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (ui->listWidget == watched)
    {
        if ((event->type() == QEvent::MouseButtonPress) && _mirrorApplyState)
        {
            QMouseEvent* pMouseEvent = (QMouseEvent*)(event);
            QPoint mousePoint = pMouseEvent->pos();
            QListWidgetItem* pMousePressItem = ui->listWidget->itemAt(mousePoint);
            if (pMousePressItem && (pMousePressItem != ui->listWidget->currentItem()))
            {//请先关闭当前轮巡
                otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Toggles Poll Hint"), tr("Please close the current Poll first."), otsCommon::SCMessageBox::Information, this);
                warning_widget->exec();
                warning_widget->deleteLater();
                return true;
            }
        }
    }
    return false;
}

void PollWidget::changeEvent(QEvent *event)
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

//大屏切换时，更新预案列表显示
void PollWidget::slotScreenChanged(int screenID)
{
    createData(screenID);
	setScreenId(screenID);
}

void PollWidget::slotStartStopButtonClicked()
{
    int item_count = ui->listWidget->count();

    if (item_count == 0)
    {//当前大屏没有轮巡可打开
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Poll Hint"), tr("There is no Poll to open on the current screen!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }

    QListWidgetItem* item = ui->listWidget->currentItem();
    if (!item)
    {//请先选中一个轮巡
		otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Poll Hint"), tr("Please select a Poll first!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
	QString objectNameText = ui->auto_pushButton->objectName();
	if (objectNameText == "poll-start")
    {
        /** 判断是否可以开启轮巡 */
        int startMirror = 0;
        if (!otsServer::DataSession::instance()->check_MirrorScreenStatus_byScreenID(startMirror, _screenId))
        {
            return;
        }

        if (startMirror != 0)
        {
            int total_startPoll = 0;
            if (!otsServer::DataSession::instance()->check_PollScreenStatus_byMirrorID(total_startPoll, startMirror))
            {
                return;
            }

            if (total_startPoll > 0)
            {//该大屏已开启镜像且镜像中存在轮巡
                otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Poll Hint"), tr("The screen has been mirrored and there is a Poll in the Mirror!"), otsCommon::SCMessageBox::Information, this);
                warning_widget->exec();
                warning_widget->deleteLater();
                return;
            }
        }

		//开启轮询通道，置灰删除按钮
		//ui->del_pushButton->setEnabled(false); 
		ui->auto_pushButton->setObjectName("poll-stop");
        ui->listWidget->setEnabled(false);
        emit signalPollGroupID(item->data(Qt::UserRole).toInt(), true);
        otsServer::DataSession::instance()->setPollGroupStatus(item->data(Qt::UserRole).toInt(), 1);
        _mirrorApplyState = true;
    }
	else if (objectNameText == "poll-stop")
    {
		//关闭轮询通道，使能删除按钮
        //ui->del_pushButton->setEnabled(true);
		ui->auto_pushButton->setObjectName("poll-start");
        ui->listWidget->setEnabled(true);
        emit signalPollGroupID(item->data(Qt::UserRole).toInt(), false);
        otsServer::DataSession::instance()->setPollGroupStatus(item->data(Qt::UserRole).toInt(), 0);
        _mirrorApplyState = false;
    }

    ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
    ui->auto_pushButton->style()->polish(ui->auto_pushButton);
}

void PollWidget::slotDelClicked()
{
    if (_mirrorApplyState)
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Poll Hint"), tr("Please close the current Poll first."), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
    int item_count = ui->listWidget->count();

    if (item_count == 0)
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Poll Hint"), tr("The current screen has no Poll that can be deleted."), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
    QListWidgetItem* pCurrentItem = ui->listWidget->currentItem();
    if (pCurrentItem)
    {
        emit signalDelPoll();
    }
    else
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Poll Hint"), tr("Please select a Poll first."), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
}

void PollWidget::slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous)
{
    _isChanged = true;
}

void PollWidget::slotListItemClicked(QListWidgetItem *current)
{
    if (!_isChanged)
    {
        ui->listWidget->setCurrentItem(NULL);
    }
	else
	{
		_isChanged = false;
	}
}
