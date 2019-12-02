#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QPoint>

namespace Ui
{
    class ScreenLoginWidget;
}
class QTranslator;

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

	/*<获取登录的用户账户名称>*/
	QString getUserInfo() { return _userAccount; }
	void setUserAccount(QString userAccount) { _userAccount = userAccount; }

	int getUserType() { return _accountType; }
	void setUserType(int userType) { _accountType = userType; }

	void refreshLabel();
	void changeTr(const QString &langCode);

	typedef struct LoginInfo
	{
		QString userAccount; /*用户账户*/
		QString userKey;    /*用户密码*/
		bool rememberState; /*记住密码状态*/
		bool autoLoginState; /*自动登录状态*/
	};

protected: 
	void showEvent(QShowEvent *event);
	void timerEvent(QTimerEvent *event);
	void changeEvent(QEvent* event);

private slots:
	/*用户类型切换按钮*/
	void on_commonUserButton_clicked();
	void on_administratorButton_clicked();
	/*最小化、关闭按钮*/
	void on_loginMiniButton_clicked();
	void on_loginCloseButtton_clicked();
	/*登录按钮*/
	void on_administratorLoginButton_clicked();
	void on_commonUserLoginButton_clicked();
	void processLogin();
	void processLeftSeconds();

	void on_rememberKeyCheckBox_clicked();
	void on_autoLoginCheckBox_clicked();

	/*多语言切换*/
	void on_langTypeCombo_currentIndexChanged(int index);


private slots:
	/*普通用户拉取选择时切换账号和密码*/
	void slotCommonComboxIndexChanged(int index);

protected:
	Ui::ScreenLoginWidget *ui;
	QList<LoginInfo> _listLoginInfo; /*登录信息*/
	bool        m_bDrag;
	QPoint      m_DragPosition;
	QTimer *_pLoginTimer;
	QTimer *_leftSecondTimer;
	int _leftSeconds;
	QString _userAccount;
	int _accountType;
	QTranslator * _translator;
};