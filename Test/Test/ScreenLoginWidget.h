#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QPoint>

namespace Ui
{
    class ScreenLoginWidget;
}
class ScreenLoginWidget : public QWidget
{
	Q_OBJECT
public:
    ScreenLoginWidget(QWidget *parent = Q_NULLPTR);
	~ScreenLoginWidget();

	void initWindow();
	void initData();
	virtual bool eventFilter(QObject * watched, QEvent * event);
	void EnableLoginFrame(bool enable);
	void setErrorTip(const QString& sError, bool bLoginPwd = true);

	/*<��ȡ��¼���û��˻�����>*/
	QString getUserInfo() { return _userAccount; }
	void setUserAccount(QString userAccount) { _userAccount = userAccount; }

	int getUserType() { return _accountType; }
	void setUserType(int userType) { _accountType = userType; }

	typedef struct LoginInfo
	{
		QString userAccount; /*�û��˻�*/
		QString userKey;    /*�û�����*/
		bool rememberState; /*��ס����״̬*/
		bool autoLoginState; /*�Զ���¼״̬*/
	};

protected: 
	void showEvent(QShowEvent *event);
	void timerEvent(QTimerEvent *event);

private slots:
	/*�û������л���ť*/
	void on_commonUserButton_clicked();
	void on_administratorButton_clicked();
	/*��С�����رհ�ť*/
	void on_loginMiniButton_clicked();
	void on_loginCloseButtton_clicked();
	/*��¼��ť*/
	void on_administratorLoginButton_clicked();
	void on_commonUserLoginButton_clicked();
	void processLogin();
	void processLeftSeconds();

	void on_rememberKeyCheckBox_clicked();
	void on_autoLoginCheckBox_clicked();


private slots:
	/*��ͨ�û���ȡѡ��ʱ�л��˺ź�����*/
	void slotCommonComboxIndexChanged(int index);

protected:
	Ui::ScreenLoginWidget *ui;
	QList<LoginInfo> _listLoginInfo; /*��¼��Ϣ*/
	bool        m_bDrag;
	QPoint      m_DragPosition;
	QTimer *_pLoginTimer;
	QTimer *_leftSecondTimer;
	int _leftSeconds;
	QString _userAccount;
	int _accountType;
};