#pragma once

#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QMap>
#include <QEvent>
#include <QMouseEvent>
#include <QFrame>

class EnrollDataDialog;
class QFormLayout;
class DecoderDialog;

//������ʾ���༭����
class ScreenManageWidget : public QWidget
{
	Q_OBJECT
public:
	ScreenManageWidget(QWidget *parent = Q_NULLPTR);
	~ScreenManageWidget();
	void initUI();

protected:
	void changeEvent(QEvent *event);

public :
	QString getScreenName() { return _screenName; }
	void setScreenName(QString name = "") { _screenName = name; }
	QString getScreenID() { return _screenId; }
	void setScreenID(QString id) { _screenId = id; }

	void clearScreenInfoAndEmitUpdateSignal();

signals:
	/*
		�༭���ݺ���ʾ��ȡ����
	*/
	void updateScreenData();
public slots:
	/*
		����༭������
	*/
	void disposeScreenEditSlot();

	/*
		����ѡ�д�����
	*/
	void disposeSelectScreenSlot(QListWidgetItem *selectItem);

	/*
		��ȡ���´�������
	*/
	void createData();

protected:
	void paintEvent(QPaintEvent *event);

private:

	QListWidget *_screenListWidget = nullptr;
	QPushButton *_addScreenButton = nullptr;
	QPushButton *_modScreenButton = nullptr;
	QPushButton *_delScreenButton = nullptr;
	EnrollDataDialog * _enrollDialog = nullptr; //����

private:
	QString _screenName = ""; /* <��������>*/
	QString _screenId = ""; /* <����ID>*/
};

typedef struct
{
	QString _decoderName; //����������
	QString _decoderId;
	int _decoderRow;
	int _decoderCol;
}DecorderInfo;

extern DecorderInfo _decoderInfo;
extern QList<DecorderInfo> _decoderList;

//¼��������ݶԻ���
class EnrollDataDialog : public QDialog
{
	Q_OBJECT

public:
	EnrollDataDialog(QWidget *parent = Q_NULLPTR);
	void AddScreenDataUI();
	void ModScreenDataUI(QString screenId, QString screenName, int screenType);

	void setDecoderWidget(const int &nRow, const int &nCol);

signals:
	void okRelease();
	void cancelRelease();

	public slots:
	void disposeButtonSlot();
	void disposeEditingFinishedSlot();
	void disposeComboxIndexChangedSlot(int index = -1);
	void disposeGetDecoderInfo();

public:
	QLineEdit * _screenNameLineEdit = nullptr; //��������
	QLineEdit * _screenIdLineEdit = nullptr;  //����ID
	QLineEdit * _screenRowsLineEdit = nullptr; //��������
	QLineEdit * _screenColsLineEdit = nullptr; //��������
	QWidget * _screenDecoderWidget = nullptr; //������������

	QFormLayout * _formLayout = nullptr;
	QComboBox * _is4KCombobox = nullptr; //�Ƿ�4k

	QPushButton * _okPushButton = nullptr;
	QPushButton * _cancelPushButton = nullptr;

	QString g_screenName = "";  //��������
	int g_screenID = 0; //����ID
	int g_screenRow = 0; //������
	int g_screenCol = 0; //������
	int g_screenDecoderID = 0; //��������
	int g_screenType = 0; //������Ƿ�֧��4K
	int g_total = -1; //������ܸ���

	DecoderDialog * _decoderDialog = nullptr; //�����������Ի���
};

//¼���������ݶԻ���
class DecoderDialog : public QDialog
{
	Q_OBJECT
public:
	DecoderDialog(QWidget *parent = nullptr);
	~DecoderDialog();

	void initUI(DecorderInfo info);

	public slots:
	void disposeEditingFinishedSlot();
	void disposeButtonSlot(); ////����ť�¼�

public:
	QLineEdit *_decorderNameLineEdit = nullptr; //����������
	QLineEdit *_decorderIdLineEdit = nullptr; //������ID
	QLineEdit *_decoderRowLineEdit = nullptr;  //������������
	QLineEdit *_decoderColLineEdit = nullptr;  //������������
	QFormLayout *_formLayout = nullptr;

	//�ײ���ť
	QPushButton *_okPushButton = nullptr; //ȷ����ť
	QPushButton *_cancelPushButton = nullptr; //ȡ����ť
};