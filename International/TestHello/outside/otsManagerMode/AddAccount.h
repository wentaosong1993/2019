#pragma once

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QFrame>

extern QString g_userName; //�û�����
extern QString g_userPswd; //�û�����
extern QString g_userAcct; //��¼�˺�
extern int g_userType; //�˻����ͣ���ͨ�û�������Ա��
extern QString g_userId; //�û�ID

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
	void disposePersonSlot(); //��ӹ���Ա����ͨ�û�
	void refreshUserInfo(); //ˢ���û�����

protected:
	void paintEvent(QPaintEvent *event);
	
private:
	QPushButton *_addUser = nullptr; //�����û�
	QPushButton *_delUser = nullptr; //ɾ���û�
	QPushButton *_modUser = nullptr; //�޸��û���Ϣ
	QPushButton *_resetPassword = nullptr; //��������
	PropertyDialog * _popDialog = nullptr; //����
	QHBoxLayout *_pHLayout1;
	QVBoxLayout *_pVLayout1;
	QTableWidget *_userTableWidget = nullptr; 
};
