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

	//��ȡ��ǰѡ�еľ���ͨ���ŷ���ID(ID > 0)
	int getSelectedMirrorSchemeId();
	void delSelectedItem();
	bool getMirrorState() { return _isMirrorState; }

    /* ����listWidget�¼����� */
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
    /** ���ɾ������ť��Ӧ */
    void slotDelClicked();

private:
    void createData(int currentScreenID);

private:
    int _selectItemID = 0;/**< ѡ�е�ItemID */
	//QMap<int, MirrorItem* > _mapSeletedItem; //ѡ�е���ѯԤ��ID��Item
    bool _isSelectItem;/**< �Ƿ�ѡ��Item */
	Ui::ChannelWidget* ui;
	bool _isChanged;
	bool _isMirrorState = false; /*����ͨ���������ر�״̬*/
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
//    int _id;/**< ���񷽰�ID */
//};
