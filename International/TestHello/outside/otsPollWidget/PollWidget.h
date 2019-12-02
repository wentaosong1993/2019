#pragma once

#include <QWidget>
#include <QMap>
#include <QRadioButton>

class QPushButton;
class QScrollArea;
class PollItem;
class QListWidgetItem;

namespace Ui
{
    class ChannelWidget;
}

class PollWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PollWidget(QWidget *parent = 0);

    void createTestData();

    void addItem(int ID, QString strName);

	//获取当前选中的轮训预案的ID(轮训预ID > 0)
	int getSelectedPollId();
	void delSelectedItem();
	void setScreenId(int id) { _screenId = id; }

    /* 处理listWidget事件过滤 */
    virtual bool eventFilter(QObject *watched, QEvent *event);
protected:
	void changeEvent(QEvent *event);

signals:
    void signalPollGroupID(int groupID,bool start);
    void signalAddPoll();
	void signalDelPoll();

public slots:
    void slotScreenChanged(int screenID);

private slots:
    void slotStartStopButtonClicked();
	/*！
	镜像通道开启、关闭状态
	*/
	void slotSetMirrorState(bool state) { _mirrorApplyState = state; }
    /** 删除按钮点击处理 */
    void slotDelClicked();

private slots:
    void slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous);
    void slotListItemClicked(QListWidgetItem *current);

private:
    void createData(int currentScreenID);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::ChannelWidget* ui;
    bool _isChanged;
	bool _mirrorApplyState = false; /* <镜像通道开启、关闭状态>*/
	int _screenId = 0; /*当前大屏ID*/
};
