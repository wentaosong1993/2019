#pragma once

#include <QWidget>
#include <QMap>
#include <QRadioButton>
#include <QListWidgetItem>

class QPushButton;
class QScrollArea;
//class MirrorItem;

namespace Ui
{
	class ChannelWidget;
}

class MirrorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MirrorWidget(QWidget *parent = 0);

    void addItem(int ID, QString strName);

	//获取当前选中的镜像通道放方案ID(ID > 0)
	int getSelectedMirrorSchemeId();
	void delSelectedItem();
	bool getMirrorState() { return _isMirrorState; }

    /* 处理listWidget事件过滤 */
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void signalPollGroupID(int groupID,bool start);
    void signalAddMirror();
	void signalDelMirror();
	void signalApplyMirror(bool state, int mirrorId);

public slots:
    void slotScreenChanged(int screenID);

protected:
	void paintEvent(QPaintEvent *event);
	void changeEvent(QEvent *event);


private slots:
    void slotStartStopButtonClicked();
	void slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous);
	void slotListItemClicked(QListWidgetItem *current);
    /** 点击删除镜像按钮响应 */
    void slotDelClicked();

private:
    void createData(int currentScreenID);

private:
    int _selectItemID = 0;/**< 选中的ItemID */
	//QMap<int, MirrorItem* > _mapSeletedItem; //选中的轮询预案ID和Item
    bool _isSelectItem;/**< 是否选中Item */
	Ui::ChannelWidget* ui;
	bool _isChanged;
	bool _isMirrorState = false; /*镜像通道开启、关闭状态*/
};

//class MirrorItem :public QRadioButton
//{
//    Q_OBJECT
//public:
//    explicit MirrorItem(QWidget *parent = 0);
//
//    void setID(int id){ _id = id; }
//    int getID(){ return _id; }
//
//private:
//    int _id;/**< 镜像方案ID */
//};
