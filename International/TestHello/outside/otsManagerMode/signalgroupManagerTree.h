#ifndef SIGNALGROUPMANAGERTREE_H
#define SIGNALGROUPMANAGERTREE_H

#include <QTreeWidget>
#include "ots/Common.h"


////�ź�Դ���ơ��ź�Դ�����ơ�ID���Ƿ�4k����
extern QString _signalName;
extern QString _signalNewName;		 //�ź�Դ������
extern int _signalId;
extern int _typeIndex;

//�ź�Դ�����ơ��ź�Դ��������
extern QString _signalGroupName;	 //�ź�Դ������
extern QString _signalGroupNewName; //�ź�Դ��������


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
		ID_Role = Qt::UserRole + 1, /*<�ź�Դid>*/
		ParentID_Role,				/*<�ź�Դ��id---0�Լ����ݿ����ɵ�>*/
		ItemType_Role,				/*<�ڵ�����ID��0-��ڵ� 1-�ź�Դ�ڵ�>*/
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

