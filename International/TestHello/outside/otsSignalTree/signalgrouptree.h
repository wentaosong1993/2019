#ifndef SIGNALGROUPTREE_H
#define SIGNALGROUPTREE_H

#include <QTreeWidget>
#include "ots/Common.h"
#include <QStringList>

class QTreeWidgetItem;
class SignalGroupTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SignalGroupTree(QWidget *parent = Q_NULLPTR);

    bool createTreeData();
    const QStringList& getAllTreeNodeName(){ return _strListTreeNode; }

signals:
    void signalItemClicked(int ID, int type, QString name);

private:
    void clearTreeData();
    void Parse_Tree(QTreeWidgetItem* parentTreeItem, NODES& nodes);

private:
    enum Item_Role
    {
        ID_Role = Qt::UserRole+1,
        ParentID_Role,
        ItemType_Role,
    };

protected:
    void paintEvent(QPaintEvent *event);
	void changeEvent(QEvent* event);

private slots:
    void slotItemSelect();

private:
    QStringList _strListTreeNode;/**< 保存树状图所有节点名称 */
};

#endif // SIGNALGROUPTREE_H
