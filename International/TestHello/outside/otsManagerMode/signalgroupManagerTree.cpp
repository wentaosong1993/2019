#include "signalgroupManagerTree.h"
#include <QStyleOption>
#include <QPainter>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QMenu>
#include <vector>
#include <tuple>
#include <QLineEdit>
#include <QComboBox>
#include "otsServer/DataSession.h"
#include "otsControler/ControlerData.h"
#include "PropertyDialog.h"
#include <QMessageBox>
#include "otsCommon/MessageBox.h"

using namespace otsCommon;

SignalGroupManagerTree::SignalGroupManagerTree(QWidget *parent) : QTreeWidget(parent)
{
    this->setHeaderLabel(tr("Signal Group"));
	this->setHeaderHidden(true);
	this->setMouseTracking(true);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, SIGNAL(itemSelectionChanged()),this,SLOT(slotItemSelect()));
	/*<信号的参数以及参数的数据类型要保持跟帮助文档一致>*/
	QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showRightMenu(/*const QPoint &*/)));
}

void SignalGroupManagerTree::clearTreeData()
{
    this->clear();
}

bool SignalGroupManagerTree::createTreeData()
{
    clearTreeData();
    /** 从0-Root开始，递归拉取 */
    int parentID = 0;
    QTreeWidgetItem* rootItem = new QTreeWidgetItem;
    rootItem->setData(0, ID_Role, 0);
    rootItem->setData(0, ParentID_Role, 0);
    rootItem->setData(0, ItemType_Role, 0);
    rootItem->setData(0, Qt::DisplayRole, tr("Command Hall "));

    NODES nodes;
    otsServer::DataSession::instance()->Gen_Tree(parentID, nodes);
    Parse_Tree(rootItem, nodes);
    this->insertTopLevelItem(0, rootItem);
    return true;
}
//ID, UniqueName,Parents 
void SignalGroupManagerTree::Parse_Tree(QTreeWidgetItem* parentTreeItem, NODES& nodes)
{
    for (int i = 0; i < nodes.leaf.size(); i++)
    {
        otsControler::ControlerData::instance()->startPlayVideo(std::get<0>(nodes.leaf[i].info));
        QTreeWidgetItem* childItem = new QTreeWidgetItem;
        childItem->setData(0, ID_Role, std::get<0>(nodes.leaf[i].info));
        childItem->setData(0, ParentID_Role, std::get<2>(nodes.leaf[i].info));
        childItem->setData(0, ItemType_Role, 1);//代表节点类型：0-组节点 1-信号源节点；
        childItem->setData(0, Qt::DisplayRole, std::get<1>(nodes.leaf[i].info).c_str());
        qDebug() << "name-1:" << std::get<1>(nodes.leaf[i].info).c_str();
        parentTreeItem->addChild(childItem);
    }
    for (int i = 0; i < nodes.childnode.size(); i++)
    {
        QTreeWidgetItem* childItem = new QTreeWidgetItem;
        childItem->setData(0, ID_Role, std::get<0>(nodes.childnode[i].info));
        childItem->setData(0, ParentID_Role, std::get<2>(nodes.childnode[i].info));
        childItem->setData(0, ItemType_Role, 0);//代表节点类型：0-组节点 1-信号源节点；
        childItem->setData(0, Qt::DisplayRole, std::get<1>(nodes.childnode[i].info).c_str());
        qDebug() << "name-1:" << std::get<1>(nodes.childnode[i].info).c_str();
        Parse_Tree(childItem, nodes.childnode[i]);
        parentTreeItem->addChild(childItem);
    }
}

void SignalGroupManagerTree::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QTreeWidget::paintEvent(event);
}

void SignalGroupManagerTree::slotItemSelect()
{
    QTreeWidgetItem *item = this->currentItem();
    int ID = item->data(0, ID_Role).toInt();
    int type = item->data(0, ItemType_Role).toInt();
    int parentID = item->data(0, ParentID_Role).toInt();
    QString name = item->data(0, Qt::DisplayRole).toString();

	//qInfo() << "item clicked";
	emit signalItemClicked(ID, type, name, parentID);
}


void SignalGroupManagerTree::showRightMenu(/*const QPoint &p*/)
{
	qInfo() << "showRightMenu";
	QMenu *menu = new QMenu(this);
	menu->setObjectName("signalGroupMenu");
	//menu->style()->unpolish(menu);
	//menu->style()->polish(menu);

	int type = this->currentItem()->data(0, ItemType_Role).toInt();
	if (type == 0)
	{
		QAction *addGrpAction = new QAction(tr("Add Signal Group"), this);
		QAction *delGrpAction = new QAction(tr("Delete Signal Group"), this);
		QAction *modGrpAction = new QAction(tr("Rename Signal Group"), this);//重命名
		menu->addSeparator();
		QAction *addAction = new QAction(tr("Add Signal"), this);

		menu->addAction(addGrpAction);
		menu->addAction(delGrpAction);
		menu->addAction(modGrpAction);
		menu->addAction(addAction);
		addGrpAction->setObjectName("addGrpAction");
		delGrpAction->setObjectName("delGrpAction");
		modGrpAction->setObjectName("modGrpAction");
		addAction->setObjectName("addAction");

		//处理鼠标右键事件
		QObject::connect(addGrpAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
		QObject::connect(delGrpAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
		QObject::connect(modGrpAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
		QObject::connect(addAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
	}
	else if (type == 1)
	{
		//QAction *addAction = new QAction("add user", this);
		QAction *delAction = new QAction(tr("Delete Signal"), this);
		QAction *modAction = new QAction(tr("Rename Signal"), this);//重命名
		//menu->addAction(addAction);
		menu->addAction(delAction);
		menu->addAction(modAction);
		//处理鼠标右键事件
		//信号源不能右键添加信号源
		//QObject::connect(addAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
		QObject::connect(delAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
		QObject::connect(modAction, SIGNAL(triggered()), this, SLOT(disposeRightMenuSlot()));
	}
	else 
	{
		qInfo() << "nothing happened";
	}
	menu->exec(QCursor::pos());
}

void SignalGroupManagerTree::disposeRightMenuSlot()
{
	QAction *pSender = dynamic_cast<QAction*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is null";
		return;
	}
	QTreeWidgetItem *selItem = this->currentItem();
	int ID = selItem->data(0, ID_Role).toInt(); //节点ID
	int parentID = selItem->data(0, ParentID_Role).toInt(); //父节点ID
	int type = selItem->data(0, ItemType_Role).toInt(); //代表节点类型：0-组节点 1-信号源节点；
	QString name = selItem->data(0, Qt::DisplayRole).toString(); //节点名称

	QString actionStr = pSender->text();
	int column = this->currentColumn();
	int index = this->indexOfTopLevelItem(selItem);

	QString curText = this->currentItem()->text(column);

	if (actionStr == tr("Add Signal Group"))
	{
		PropertyDialog *_popDialog = new PropertyDialog(this);
		_popDialog->setObjectName("add-signalGroup-Dialog");
		_popDialog->initSignalGroupUI();
		int ret = _popDialog->exec();
		QString groupName = _signalGroupName;

		if (ret == QDialog::Accepted)
		{
			int id = 0;
			std::tuple<std::string, int> groupInfo;
			groupInfo = std::make_tuple(groupName.toStdString(), ID);
			int retCode = otsServer::DataSession::instance()->addDeviceGroup(id, groupInfo);

			if (1 == retCode)
			{
				qInfo() << QStringLiteral("add signalGroup data success！！！");
				QTreeWidgetItem *node = new QTreeWidgetItem;
				node->setData(0, ID_Role, id);//数据库返回ID给我
				node->setData(0, ParentID_Role, ID);
				node->setData(0, ItemType_Role, 0);//代表节点类型：0-组节点 1-信号源节点；
				node->setData(0, Qt::DisplayRole, groupName);//信号源组名称
				//if (selItem->parent())
				//{
				//	selItem->parent()->addChild(node);
				//}
				//else
				//{
				//	selItem->addChild(node);
				//}
				selItem->addChild(node);
			}
			else if (-99 == retCode)
			{
				qInfo() << QStringLiteral("add signalGroup data repeated！！！");
				SCMessageBox* signalGroupRepeatDlg = new SCMessageBox(tr("Add Signal Group Information Repeated Hint"), tr("Please verify that the signal group name is repeated!"), otsCommon::SCMessageBox::Information, this);
				signalGroupRepeatDlg->exec();
				signalGroupRepeatDlg->deleteLater();
				signalGroupRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << QStringLiteral("add signalGroup data failed！！！");
				SCMessageBox* signalGroupFailDlg = new SCMessageBox(tr("Add Signal Group Failed Hint"), tr("Sorry,Please add signal group again!"), otsCommon::SCMessageBox::Information, this);
				signalGroupFailDlg->exec();
				signalGroupFailDlg->deleteLater();
				signalGroupFailDlg = nullptr;
			}
		}
		qInfo() << "add group";
		if (_popDialog)
		{
			_popDialog->deleteLater();
			_popDialog = nullptr;
		}
	}
	else if (actionStr == tr("Delete Signal Group"))
	{
		if (ID == 0)
		{
			SCMessageBox* myBox = new SCMessageBox(tr("Delete Signal Group Hint"), tr("Root node can't be deleted!"), otsCommon::SCMessageBox::Information, this);
			myBox->exec();
			myBox->deleteLater();
			myBox = nullptr;
			return;
		}

		SCMessageBox* myBox1 = new SCMessageBox(tr("Delete Signal Group Hint"), tr("Are you sure to delete current signal group?"), otsCommon::SCMessageBox::Warning, this);
		myBox1->exec();

		if (myBox1->result() == QDialog::Accepted)
		{
			int retCode = otsServer::DataSession::instance()->delDeviceGroup(ID);
			if (1 == retCode)
			{
				//删除成功时，更新页面数据
				if (selItem->parent())
				{
					//有父节点时
					delete selItem->parent()->takeChild(this->currentIndex().row());
				}
				else
				{
					//NUll,没有父节点时
					delete this->takeTopLevelItem(this->currentIndex().row());
				}
				qInfo() << QStringLiteral("删除组成功！！！");
			}
			qInfo() << "del group";
		}
		myBox1->deleteLater();
		myBox1 = nullptr;
	}
	else if (actionStr == tr("Rename Signal Group"))
	{
		PropertyDialog * _popDialog = new PropertyDialog(this);
		_popDialog->setObjectName("mod-signalGroup-Dialog");
		_popDialog->renameSignalGroup();
		_popDialog->_signalGroupRenameLineEdit->setText(curText);
		int ret = _popDialog->exec();
		if (ret == QDialog::Accepted)
		{
			QString groupName = _signalGroupNewName;
			int retCode = otsServer::DataSession::instance()->setDeviceGroupName(ID, groupName.toStdString());
			if (1 == retCode)
			{
				selItem->setData(0, ID_Role, ID);//数据库返回ID给我
				selItem->setData(0, ParentID_Role, parentID);
				selItem->setData(0, ItemType_Role, 0);//代表节点类型：0-组节点 1-信号源节点；
				selItem->setData(0, Qt::DisplayRole, groupName);
				qInfo() << QStringLiteral("rename signalGroup data repeated！！！");
			}
			else if (-99 == retCode)
			{
				qInfo() << QStringLiteral("rename signalGroup data repeated！！！");
				SCMessageBox* signalGroupRepeatDlg = new SCMessageBox(tr("Rename Signal Group Repeated Hint"), tr("Please confirm the signal group name is duplicate or not!"), otsCommon::SCMessageBox::Information, this);
				signalGroupRepeatDlg->exec();
				signalGroupRepeatDlg->deleteLater();
				signalGroupRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << QStringLiteral("rename signalGroup data failed！！！");
				SCMessageBox* signalGroupFailDlg = new SCMessageBox(tr("Rename Signal Group Failed Hint"), tr("Sorry,please modify signal group information again!"), otsCommon::SCMessageBox::Information, this);
				signalGroupFailDlg->exec();
				signalGroupFailDlg->deleteLater();
				signalGroupFailDlg = nullptr;
			}
		}
		if (_popDialog)
		{
			_popDialog->deleteLater();
			_popDialog = nullptr;
		}
	}
	else if (actionStr == tr("Add Signal"))
	{
		PropertyDialog * _popDialog = new PropertyDialog(this);
		_popDialog->setObjectName("add-signal-Dialog");
		_popDialog->initSignalUi();
		int ret = _popDialog->exec();
		if (ret == QDialog::Accepted)
		{
			//获取信息
			QString encoderName = _signalName;
			int encoderId = _signalId;
			int is4KEncoder = _typeIndex;

			if (!encoderName.isEmpty() && encoderId > 0)
			{
				std::tuple<int, std::string, int, int > encoderInfo;
				encoderInfo = std::make_tuple(encoderId, encoderName.toStdString(), ID, is4KEncoder);
				int retCode = otsServer::DataSession::instance()->addDevice(encoderInfo);
				if (1 == retCode)
				{
					qInfo() << QStringLiteral("add signal data success！！！");
					QTreeWidgetItem *node = new QTreeWidgetItem;
					node->setData(0, ID_Role, encoderId);
					node->setData(0, ParentID_Role, ID);
					node->setData(0, ItemType_Role, 1);//代表节点类型：0-组节点 1-信号源节点；
					node->setData(0, Qt::DisplayRole, encoderName);
					selItem->addChild(node);
				}
				else if (-99 == retCode)
				{
					qInfo() << QStringLiteral("add signal data repeated！！！");
					SCMessageBox* signalRepeatDlg = new SCMessageBox(tr("Add Signal Information Repeated Hint"), tr("Please make sure signal name is repeated or not!"), otsCommon::SCMessageBox::Information, this);
					signalRepeatDlg->exec();
					signalRepeatDlg->deleteLater();
					signalRepeatDlg = nullptr;
				}
				else
				{
					qInfo() << QStringLiteral("add signal data failed！！！");
					SCMessageBox* signalFailDlg = new SCMessageBox(tr("Add Signal Information Failed Hint"), tr("Sorry,please add signal again!"), otsCommon::SCMessageBox::Information, this);
					signalFailDlg->exec();
					signalFailDlg->deleteLater();
					signalFailDlg = nullptr;
				}
			}
		}
		if (_popDialog)
		{
			_popDialog->deleteLater();
			_popDialog = nullptr;
		}
	}
	else if (actionStr == tr("Delete Signal"))
	{
		SCMessageBox* myBox2 = new SCMessageBox(tr("Delete Signal Hint"), tr("Are you sure to delete current signal?"), otsCommon::SCMessageBox::Warning, this);
		myBox2->exec();

		if (myBox2->result() == QDialog::Accepted)
		{

			int retCode = otsServer::DataSession::instance()->delDevice(ID);
			if (retCode)
			{
				if (selItem->parent())
				{
					delete selItem;
					selItem = nullptr;
				}
				qInfo() << QStringLiteral("删除编码板成功！！！");
			}
		}
		myBox2->deleteLater();
		myBox2 = nullptr;
	}
	else if (actionStr == tr("Rename Signal"))
	{
		PropertyDialog *_popDialog = new PropertyDialog(this);
		_popDialog->setObjectName("reName-signal-Dialog");
		_popDialog->renameSignal();
		_popDialog->_signalRenameLineEdit->setText(curText);
		int ret = _popDialog->exec();
		if (ret == QDialog::Accepted)
		{
			std::tuple<int, std::string, int, int > info;
			otsServer::DataSession::instance()->getDeviceByID(ID, info);
			int encoderId = 0; 
			std::string  encoderName = "";
			int parentId = 0;
			int is4KType = -1;
			std::tie(encoderId, encoderName, parentId, is4KType) = info;

			QString encoderNewName = _signalNewName;

			//从数据拉取其他信号，这里只修改名称
			std::tuple<int, std::string, int, int > encoderInfo;
			encoderInfo = std::make_tuple(encoderId, encoderNewName.toStdString(), parentId, is4KType); //<设备ID, 设备名称, 组ID, 是否4K>
			int retCode = otsServer::DataSession::instance()->setDeviceByID(encoderId, encoderInfo);
			if (1 == retCode)
			{
				//怎么修改设备信息有两者接口
				selItem->setData(0, ID_Role, encoderId);
				selItem->setData(0, ParentID_Role, parentID);
				selItem->setData(0, ItemType_Role, 1);//代表节点类型：0-组节点 1-信号源节点；
				selItem->setData(0, Qt::DisplayRole, encoderNewName);
				qInfo() << QStringLiteral("rename signalGroup data success！！！");
			}
			else if (-99 == retCode)
			{
				qInfo() << QStringLiteral("rename signalGroup data repeated！！！");
				SCMessageBox* signalRepeatDlg = new SCMessageBox(tr("Rename Signal Information Repeated Hint"), tr("Please make sure signal name is repeated or not!"), otsCommon::SCMessageBox::Information, this);
				signalRepeatDlg->exec();
				signalRepeatDlg->deleteLater();
				signalRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << QStringLiteral("rename signalGroup data failed！！！");
				SCMessageBox* signalFailDlg = new SCMessageBox(tr("Rename Signal Failed Hint"), tr("Sorry,please modify signal information again!"), otsCommon::SCMessageBox::Information, this);
				signalFailDlg->exec();
				signalFailDlg->deleteLater();
				signalFailDlg = nullptr;
			}
		}
		if (_popDialog)
		{
			_popDialog->deleteLater();
			_popDialog = nullptr;
		}
	}
}