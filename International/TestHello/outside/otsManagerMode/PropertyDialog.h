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

	/*-----------------�ź�Դ���ź�Դ�����-------------------*/
	void initSignalUi();//��ʼ���ź�Դ����
	void initSignalGroupUI();//��ʼ���ź�Դ�鵯��
	void renameSignal(/*QString name = ""*/); //�ź�Դ������
	void renameSignalGroup(/*QString name = ""*/); //�ź�Դ��������

protected:
	void changeEvent(QEvent* event);

private slots:
	void disposeButtonSlot();//����ť�¼�
	void disposeEditingFinishedSlot(); //����QLineEditʧȥ�����¼�
	void disposeComboxIndexChangedSlot(); //����QCombox�����仯�¼�

public:
	///�ź�Դ
	QLineEdit *_signalNameLineEdit = nullptr; //�ź�Դ����
	QLineEdit* _signalIdLineEdit = nullptr; //�ź�ԴID
	QComboBox* _is4KCombox = nullptr; //�Ƿ�֧��4K 
	QLineEdit *_signalRenameLineEdit = nullptr; //�������ź�Դ����

	//�ź�Դ��
	QLineEdit *_signalGroupNameLineEdit = nullptr; //�ź�Դ������
	QLineEdit *_signalGroupRenameLineEdit = nullptr; //�������ź�Դ������

	//�ײ���ť
	QPushButton *_okPushButton = nullptr; //ȷ����ť
	QPushButton *_cancelPushButton = nullptr; //ȡ����ť


	/*-----------------�༭��Ա��Ϣ*---------------------------*/
public:
	void addPersonnelUI();
	void delPersonnelUI(QString userId = "");
	void modPersonnelUI(QString userName = "", QString userId = "", int userType = 0, QString userAccount = "", QString userPassword = "");
	void resetPasswordUI();

//signals:
//	void getUserDataAgain(); //�༭��Ա���ٴ���ȡ����

private slots:
	void disposePersonnelButtonSlot(); //����ť�¼�
	void disposePersonnelComboxIndexChangedSlot(int index);

public:
	//�û�����, �û����ͣ��û�����, �û��˺�>
	QLineEdit * _userNameLineEdit = nullptr;
	QLineEdit * _userAccLineEdit = nullptr;
	QLineEdit * _userPsLineEdit = nullptr;
	QComboBox * _userTypeCombox = nullptr;
	QLineEdit * _userIdLineEdit = nullptr;
	QLineEdit * _resetPasswordLineEdit = nullptr;
};