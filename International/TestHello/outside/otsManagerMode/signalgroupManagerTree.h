#ifndef SIGNALGROUPMANAGERTREE_H
#define SIGNALGROUPMANAGERTREE_H

#include <QTreeWidget>
#include "ots/Common.h"


////信号源名称、信号源新名称、ID、是否4k索引
extern QString _signalName;
extern QString _signalNewName;		 //信号源新名称
extern int _signalId;
extern int _typeIndex;

//信号源组名称、信号源组新名称
extern QString _signalGroupName;	 //信号源组名称
extern QString _signalGroupNewName; //信号源组新名称


class QTreeWidgetItem;
class PropertyDialog;

class SignalGroupManagerTree : public QTreeWidget
{
	Q_OBJECT
public:
	explicit SignalGroupManagerTree(QWidget *parent = Q_NULLPTR);

	bool createTreeData();

	void setParentId(int parentId);

signals:
	void signalItemClicked(int ID, int type, QString name, int parentId);

private:
	void clearTreeData();
	void Parse_Tree(QTreeWidgetItem* parentTreeItem, NODES& nodes);

private:
	enum Item_Role
	{
		ID_Role = Qt::UserRole + 1, /*<信号源id>*/
		ParentID_Role,				/*<信号源组id---0以及数据库生成的>*/
		ItemType_Role,				/*<节点类型ID，0-组节点 1-信号源节点>*/
	};

protected:
	void paintEvent(QPaintEvent *event);

	public slots:
	void slotItemSelect();
	void showRightMenu(/*const QPoint &p*/);//right button menu
	void disposeRightMenuSlot();

private:
	int _parentId;
};
#endif // SIGNALGROUPMANAGERTREE_H

