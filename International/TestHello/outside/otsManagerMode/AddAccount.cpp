#include "AddAccount.h"
#include <QDebug>
#include <vector>
#include <tuple>
#include "otsServer/DataSession.h"
#include "PropertyDialog.h"
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QPainter>
#include <QMessageBox>
#include <QEvent>
#include "otsCommon/MessageBox.h"
#include "otsScreenLogin/globalApi.h"

QString __userAccountName = "";
int __userType = 0;

using namespace otsCommon;

AddAccountWidget::AddAccountWidget(QWidget *parent)
	:QWidget(parent)
{
	initUI();
	//initData();
	connect(_addUser, SIGNAL(clicked()), this, SLOT(disposePersonSlot()));
	connect(_delUser, SIGNAL(clicked()), this, SLOT(disposePersonSlot()));
	connect(_modUser, SIGNAL(clicked()), this, SLOT(disposePersonSlot()));
	connect(_resetPassword, SIGNAL(clicked()), this, SLOT(disposePersonSlot()));
}

AddAccountWidget::~AddAccountWidget()
{
}

void AddAccountWidget::initUI()
{
	_userTableWidget = new QTableWidget(this);
	_userTableWidget->setObjectName("userTableWidget");
	_userTableWidget->setColumnCount(5);
	_userTableWidget->verticalHeader()->setVisible(false);
	_userTableWidget->horizontalHeader()->setVisible(true);
	_userTableWidget->setHorizontalHeaderLabels(QStringList() << tr("User Name") << tr("User Type") << tr("User ID") << tr("User Account") << tr("User Password"));
	_userTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	_userTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	_userTableWidget->setGridStyle(Qt::SolidLine);
	_userTableWidget->setShowGrid(true);
	_userTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_userTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	_userTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_userTableWidget->horizontalHeader()->setSectionsClickable(false);
	_userTableWidget->verticalHeader()->setSectionsClickable(false);

	_pVLayout1 = new QVBoxLayout;
	_pVLayout1->setContentsMargins(20, 20, 20, 0);

	_pHLayout1 = new QHBoxLayout;
	_pHLayout1->addSpacing(20);

	_addUser = new QPushButton(tr("Add User"), this);
	_delUser = new QPushButton(tr("Delete User"), this);
	_modUser = new QPushButton(tr("Modify User"), this);
	_resetPassword = new QPushButton(tr("Modify Password"), this);
	_addUser->setObjectName("addUser");
	_delUser->setObjectName("delUser");
	_modUser->setObjectName("modUser");
	_resetPassword->setObjectName("resetPassword");

	_pHLayout1->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	_pHLayout1->addWidget(_addUser);
	_pHLayout1->addWidget(_delUser);
	_pHLayout1->addWidget(_modUser);
	_pHLayout1->addWidget(_resetPassword);
	_pHLayout1->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));

	_pVLayout1->addWidget(_userTableWidget);
	_pVLayout1->addLayout(_pHLayout1);

	this->setLayout(_pVLayout1);
}

void AddAccountWidget::initData()
{
	//获取用户数据
	refreshUserInfo();
}

void AddAccountWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_addUser->setText(_addUser->text());
			_delUser->setText(_delUser->text());
			_modUser->setText(_modUser->text());
			_resetPassword->setText(_resetPassword->text());
		}
		break;
	default:
		break;
	}
}


void AddAccountWidget::disposePersonSlot()
{
	QPushButton *pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	_popDialog = new PropertyDialog(this);
	connect(this, SIGNAL(getUserDataAgain()), this, SLOT(refreshUserInfo()));

	if (pSender == _addUser)
	{
		if (_popDialog == nullptr)
		{
			qDebug() << "_popDialog is nullptr";
			return;
		}
		_popDialog->setObjectName("add-propertyDialog");
		_popDialog->addPersonnelUI();
		int ret = _popDialog->exec();
		int userId = -1; 

		if (ret == QDialog::Accepted)
		{
			std::tuple<std::string, int, std::string, std::string> userInfo;
			userInfo = std::make_tuple(g_userName.toStdString(), g_userType, g_userPswd.toStdString(), g_userAcct.toStdString());
			int retCode = otsServer::DataSession::instance()->addUser(userId, userInfo);
			if (1 == retCode)
			{
				qInfo() << "add User success!!!";
				emit getUserDataAgain();
			}
			else if (-99 == retCode)
			{
				//重复
				qInfo() << "add repeated user data!!!";
				SCMessageBox* addUserRepeatDlg = new SCMessageBox(tr("Add User Repeated Hint"), tr("Please make sure that the user information is duplicated!"), otsCommon::SCMessageBox::Information, this);
				addUserRepeatDlg->exec();
				addUserRepeatDlg->deleteLater();
				addUserRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << "add user data failed !!!";
				SCMessageBox* addUserFailDlg = new SCMessageBox(tr("Add User Failed Hint"), tr("Sorry,please add user again!"), otsCommon::SCMessageBox::Information, this);
				addUserFailDlg->exec();
				addUserFailDlg->deleteLater();
				addUserFailDlg = nullptr;
			}
		}
	}
	else if (pSender == _delUser)
	{
		if (_popDialog == nullptr)
		{
			qDebug() << "_popDialog is nullptr";
			return;
		}
		//必须选中item才能弹框编辑
		QTableWidgetItem* item = _userTableWidget->currentItem();
		if (!item)
		{
			SCMessageBox* myBox0 = new SCMessageBox(tr("Delete User Hint"), tr("Please select the user you want to delete from the table above!"), otsCommon::SCMessageBox::Information, this);
			myBox0->exec();
			myBox0->deleteLater();
			myBox0 = nullptr;
			if (_popDialog)
			{
				_popDialog->deleteLater();
				_popDialog = nullptr;
			}
			return;
		}

		int item_row = _userTableWidget->row(item);
		QString userType = _userTableWidget->item(item_row, 1)->text();
		QString userId = _userTableWidget->item(item_row, 2)->text();
		QString userAccount = _userTableWidget->item(item_row, 3)->text();


		if (userAccount == "admin")
		{
			SCMessageBox* myBox1 = new SCMessageBox(tr("Delete User Hint"), tr("Sorry,the super administrator can't be deleted!"), otsCommon::SCMessageBox::Information, this);
			myBox1->exec();
			myBox1->deleteLater();
			myBox1 = nullptr;
			return;
		}

		SCMessageBox* myBox11 = new SCMessageBox(tr("Delete User Hint"), tr("Are your sure to delete current user?"), otsCommon::SCMessageBox::Warning, this);;
		myBox11->exec();

		if (myBox11->result() == QDialog::Accepted)
		{
			int retCode = otsServer::DataSession::instance()->delUser(userId.toInt());
			if (retCode)
			{
				emit getUserDataAgain();
				qInfo() << QStringLiteral("删除用户成功！！！");
			}
		}
		myBox11->deleteLater();
		myBox11 = nullptr;
	}
	else if (pSender == _modUser)
	{
		if (_popDialog == nullptr)
		{
			qDebug() << "_popDialog is nullptr";
			return;
		}

		//必须选中item才能弹框编辑
		QTableWidgetItem* item = _userTableWidget->currentItem();
		if (!item)
		{
			SCMessageBox* myBox2 = new SCMessageBox(tr("Modify User Hint"), tr("Please select the user you want to delete from the table above!"), otsCommon::SCMessageBox::Information, this);
			myBox2->exec();
			myBox2->deleteLater();
			myBox2 = nullptr;
			if (_popDialog)
			{
				_popDialog->deleteLater();
				_popDialog = nullptr;
			}
			return;
		}

		//从item获取数据(原数据，旧数据)
		int item_row = _userTableWidget->row(item);
		QString userName = _userTableWidget->item(item_row, 0)->text(); //用户名称
		QString userType = _userTableWidget->item(item_row, 1)->text();//账户类型（普通用户2、管理员1）
		QString userId = _userTableWidget->item(item_row, 2)->text();//登录账号
		QString userAccount = _userTableWidget->item(item_row, 3)->text();//用户密码
		QString userPassword = _userTableWidget->item(item_row, 4)->data(Qt::UserRole).toString();//用户ID

		int type = (userType == ("Common user") ? 2 : 1);

		/*<超级管理员可以自己修改密码，其他普通用户不能修该超级管理员的信息>*/
		if (__userAccountName == "admin")
		{

		}
		else
		{
			if (userAccount == "admin")
			{
				SCMessageBox* myBox1 = new SCMessageBox(tr("Modify User Hint"), tr("Sorry, super administrator information can't be modified!"), otsCommon::SCMessageBox::Information, this);
				myBox1->exec();
				myBox1->deleteLater();
				myBox1 = nullptr;
				return;
			}
		}

		qInfo() << "userInfo:" << userId << type << userAccount << userPassword;
		_popDialog->setObjectName("mod-propertyDialog");
		g_userName = userName;
		g_userType = type;
		g_userAcct = userAccount;
		g_userPswd = userPassword;
		_popDialog->modPersonnelUI(userName, userId, type, userAccount, userPassword);
		int ret = _popDialog->exec();

		if (ret == QDialog::Accepted)
		{
			int db_userID = userId.toInt();//userId疑似为0
			qInfo() << "userId:" << userId;
			std::tuple<std::string, int, std::string> userInfo;

			qInfo() << "modified new data:" << g_userName << g_userType << g_userAcct << g_userPswd;

			userInfo = std::make_tuple(g_userName.toStdString(), g_userType, g_userAcct.toStdString());
			int retCode = otsServer::DataSession::instance()->updateUserNotPwd(db_userID, userInfo);
			if (1 == retCode)
			{
				emit getUserDataAgain();
				qInfo() << QStringLiteral("modify-User, ID = %1 success!!!").arg(db_userID);
			}
			else if (-99 == retCode)
			{
				qInfo() << "modify-repeated user data!!!";
				SCMessageBox* modUserRepeatDlg = new SCMessageBox(tr("Modify User Repeated Hint"), tr("Please confirm the user information is Repeated or not!"), otsCommon::SCMessageBox::Information, this);
				modUserRepeatDlg->exec();
				modUserRepeatDlg->deleteLater();
				modUserRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << "modify-User data Failed!!!";
				SCMessageBox* modUserFailDlg = new SCMessageBox(tr("Modify User Failed Hint"), tr("Sorry,Please modify the user information again!"), otsCommon::SCMessageBox::Information, this);
				modUserFailDlg->exec();
				modUserFailDlg->deleteLater();
				modUserFailDlg = nullptr;
			}
		}
	}
	else if (pSender == _resetPassword)
	{
		if (_popDialog == nullptr)
		{
			qDebug() << "_popDialog is nullptr";
			return;
		}

		if (2 == __userType)
		{
			SCMessageBox* resetDlg = new SCMessageBox(tr("Modify User Password Failed Hint"), tr("The common user can't modify the password!"), otsCommon::SCMessageBox::Information, this);
			resetDlg->exec();
			resetDlg->deleteLater();
			resetDlg = nullptr;
			if (_popDialog)
			{
				_popDialog->deleteLater();
				_popDialog = nullptr;
			}
			return;
		}

		//必须选中item才能弹框编辑
		QTableWidgetItem* item = _userTableWidget->currentItem();
		if (!item)
		{
			SCMessageBox* myBox2 = new SCMessageBox(tr("Modify User Password Hint"), tr("Please select the user you want to modify from the table!"), otsCommon::SCMessageBox::Information, this);
			myBox2->exec();
			myBox2->deleteLater();
			myBox2 = nullptr;
			if (_popDialog)
			{
				_popDialog->deleteLater();
				_popDialog = nullptr;
			}
			return;
		}

		//从item获取数据(原数据，旧数据)
		int item_row = _userTableWidget->row(item);
		QString userName = _userTableWidget->item(item_row, 0)->text(); //用户名称
		QString userType = _userTableWidget->item(item_row, 1)->text();//账户类型（普通用户2、管理员1）
		QString userId = _userTableWidget->item(item_row, 2)->text();//登录账号
		QString userAccount = _userTableWidget->item(item_row, 3)->text();//用户密码
		QString userPassword = _userTableWidget->item(item_row, 4)->data(Qt::UserRole).toString();//用户ID

		int type = (userType == "Common user" ? 2 : 1);

		if (__userAccountName != "admin" && userAccount == "admin")
		{
			SCMessageBox* myBox2 = new SCMessageBox(tr("Modify User Password Hint"), tr("The super administrator's password can only be modified by himself!"), otsCommon::SCMessageBox::Information, this);
			myBox2->exec();
			myBox2->deleteLater();
			myBox2 = nullptr;
			if (_popDialog)
			{
				_popDialog->deleteLater();
				_popDialog = nullptr;
			}
			return;
		}

		_popDialog->setObjectName("reset-propertyDialog");
		_popDialog->resetPasswordUI();
		int ret = _popDialog->exec();

		if (ret == QDialog::Accepted)
		{
			std::tuple<std::string, int, std::string, std::string> userInfo; ///用户名，用户类型，用户密码，用户账户名称

			qInfo() << "reset password:" << g_userPswd;

			userInfo = std::make_tuple(userName.toStdString(), type, g_userPswd.toStdString(), userAccount.toStdString());
			int retCode = otsServer::DataSession::instance()->updateUser(userId.toInt(), userInfo);
			if (1 == retCode)
			{
				emit getUserDataAgain();
				qInfo() << QStringLiteral("modify-User, ID = %1 success!!!").arg(userId.toInt());
			}
			else if (-99 == retCode)
			{
				qInfo() << "modify-repeated user data!!!";
				SCMessageBox* modUserRepeatDlg = new SCMessageBox(tr("Modify User Information Repeated Hint"), tr("Please confirm user information is repeated or not!"), otsCommon::SCMessageBox::Information, this);
				modUserRepeatDlg->exec();
				modUserRepeatDlg->deleteLater();
				modUserRepeatDlg = nullptr;
			}
			else
			{
				qInfo() << "modify-User data Failed!!!";
				SCMessageBox* modUserFailDlg = new SCMessageBox(tr("Modify User Information Failed Hint"), tr("Sorry,Please modify user information again!"), otsCommon::SCMessageBox::Information, this);
				modUserFailDlg->exec();
				modUserFailDlg->deleteLater();
				modUserFailDlg = nullptr;
			}
		}		
	}

	if (_popDialog)
	{
		_popDialog->deleteLater();
		_popDialog = nullptr;
	}
}

void AddAccountWidget::refreshUserInfo()
{
	int startIndex = 0; //开始索引
	std::vector<std::tuple<int, std::string, int, std::string, std::string>> vectorUserInfo;
	int retCode = otsServer::DataSession::instance()->getUserList(startIndex, vectorUserInfo);

	for (int i = 0; i < vectorUserInfo.size(); i++)
	{
		////如果超过50个用户，则再继续请求一次
		if (vectorUserInfo.size() >= 50)
		{
			otsServer::DataSession::instance()->getUserList(50, vectorUserInfo);
		}

		int userId = 0;
		std::string userName = "";
		int level = 0;
		std::string password = "";
		std::string account = "";

		std::tie(userId, userName, level, password, account) = vectorUserInfo[i];
		_userTableWidget->setRowCount(vectorUserInfo.size());

		_userTableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(userName)));
		_userTableWidget->setItem(i, 1, new QTableWidgetItem((level == 1 ? ("Administrator") : ("Common user"))));
		_userTableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(userId)));
		_userTableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(account)));
		QTableWidgetItem* pPassword_item = new QTableWidgetItem("******");
		pPassword_item->setData(Qt::UserRole, QString::fromStdString(password));
		_userTableWidget->setItem(i, 4, pPassword_item);
	}

	for (int row = 0; row < _userTableWidget->rowCount(); row++)
	{
		for (int col = 0; col < _userTableWidget->columnCount(); col++)
		{
			_userTableWidget->item(row, col)->setTextAlignment(Qt::AlignCenter);
		}
	}
	qInfo() << QStringLiteral("数据获取成功！");
}

void AddAccountWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

	QWidget::paintEvent(event);
}