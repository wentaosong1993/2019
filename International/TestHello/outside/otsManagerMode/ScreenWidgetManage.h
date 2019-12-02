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

//大屏显示、编辑界面
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
		编辑数据后，提示拉取数据
	*/
	void updateScreenData();
public slots:
	/*
		处理编辑大屏槽
	*/
	void disposeScreenEditSlot();

	/*
		处理选中大屏槽
	*/
	void disposeSelectScreenSlot(QListWidgetItem *selectItem);

	/*
		拉取最新大屏数据
	*/
	void createData();

protected:
	void paintEvent(QPaintEvent *event);

private:

	QListWidget *_screenListWidget = nullptr;
	QPushButton *_addScreenButton = nullptr;
	QPushButton *_modScreenButton = nullptr;
	QPushButton *_delScreenButton = nullptr;
	EnrollDataDialog * _enrollDialog = nullptr; //弹框

private:
	QString _screenName = ""; /* <大屏名称>*/
	QString _screenId = ""; /* <大屏ID>*/
};

typedef struct
{
	QString _decoderName; //解码器名称
	QString _decoderId;
	int _decoderRow;
	int _decoderCol;
}DecorderInfo;

extern DecorderInfo _decoderInfo;
extern QList<DecorderInfo> _decoderList;

//录入大屏数据对话框
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
	QLineEdit * _screenNameLineEdit = nullptr; //大屏名称
	QLineEdit * _screenIdLineEdit = nullptr;  //大屏ID
	QLineEdit * _screenRowsLineEdit = nullptr; //大屏行数
	QLineEdit * _screenColsLineEdit = nullptr; //大屏列数
	QWidget * _screenDecoderWidget = nullptr; //大屏解码板组件

	QFormLayout * _formLayout = nullptr;
	QComboBox * _is4KCombobox = nullptr; //是否4k

	QPushButton * _okPushButton = nullptr;
	QPushButton * _cancelPushButton = nullptr;

	QString g_screenName = "";  //大屏名称
	int g_screenID = 0; //大屏ID
	int g_screenRow = 0; //大屏行
	int g_screenCol = 0; //大屏列
	int g_screenDecoderID = 0; //大屏类型
	int g_screenType = 0; //解码板是否支持4K
	int g_total = -1; //解码板总个数

	DecoderDialog * _decoderDialog = nullptr; //解码器参数对话框
};

//录入解码板数据对话框
class DecoderDialog : public QDialog
{
	Q_OBJECT
public:
	DecoderDialog(QWidget *parent = nullptr);
	~DecoderDialog();

	void initUI(DecorderInfo info);

	public slots:
	void disposeEditingFinishedSlot();
	void disposeButtonSlot(); ////处理按钮事件

public:
	QLineEdit *_decorderNameLineEdit = nullptr; //解码器名称
	QLineEdit *_decorderIdLineEdit = nullptr; //解码器ID
	QLineEdit *_decoderRowLineEdit = nullptr;  //解码器所在行
	QLineEdit *_decoderColLineEdit = nullptr;  //解码器所在列
	QFormLayout *_formLayout = nullptr;

	//底部按钮
	QPushButton *_okPushButton = nullptr; //确定按钮
	QPushButton *_cancelPushButton = nullptr; //取消按钮
};