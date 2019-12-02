#include "MirrorWidget.h"
#include "otsServer/DataSession.h"
#include "ui_ChannelWidget.h"
#include <QListWidgetItem>
#include <QDebug>
#include <QPainter>
#include "otsCommon/MessageBox.h"
#include <QMouseEvent>
#include <QEvent>

MirrorWidget::MirrorWidget(QWidget *parent) : QWidget(parent), ui(new Ui::ChannelWidget), _isChanged(false)
{
	ui->setupUi(this);
	ui->label->setText(tr("Mirror Channel"));//镜像通道
	QObject::connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
		this, SLOT(slotListItemChange(QListWidgetItem*, QListWidgetItem*)));
	QObject::connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
		this, SLOT(slotListItemClicked(QListWidgetItem*)));

	ui->auto_pushButton->setObjectName("mirror-start");
	ui->add_pushButton->setObjectName("mirror-add");
	ui->del_pushButton->setObjectName("mirror-del");

	QObject::connect(ui->add_pushButton, SIGNAL(clicked()), this, SIGNAL(signalAddMirror()));
    QObject::connect(ui->del_pushButton, SIGNAL(clicked()), this, SLOT(slotDelClicked()));
	QObject::connect(ui->auto_pushButton, SIGNAL(clicked()), this, SLOT(slotStartStopButtonClicked()));

	ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
	ui->auto_pushButton->style()->polish(ui->auto_pushButton);

    /* 将listWidget的事件注册到本窗口进行过滤鼠标点击事件 */
    ui->listWidget->installEventFilter(this);
}

void MirrorWidget::createData(int currentScreenID)
{
	ui->listWidget->clear();
	ui->del_pushButton->setEnabled(true);
	ui->auto_pushButton->setObjectName("mirror-start");
	//ui->listWidget->setEnabled(true);
    _isMirrorState = false;
	//mirrorId,status,mirrorName
	std::vector<std::tuple<int, std::string, int> > allMirror; 
	bool retCode = otsServer::DataSession::instance()->getMirrorByScreenID(currentScreenID, allMirror);
	for (int i = 0; i < allMirror.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem;
		int id = std::get<0>(allMirror[i]);
		QString name = std::get<1>(allMirror[i]).c_str();
		item->setData(Qt::UserRole, id);
		item->setData(Qt::DisplayRole, name);
		ui->listWidget->addItem(item);

		if (std::get<2>(allMirror[i]) == 1)
		{
			//ui->del_pushButton->setEnabled(false);
			ui->auto_pushButton->setObjectName("mirror-stop");
			ui->listWidget->setEnabled(false);
			ui->listWidget->setCurrentItem(item);
            _isMirrorState = true;
		}
	}
	ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
	ui->auto_pushButton->style()->polish(ui->auto_pushButton);
}

void MirrorWidget::addItem(int ID, QString strName)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setData(Qt::UserRole, ID);
	item->setData(Qt::DisplayRole, strName);
	ui->listWidget->insertItem(ui->listWidget->count(), item);
}

int MirrorWidget::getSelectedMirrorSchemeId()
{
	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item)
	{
		return item->data(Qt::UserRole).toInt();
	}
	return 0;
}

void MirrorWidget::delSelectedItem()
{
	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item)
	{
		ui->listWidget->takeItem(ui->listWidget->row(item));
		ui->listWidget->setCurrentItem(NULL);
        delete item;
	}
	this->update();
}

bool MirrorWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (ui->listWidget == watched)
    {
        if ((event->type() == QEvent::MouseButtonPress) && _isMirrorState)
        {
            QMouseEvent* pMouseEvent = (QMouseEvent*)(event);
            QPoint mousePoint = pMouseEvent->pos();
            QListWidgetItem* pMousePressItem = ui->listWidget->itemAt(mousePoint);
            if (pMousePressItem && (pMousePressItem != ui->listWidget->currentItem()))
            {
                otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Toggles Mirror Hint"), tr("Please turn off the current Mirror first!"), otsCommon::SCMessageBox::Information, this);
                warning_widget->exec();
                warning_widget->deleteLater();
                return true;
            }
        }
    }
    return false;
}

//大屏切换时，更新预案列表显示
void MirrorWidget::slotScreenChanged(int screenID)
{
    createData(screenID);
}

void MirrorWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
	QWidget::paintEvent(event);
}

void MirrorWidget::changeEvent(QEvent* event)
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

void MirrorWidget::slotStartStopButtonClicked()
{
    int item_count = ui->listWidget->count();

    if (item_count == 0)
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Mirror Hint"), tr("There is no Mirror to open on the current screen!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }

	QListWidgetItem* item = ui->listWidget->currentItem();
	if (!item)
	{
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Mirror Hint"), tr("Please select a Mirror first!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
		return;
	}
	int mirrorId = getSelectedMirrorSchemeId();
	QString objectNameText = ui->auto_pushButton->objectName();
	if (objectNameText == "mirror-start")
    {
        /** 判断是否可以开启镜像 */
        int total_startMirror = 0;
        if (!otsServer::DataSession::instance()->check_MirrorScreenStatus_byMirrorID(total_startMirror, mirrorId))
        {
            return;
        }

        if (total_startMirror > 0)
        {//该镜像中存在开启镜像的大屏
            otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Mirror Hint"), tr("There is a screen in the Mirror that opens the Mirror!"), otsCommon::SCMessageBox::Information, this);
            warning_widget->exec();
            warning_widget->deleteLater();
            return;
        }

        int total_startPoll = 0;
        if (!otsServer::DataSession::instance()->check_PollScreenStatus_byMirrorID(total_startPoll, mirrorId))
        {
            return;
        }

        if (total_startPoll > 1)
        {//该镜像中存在多个开启轮巡的大屏
            otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Open Mirror Hint"), tr("There are multiple screens in the Mirror that turn on the Poll!"), otsCommon::SCMessageBox::Information, this);
            warning_widget->exec();
            warning_widget->deleteLater();
            return;
        }

		//开启镜像通道，置灰删除按钮
		//ui->del_pushButton->setEnabled(false);
		ui->auto_pushButton->setObjectName("mirror-stop");
		ui->listWidget->setEnabled(false);
		if (mirrorId > 0)
		{
			otsServer::DataSession::instance()->setMirrorStatus(mirrorId, 1);
			_isMirrorState = true;
			emit signalApplyMirror(true, mirrorId);
		}
    }
	else if (objectNameText == "mirror-stop")
    {
		//关闭镜像通道，使能删除按钮
		//ui->del_pushButton->setEnabled(true);
		ui->auto_pushButton->setObjectName("mirror-start");
		ui->listWidget->setEnabled(true);
		otsServer::DataSession::instance()->setMirrorStatus(mirrorId, 0);
		_isMirrorState = false;
		emit signalApplyMirror(false, mirrorId);
        _isMirrorState = false;
    }

	ui->auto_pushButton->style()->unpolish(ui->auto_pushButton);
	ui->auto_pushButton->style()->polish(ui->auto_pushButton);
}


void MirrorWidget::slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous)
{
	_isChanged = true;
}

void MirrorWidget::slotListItemClicked(QListWidgetItem *current)
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

void MirrorWidget::slotDelClicked()
{
    if (_isMirrorState)
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Mirror Hint"), tr("Please turn off the current Mirror first!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
    int item_count = ui->listWidget->count();

    if (item_count == 0)
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Mirror Hint"), tr("There is no Mirror to delete on the current screen!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
    QListWidgetItem* pCurrentItem = ui->listWidget->currentItem();
    if (pCurrentItem)
    {
        emit signalDelMirror();
    }
    else
    {
        otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Mirror Hint"), tr("Please select a Mirror first!"), otsCommon::SCMessageBox::Information, this);
        warning_widget->exec();
        warning_widget->deleteLater();
        return;
    }
}

/******************单个镜像通道控件*********************/

//MirrorItem::MirrorItem(QWidget *parent):QRadioButton(parent)
//{
//
//}
