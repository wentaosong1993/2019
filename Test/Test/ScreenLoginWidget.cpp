#include "ScreenLoginWidget.h"
#include <otsServer/DataSession.h>
#include <QValidator>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStackedWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QComboBox>
#include <QStackedLayout> 
#include <QString>
#include <QTimerEvent>
#include <QStyledItemDelegate>
#include <QTimer>
#include "ui_ScreenLoginWidget.h"
#include "otsControler/net_globle_data.h"
#include "otsControler/login.h"
#include "ots/GUIEvent.h"
#include "ots/Logger.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

using namespace  rapidjson;

#define LOGIN_WIDTH		524
#define LOGIN_HEIGHT	535

int _userType = 2;

ScreenLoginWidget::ScreenLoginWidget(QWidget *parent)
: QWidget(parent), ui(new Ui::ScreenLoginWidget)
{
	m_bDrag = false;
	_leftSeconds = 0;
	_pLoginTimer = nullptr;
	_leftSecondTimer = nullptr;
	_userAccount = "";

	ui->setupUi(this);
	ui->menuFrame->installEventFilter(this);

	initWindow();
	ui->commonCombobox->setCurrentIndex(-1);
	connect(ui->commonCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCommonComboxIndexChanged(int)));
}

ScreenLoginWidget::~ScreenLoginWidget()
{
    delete ui;
}

void ScreenLoginWidget::initWindow()
{
	ui->labelError_1->hide();

	ui->commonUserLoginButton->setObjectName("login-Button");
	QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
	ui->commonCombobox->setItemDelegate(itemDelegate);

	ui->commonCombobox->lineEdit()->setPlaceholderText(QStringLiteral("�������û����ơ���"));

	ui->userKeyLineEdit->setPlaceholderText(QStringLiteral("�������û����롭��"));
	ui->commonUserLoginButton->setFocus();
	ui->commonUserLoginButton->setAutoDefault(true);
	ui->commonUserLoginButton->setDefault(true);

	//QRegExp accountReg("^[\u4e00-\u9fa5_a-zA-Z0-9]{6,10}$");//���֡���ĸ������
	QRegExp accountReg("[0-9A-Za-z]{6,10}$"); /*< ���֡���ĸ6~10λ >*/
	QValidator *validator = new QRegExpValidator(accountReg, ui->commonCombobox);
	ui->commonCombobox->setValidator(validator);

	/*<����������ĸ������ 6~8λ>*/
	QRegExp regx("[0-9A-Za-z]{6,8}$");
	validator = new QRegExpValidator(regx, ui->userKeyLineEdit);
	ui->userKeyLineEdit->setValidator(validator);

	ui->userKeyLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
	//ui->stackedWidget->setStackingMode(QStackedLayout::StackOne);

	/*��ʼ���˻�����������*/
	initData();
}

void ScreenLoginWidget::initData()
{
	_listLoginInfo.clear();
	ui->commonCombobox->clear();
	QFile file("login.ini");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QStringList list = line.split(",", QString::SkipEmptyParts);

		if (list.size() < 4)
		{
			break;
		}
		qInfo() << QString::fromLocal8Bit("��¼��Ϣ1��") << list.at(0) << "," << list.at(1) << "," << list.at(2).toInt() << "," << list.at(3).toInt();

		LoginInfo loginInfo;
		loginInfo.userAccount = list.at(0);;
		loginInfo.userKey = list.at(1);
		loginInfo.rememberState = list.at(2).toInt();
		loginInfo.autoLoginState = list.at(3).toInt();
		_listLoginInfo.append(loginInfo);
	}
	file.close();

	for (int i = 0; i < _listLoginInfo.size(); i++)
	{
		QString account = _listLoginInfo[i].userAccount;
		QString key = _listLoginInfo[i].userKey;
		bool rememberState = _listLoginInfo[i].rememberState;
		bool autoLogin = _listLoginInfo[i].autoLoginState;
		qInfo() << QString::fromLocal8Bit("��¼��Ϣ2��") << account << "," << key << "," << rememberState << "," << autoLogin;
		ui->commonCombobox->insertItem(i, account);
		ui->rememberKeyCheckBox->setChecked(rememberState);
		ui->autoLoginCheckBox->setChecked(autoLogin);
		ui->userKeyLineEdit->setText(key);
	}
	EnableLoginFrame(true);
}

void ScreenLoginWidget::EnableLoginFrame(bool enable)
{
	ui->commonCombobox->setEnabled(enable);
	ui->userKeyLineEdit->setEnabled(enable);
	ui->autoLoginCheckBox->setEnabled(enable);
	ui->rememberKeyCheckBox->setEnabled(enable);
}

void ScreenLoginWidget::on_commonUserButton_clicked()
{
	_userType = 2;
}

void ScreenLoginWidget::on_administratorButton_clicked()
{
	if (_pLoginTimer)
	{
		_pLoginTimer->stop();
		_pLoginTimer->deleteLater();
		_pLoginTimer = nullptr;
	}
	if (_leftSecondTimer)
	{
		_leftSecondTimer->stop();
		_leftSecondTimer->deleteLater();
		_leftSecondTimer = nullptr;
	}
	_userType = 1;
	ui->commonUserLoginButton->setObjectName("login-Button");
	ui->commonUserLoginButton->setText(QStringLiteral("��¼"));
	EnableLoginFrame(true);
}

bool ScreenLoginWidget::eventFilter(QObject * watched, QEvent * event)
{
	switch (event->type())
	{
	case QEvent::MouseButtonPress:
		if (watched == ui->menuFrame)
		{
			QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
			if (mouseEvent && mouseEvent->button() == Qt::LeftButton)
			{
				m_bDrag = true;
				m_DragPosition = mouseEvent->globalPos() - this->pos();
				return true;
			}
		}
		break;

	case QEvent::MouseMove:
	{
		QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
		if (m_bDrag && mouseEvent && (mouseEvent->buttons() && Qt::LeftButton))
		{
			QRect rect = QApplication::desktop()->availableGeometry();
			QPoint topLeftDlgPt = mouseEvent->globalPos() - m_DragPosition;
			if (!rect.contains(topLeftDlgPt)/* || !rect.contains(bottomRightpt)*/)
			{
				QPoint topLeftPt = rect.topLeft();
				if (topLeftDlgPt.y() < topLeftPt.y())
				{
					topLeftDlgPt.setY(topLeftPt.y());
				}
			}
			this->setCursor(Qt::SizeAllCursor);

			move(topLeftDlgPt);
			return true;
		}
	}
	break;

	case QEvent::MouseButtonRelease:
		m_bDrag = false;
		this->setCursor(Qt::ArrowCursor);
		return true;
		break;

	default:
		break;
	}

	return QWidget::eventFilter(watched, event);
}

void ScreenLoginWidget::on_loginMiniButton_clicked()
{
	setWindowState(Qt::WindowMinimized);
}

void ScreenLoginWidget::on_loginCloseButtton_clicked()
{
	close();
}

void ScreenLoginWidget::on_commonUserLoginButton_clicked()
{
	QPushButton *pSender = dynamic_cast<QPushButton*>(sender());
	if (pSender->objectName() == "login-Button")
	{
		std::string userAccount = ui->commonCombobox->currentText().toStdString();
		std::string userPassword = ui->userKeyLineEdit->text().trimmed().toStdString();
        /*
        * Author: John_Yang
        * Date:   2019-11-13
        * Note:   ���͵�¼��������
        */
        if (g_masterID == g_self_ID)
        {
            int user_id = 0;
            int user_type = 0;
            bool result = login_by_master(user_id, user_type, userAccount, userPassword);
            if (result)
            {
                OTS_LOG->warn("login successful id:{} type:{} name:{} pas:{}", user_id, user_type, userAccount, userPassword);
                Document jsonDoc;    //����һ��domԪ��Document
                Document::AllocatorType &allocator = jsonDoc.GetAllocator(); //��ȡ������
                jsonDoc.SetObject();    //����ǰ��Document����Ϊһ��object��Ҳ����˵������Document��һ��Object���͵�domԪ��
                // �½�Value����1(object����)
                Value value1(kObjectType);
                value1.AddMember("ret_code", 1, allocator);
                value1.AddMember("ret_msg", rapidjson::StringRef("��¼�ɹ�"), allocator);
                //ret_code �ɹ�����
                value1.AddMember("userid", user_id, allocator);
                value1.AddMember("type", user_type, allocator);
                rapidjson::StringBuffer js_buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(js_buffer);
                value1.Accept(writer);
                std::string ui_buff = js_buffer.GetString();
                QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_LOGIN_RES, QString::fromLocal8Bit(ui_buff.c_str())));
            }
            else
            {
                OTS_LOG->warn("login failed id:{} type:{}", user_id, user_type);
                Document jsonDoc;    //����һ��domԪ��Document
                Document::AllocatorType &allocator = jsonDoc.GetAllocator(); //��ȡ������
                jsonDoc.SetObject();    //����ǰ��Document����Ϊһ��object��Ҳ����˵������Document��һ��Object���͵�domԪ��
                // �½�Value����1(object����)
                Value value1(kObjectType);
                value1.AddMember("ret_code", 0, allocator);
                value1.AddMember("ret_msg", rapidjson::StringRef("��¼ʧ��"), allocator);
                rapidjson::StringBuffer js_buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(js_buffer);
                value1.Accept(writer);
                std::string ui_buff = js_buffer.GetString();
                QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_LOGIN_RES, QString::fromLocal8Bit(ui_buff.c_str())));
            }
        }
        else
        {
            send_login_by_slave(userAccount, userPassword);
        }
		int userId = 0;
		//bool retCode = otsServer::DataSession::instance()->checkLogin(userId, 2, userAccount, userPassword);
		//if (retCode)
		//{
		//	setUserAccount(QString::fromStdString(userAccount));
		//	setUserType(2);

		//	//��¼�ɹ�	
		//	//��ס����Ļ�,�����˺���Ϣ
		//	bool rememberKeyState = ui->rememberKeyCheckBox->isChecked();
		//	bool autoLoginState = ui->autoLoginCheckBox->isChecked();
		//	if (rememberKeyState || autoLoginState)
		//	{
		//		QFile file("login.ini");
		//		if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
		//			return;
		//		QTextStream in(&file);
		//		/*�˻��б�*/
		//		QStringList accountList;
		//		accountList.clear();
		//		while (!in.atEnd())
		//		{
		//			QString content = in.readLine();
		//			qInfo() << QStringLiteral("�ļ����ݣ�") << content;
		//			QStringList list = content.split(",", QString::SkipEmptyParts);
		//			if (list.size() == 0)
		//			{
		//				accountList.append(QString::fromStdString(userAccount));
		//			}
		//			else
		//			{
		//				qInfo() << QStringLiteral("��ǰ�����ݣ�") << list[0];
		//				accountList.append(list[0]);
		//			}
		//		}
		//		bool state = true;
		//		int index = 0;
		//		int listSize = accountList.size();
		//		if (listSize == 0)
		//		{
		//			in << QString::fromStdString(userAccount) << "," << QString::fromStdString(userPassword) << "," << QString::number(rememberKeyState) << "," << QString::number(autoLoginState) << "\n";
		//			file.close();
		//			//accept();
		//			return;
		//		}
		//		while (state)
		//		{

		//			if (QString::fromStdString(userAccount) == accountList[index])
		//			{
		//				state = false;
		//			}

		//			if (index < listSize - 1)
		//			{
		//				++index;
		//			}
		//			else
		//			{
		//				break;
		//			}

		//		}
		//		if (state)
		//		{
		//			in << QString::fromStdString(userAccount) << "," << QString::fromStdString(userPassword) << "," << QString::number(rememberKeyState) << "," << QString::number(autoLoginState) << "\n";
		//		}
		//		file.close();
		//	}
			//accept();
		//}
		//else
		//{
		//	setErrorTip(QStringLiteral("�û������������"), true);
		//	//��¼ʧ��
		//}
	}
	else if (pSender->objectName() == "cancel-autoLogin")
	{
		if (_pLoginTimer)
		{
			_pLoginTimer->stop();
			_pLoginTimer->deleteLater();
			_pLoginTimer = nullptr;
		}
		if (_leftSeconds)
		{
			_leftSecondTimer->stop();
			_leftSecondTimer->deleteLater();
			_leftSecondTimer = nullptr;
		}
		pSender->setObjectName("login-Button");
		pSender->setText(QStringLiteral("��¼"));
		EnableLoginFrame(true);
	}	
}

void ScreenLoginWidget::on_rememberKeyCheckBox_clicked()
{
	if (!ui->rememberKeyCheckBox->isChecked())
	{
		ui->autoLoginCheckBox->setCheckState(Qt::Unchecked);
	}
}

void ScreenLoginWidget::on_autoLoginCheckBox_clicked()
{
	if (ui->autoLoginCheckBox->isChecked())
	{
		ui->rememberKeyCheckBox->setCheckState(Qt::Checked);
	}
}

void ScreenLoginWidget::on_administratorLoginButton_clicked()
{

}

void ScreenLoginWidget::setErrorTip(const QString& sError, bool bLoginPwd)
{
	if (bLoginPwd)
	{
		ui->labelError_1->setText(sError);
		ui->labelError_1->show();
	}
}

void ScreenLoginWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
	this->startTimer(10);
	//ui->commonCombobox->setCurrentIndex(0);
}

void ScreenLoginWidget::timerEvent(QTimerEvent *event)
{
	this->killTimer(event->timerId());
	ui->commonCombobox->setCurrentIndex(0);
}

//�����û����ƻ�ȡ����
void ScreenLoginWidget::slotCommonComboxIndexChanged(int index)
{
	QString curAccount = ui->commonCombobox->itemText(index);
	QString key = "";
	bool rememberState = false;
	bool autoLogin = false;
	for (int i = 0; i < _listLoginInfo.size(); i++)
	{
		QString account = _listLoginInfo[i].userAccount;
		
		if (curAccount == account)
		{
			key = _listLoginInfo[i].userKey;
			rememberState = _listLoginInfo[i].rememberState;
			autoLogin = _listLoginInfo[i].autoLoginState;
			ui->userKeyLineEdit->setText(key);
			ui->rememberKeyCheckBox->setChecked(rememberState);
			ui->autoLoginCheckBox->setChecked(autoLogin);

			//���Զ���¼��ѡ
			if (ui->autoLoginCheckBox->isChecked())
			{
				//���õ�¼�òۺ���
				_pLoginTimer = new QTimer(this);
				ui->commonUserLoginButton->setObjectName("cancel-autoLogin");
				connect(_pLoginTimer, SIGNAL(timeout()), this, SLOT(processLogin()));
				_pLoginTimer->start(3000);
				_leftSeconds = _pLoginTimer->interval() / 1000;

				_leftSecondTimer = new QTimer(this);
				connect(_leftSecondTimer, SIGNAL(timeout()), this, SLOT(processLeftSeconds()));
				_leftSecondTimer->start(1000);

				ui->commonUserLoginButton->setText(QStringLiteral("ȡ���Զ���¼(%1)").arg(_leftSeconds));
				break;
			}
		}
		
		qInfo() << curAccount << "," << key;
	}
}

void ScreenLoginWidget::processLogin()
{
	ui->commonUserLoginButton->setText(QStringLiteral("��¼"));
	ui->commonUserLoginButton->setObjectName("login-Button");
	ui->commonUserLoginButton->click();
}

void ScreenLoginWidget::processLeftSeconds()
{
	--_leftSeconds;
	EnableLoginFrame(false);
	ui->commonUserLoginButton->setText(QStringLiteral("ȡ���Զ���¼(%1)").arg(_leftSeconds));
	if (_leftSeconds == 0)
	{
		EnableLoginFrame(true);
		ui->commonUserLoginButton->setText(QStringLiteral("��¼"));
		_leftSecondTimer->stop();
		_leftSecondTimer->deleteLater();
		_leftSecondTimer = nullptr;
	}
}

