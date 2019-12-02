#include "signalgrouptree.h"
#include <QStyleOption>
#include <QPainter>
#include <QTreeWidgetItem>
#include <QDebug>
#include <QEvent>
#include <vector>
#include <tuple>
#include "otsServer/DataSession.h"
#include "otsControler/ControlerData.h"

SignalGroupTree::SignalGroupTree(QWidget *parent) : QTreeWidget(parent)
{
    _strListTreeNode.clear();
    this->setHeaderLabel(tr("Signal Group"));
	this->setHeaderHidden(true);
    QObject::connect(this, SIGNAL(itemSelectionChanged()),
                     this,SLOT(slotItemSelect()));
}

void SignalGroupTree::clearTreeData()
{
    _strListTreeNode.clear();
    this->clear();
}

bool SignalGroupTree::createTreeData()
{
    clearTreeData();
    /** ��0-Root��ʼ���ݹ���ȡ */
    int parentID = 0;
    QTreeWidgetItem* rootItem = new QTreeWidgetItem; //���ڵ�
    rootItem->setData(0, ID_Role, 0);
    rootItem->setData(0, ParentID_Role, 0);
    rootItem->setData(0, ItemType_Role, 0);
    rootItem->setData(0, Qt::DisplayRole, tr("Command Hall"));
    _strListTreeNode << tr("Command Hall");

	/*
	typedef struct node
	{
		std::tuple<int, std::string, int>  info; //������Ϣ ID ,NAME PARENT //ROOTΪ���ڵ� 
		std::vector<struct node> childnode; //�ӽڵ� 
		std::vector<struct node> leaf; //�����Ҷ�ӽڵ�
	}NODES;
	*/
    NODES nodes;
    otsServer::DataSession::instance()->Gen_Tree(parentID, nodes);
	/*��ȡ���ݣ������ź����νṹ*/
    Parse_Tree(rootItem, nodes);
    this->insertTopLevelItem(0, rootItem);
    this->expandItem(rootItem);
    return true;
}

void SignalGroupTree::Parse_Tree(QTreeWidgetItem* parentTreeItem, NODES& nodes)
{
    for (int i = 0; i < nodes.leaf.size(); i++)
    {
        otsControler::ControlerData::instance()->startPlayVideo(std::get<0>(nodes.leaf[i].info));
        QTreeWidgetItem* childItem = new QTreeWidgetItem;
        childItem->setData(0, ID_Role, std::get<0>(nodes.leaf[i].info)); //device ID 
        childItem->setData(0, ParentID_Role, std::get<2>(nodes.leaf[i].info)); //ParentId
		/*�ӽڵ�����*/
        childItem->setData(0, ItemType_Role, 1); 
        childItem->setData(0, Qt::DisplayRole, std::get<1>(nodes.leaf[i].info).c_str()); //device name
        _strListTreeNode << std::get<1>(nodes.leaf[i].info).c_str();
        //qDebug() << "name-1:" << std::get<1>(nodes.leaf[i].info).c_str();
        parentTreeItem->addChild(childItem);
    }
    for (int i = 0; i < nodes.childnode.size(); i++)
    {
        QTreeWidgetItem* childItem = new QTreeWidgetItem;
        childItem->setData(0, ID_Role, std::get<0>(nodes.childnode[i].info));
        childItem->setData(0, ParentID_Role, std::get<2>(nodes.childnode[i].info));
		/*���ڵ�����*/
        childItem->setData(0, ItemType_Role, 0); 
        childItem->setData(0, Qt::DisplayRole, std::get<1>(nodes.childnode[i].info).c_str());
        _strListTreeNode << std::get<1>(nodes.childnode[i].info).c_str();
        //qDebug() << "name-1:" << std::get<1>(nodes.childnode[i].info).c_str();

		/*�ӽڵ����ж����ӽڵ�ʱ���ݹ����*/
        Parse_Tree(childItem, nodes.childnode[i]); 
        parentTreeItem->addChild(childItem);
    }
}

void SignalGroupTree::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QTreeWidget::paintEvent(event);
}
void SignalGroupTree::changeEvent(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::LanguageChange:
	{
		this->setHeaderLabel(tr(this->headerItem()->text(0).toStdString().c_str()));
	}
	break;
	default:
		break;
	}
}


/*ѡ�����νṹ��ĳһ�ź�ʱ����<id, node type, name>*/
void SignalGroupTree::slotItemSelect()
{
    QTreeWidgetItem *item = this->currentItem();
    if (item)
    {
        int ID = item->data(0, ID_Role).toInt();
        int type = item->data(0, ItemType_Role).toInt();
        //int parentID = item->data(0, ParentID_Role).toInt();
        QString name = item->data(0, Qt::DisplayRole).toString();

        emit signalItemClicked(ID, type, name);
    }
}
