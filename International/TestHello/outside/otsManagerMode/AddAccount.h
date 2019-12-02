#pragma once

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QFrame>

extern QString g_userName; //用户名称
extern QString g_userPswd; //用户密码
extern QString g_userAcct; //登录账号
extern int g_userType; //账户类型（普通用户、管理员）
extern QString g_userId; //用户ID

class PropertyDialog;

class AddAccountWidget : public QWidget
{
	Q_OBJECT
public:
	AddAccountWidget(QWidget *parent = Q_NULLPTR);
	~AddAccountWidget();
	void initUI();
	void initData();

protected:
	void changeEvent(QEvent *event);

signals:
	void getUserDataAgain();

public slots:
	void disposePersonSlot(); //添加管理员、普通用户
	void refreshUserInfo(); //刷新用户数据

protected:
	void paintEvent(QPaintEvent *event);
	
private:
	QPushButton *_addUser = nullptr; //增加用户
	QPushButton *_delUser = nullptr; //删除用户
	QPushButton *_modUser = nullptr; //修改用户信息
	QPushButton *_resetPassword = nullptr; //重置密码
	PropertyDialog * _popDialog = nullptr; //弹框
	QHBoxLayout *_pHLayout1;
	QVBoxLayout *_pVLayout1;
	QTableWidget *_userTableWidget = nullptr; 
};
