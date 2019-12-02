#pragma once

#include <QDialog>
#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;


class PropertyDialog : public QDialog
{
	Q_OBJECT
public:
	PropertyDialog(QWidget *parent = Q_NULLPTR);
	~PropertyDialog();

	/*-----------------信号源与信号源组相关-------------------*/
	void initSignalUi();//初始化信号源弹框
	void initSignalGroupUI();//初始化信号源组弹框
	void renameSignal(/*QString name = ""*/); //信号源重命名
	void renameSignalGroup(/*QString name = ""*/); //信号源组重命名

protected:
	void changeEvent(QEvent* event);

private slots:
	void disposeButtonSlot();//处理按钮事件
	void disposeEditingFinishedSlot(); //处理QLineEdit失去焦点事件
	void disposeComboxIndexChangedSlot(); //处理QCombox索引变化事件

public:
	///信号源
	QLineEdit *_signalNameLineEdit = nullptr; //信号源名称
	QLineEdit* _signalIdLineEdit = nullptr; //信号源ID
	QComboBox* _is4KCombox = nullptr; //是否支持4K 
	QLineEdit *_signalRenameLineEdit = nullptr; //重命名信号源名称

	//信号源组
	QLineEdit *_signalGroupNameLineEdit = nullptr; //信号源组名称
	QLineEdit *_signalGroupRenameLineEdit = nullptr; //重命名信号源组名称

	//底部按钮
	QPushButton *_okPushButton = nullptr; //确定按钮
	QPushButton *_cancelPushButton = nullptr; //取消按钮


	/*-----------------编辑人员信息*---------------------------*/
public:
	void addPersonnelUI();
	void delPersonnelUI(QString userId = "");
	void modPersonnelUI(QString userName = "", QString userId = "", int userType = 0, QString userAccount = "", QString userPassword = "");
	void resetPasswordUI();

//signals:
//	void getUserDataAgain(); //编辑人员后，再次拉取数据

private slots:
	void disposePersonnelButtonSlot(); //处理按钮事件
	void disposePersonnelComboxIndexChangedSlot(int index);

public:
	//用户名字, 用户类型，用户密码, 用户账号>
	QLineEdit * _userNameLineEdit = nullptr;
	QLineEdit * _userAccLineEdit = nullptr;
	QLineEdit * _userPsLineEdit = nullptr;
	QComboBox * _userTypeCombox = nullptr;
	QLineEdit * _userIdLineEdit = nullptr;
	QLineEdit * _resetPasswordLineEdit = nullptr;
};