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

	//��ȡ��ǰѡ�е���ѵԤ����ID(��ѵԤID > 0)
	int getSelectedPollId();
	void delSelectedItem();
	void setScreenId(int id) { _screenId = id; }

    /* ����listWidget�¼����� */
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
	/*��
	����ͨ���������ر�״̬
	*/
	void slotSetMirrorState(bool state) { _mirrorApplyState = state; }
    /** ɾ����ť������� */
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
	bool _mirrorApplyState = false; /* <����ͨ���������ر�״̬>*/
	int _screenId = 0; /*��ǰ����ID*/
};
