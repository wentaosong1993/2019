#include "PropertyDialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDebug>
#include <vector>
#include <tuple>
#include "otsServer/DataSession.h"
#include <QLabel>
#include <QStyle>
#include <QEvent>

////信号源名称、信号源新名称、ID、是否4k索引
QString _signalName = "";
QString _signalNewName = "";		 //信号源新名称
int _signalId = 0;
int _typeIndex = 0;

//信号源组名称、信号源组新名称
QString _signalGroupName = "";	 //信号源组名称
QString _signalGroupNewName = ""; //信号源组新名称


/*编辑人员信息相关*/
QString g_userName = ""; //用户名称
QString g_userPswd = ""; //用户密码
QString g_userAcct = ""; //登录账号
int g_userType = 2; //账户类型（普通用户、管理员）
QString g_userId = ""; //用户ID

PropertyDialog::PropertyDialog(QWidget *parent)
	:QDialog(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setObjectName("propertyDialog");
	
	_okPushButton = new QPushButton(tr("OK"),this);
	_cancelPushButton = new QPushButton(tr("Cancel"),this);
	_okPushButton->setObjectName("property-okButton");
	_cancelPushButton->setObjectName("property-cancelButton");
	connect(_okPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
	connect(_cancelPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
}

PropertyDialog::~PropertyDialog()
{
/*-----------析构信号源、信号源组相关对象-----------------*/
	if (_okPushButton)
	{
		delete _okPushButton;
		_okPushButton = nullptr;
	}
	if (_cancelPushButton)
	{
		delete _cancelPushButton;
		_cancelPushButton = nullptr;
	}
	if (_signalNameLineEdit)
	{
		delete _signalNameLineEdit;
		_signalNameLineEdit = nullptr;
	}
	if (_signalIdLineEdit)
	{
		delete _signalIdLineEdit;
		_signalIdLineEdit = nullptr;
	}
	if (_is4KCombox)
	{
		delete _is4KCombox;
		_is4KCombox = nullptr;
	}
	if (_signalGroupNameLineEdit)
	{
		delete _signalGroupNameLineEdit;
		_signalGroupNameLineEdit = nullptr;
	}

	if (_signalRenameLineEdit)
	{
		delete _signalRenameLineEdit;
		_signalRenameLineEdit = nullptr;
	}

	if (_signalGroupRenameLineEdit)
	{
		delete _signalGroupRenameLineEdit;
		_signalGroupRenameLineEdit = nullptr;
	}

/*-----------析构编辑人员信息对象-----------------*/
	if (_userNameLineEdit)
	{
		delete _userNameLineEdit; 
		_userNameLineEdit = nullptr;
	}

	if (_userAccLineEdit)
	{
		delete _userAccLineEdit;
		_userAccLineEdit = nullptr;
	}

	if (_userPsLineEdit)
	{
		delete _userPsLineEdit;
		_userPsLineEdit = nullptr;
	}

	if (_userTypeCombox)
	{
		delete _userTypeCombox;
		_userTypeCombox = nullptr;
	}

	if (_userIdLineEdit)
	{
		delete _userIdLineEdit;
		_userIdLineEdit = nullptr;
	}
}

//初始化信号源弹框
void PropertyDialog::initSignalUi()
{
	//this->setWindowTitle(QStringLiteral("新增信号源"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	//vLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
	vLayout->setMargin(0);

	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("addSignal-titleLabel");
	titleLabel->setText(tr("Add Signal"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("addSignal-Gap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线
	
	_signalNameLineEdit = new QLineEdit(this);
	_signalIdLineEdit = new QLineEdit(this);
	_is4KCombox = new QComboBox(this);
	_signalNameLineEdit->setObjectName("add-signalNameLineEdit");
	_signalIdLineEdit->setObjectName("add-signalIdLineEdit");
	_is4KCombox->setObjectName("add-signal4KCombox");
	QStringList is4KTypeList;
	is4KTypeList << tr("No") << tr("Yes");
	_is4KCombox->addItems(is4KTypeList);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Signal Name:"), _signalNameLineEdit);
	formLayout->addRow(tr("&Signal ID:"), _signalIdLineEdit);
	formLayout->addRow(tr("&Support 4K:"), _is4KCombox);
	vLayout->addLayout(formLayout);

	QWidget* signalNameLabel = formLayout->labelForField(_signalNameLineEdit);
	QWidget* signalIdLabel = formLayout->labelForField(_signalIdLineEdit);
	QWidget* signal4KType = formLayout->labelForField(_is4KCombox);
	signalNameLabel->setObjectName("add-signalNameLabel");
	signalIdLabel->setObjectName("add-signalIdLabel");
	signal4KType->setObjectName("add-signal4KType");

	_signalNameLineEdit->setMaxLength(10);
	_signalNameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	_signalIdLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-5 characters]"));
	QRegExp regx("[0-9]{1,5}$");
	QValidator *validator = new QRegExpValidator(regx, _signalIdLineEdit);
	_signalIdLineEdit->setValidator(validator);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton(0);
	//_cancelPushButton = new QPushButton(0);
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("add-signal-okButton");
	_cancelPushButton->setObjectName("add-signal-cancelButton");

	connect(_signalNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
	connect(_signalIdLineEdit, &QLineEdit::textEdited, this, &PropertyDialog::disposeEditingFinishedSlot);
	connect(_is4KCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(disposeComboxIndexChangedSlot()));
}

//初始化信号源组弹框
void PropertyDialog::initSignalGroupUI()
{
	if (this == nullptr)
	{
		qInfo() << "this ptr is null";
	}
	//this->setWindowTitle(tr("Add Signal Group"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	//vLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
	vLayout->setMargin(0);

	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("addSignalGroup-titleLabel");
	titleLabel->setText(tr("Add Signal Group"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("addSignalGroup-Gap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_signalGroupNameLineEdit = new QLineEdit(this);
	_signalGroupNameLineEdit->setObjectName("add-signalGroupNameLineEdit");
	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Signal Group Name:"), _signalGroupNameLineEdit);
	vLayout->addLayout(formLayout);

	QWidget* signalGroupNameLabel = formLayout->labelForField(_signalGroupNameLineEdit);
	signalGroupNameLabel->setObjectName("add-signalGroupNameLabel"); 

	_signalGroupNameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	_signalGroupNameLineEdit->setMaxLength(10);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton;
	//_cancelPushButton = new QPushButton;
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("add-signalGroup-okButton");
	_cancelPushButton->setObjectName("add-signalGroup-cancelButton");

	connect(_signalGroupNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
}

//重命名信号源
void PropertyDialog::renameSignal(/*QString name*/)
{
	//this->setWindowTitle(QStringLiteral("重命名信号源"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	//vLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));

	vLayout->setMargin(0);

	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("reName-Signal-titleLabel");
	titleLabel->setText(tr("Rename Signal"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("reName-Signal-Gap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_signalRenameLineEdit = new QLineEdit(this);
	_signalRenameLineEdit->setObjectName("reName-signalNameLineEdit");
	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Signal Name:"), _signalRenameLineEdit);
	vLayout->addLayout(formLayout);

	_signalRenameLineEdit->setMaxLength(10);
	_signalRenameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));

	QWidget* reNameLabel = formLayout->labelForField(_signalRenameLineEdit);
	reNameLabel->setObjectName("reName-signalNameLabel");

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton(0);
	//_cancelPushButton = new QPushButton(0);
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("rename-signal-okButton");
	_cancelPushButton->setObjectName("rename-signal-cancelButton");

	connect(_signalRenameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
}

//重命名信号源组
void PropertyDialog::renameSignalGroup(/*QString name*/)
{
	//this->setWindowTitle(QStringLiteral("重命名信号源组"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	//vLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
	vLayout->setMargin(0);

	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("reName-SignalGroup-titleLabel");
	titleLabel->setText(tr("Rename Signal Group"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("reName-SignalGroup-Gap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_signalGroupRenameLineEdit = new QLineEdit(this);
	_signalGroupRenameLineEdit->setObjectName("reName-signalGroupNameLineEdit");
	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Siganl Group Name:"), _signalGroupRenameLineEdit);
	vLayout->addLayout(formLayout);

	_signalGroupRenameLineEdit->setMaxLength(10);
	_signalGroupRenameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	QWidget *signalGroupLabel = formLayout->labelForField(_signalGroupRenameLineEdit);
	signalGroupLabel->setObjectName("reName-signalGroupNameLabel");

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton(0);
	//_cancelPushButton = new QPushButton(0);
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("rename-signalGroup-okButton");
	_cancelPushButton->setObjectName("rename-signalGroup-cancelButton");

	connect(_signalGroupRenameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
}

void PropertyDialog::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			if(_okPushButton)
			{
				_okPushButton->setText(_okPushButton->text());
			}
			if(_cancelPushButton)
			{
				_cancelPushButton->setText(_cancelPushButton->text());
			}
		}
		break;
	default:
		break;
	}
}

void PropertyDialog::disposeButtonSlot()
{
	QPushButton *pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}

	if (pSender == _okPushButton)
	{
		if (_okPushButton->objectName() == "add-signal-okButton")
		{
			//必填
			if (this->_signalNameLineEdit->text().trimmed().isEmpty() || this->_signalIdLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "add-signalGroup-okButton")
		{
			//必填
			if (this->_signalGroupNameLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "rename-signal-okButton")
		{
			//必填
			if (this->_signalRenameLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "rename-signalGroup-okButton")
		{
			//必填
			if (this->_signalGroupRenameLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "add-account-okButton")
		{
			if (_userNameLineEdit->text().trimmed().isEmpty() || _userAccLineEdit->text().trimmed().isEmpty() || _userPsLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "mod-account-okButton")
		{
			if (_userNameLineEdit->text().trimmed().isEmpty() || _userAccLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}
		else if (_okPushButton->objectName() == "reset-password-okButton")
		{
			if (_resetPasswordLineEdit->text().trimmed().isEmpty())
			{
				return;
			}
		}

		accept();
	}
	else if (pSender == _cancelPushButton)
	{
		//重置内容
		if (_signalNameLineEdit)
		{
			_signalNameLineEdit->clear();
		}
		if (_signalIdLineEdit)
		{
			_signalIdLineEdit->clear();
		}
		if (_signalRenameLineEdit)
		{
			_signalRenameLineEdit->clear();
		}
		if (_signalGroupNameLineEdit)
		{
			_signalGroupNameLineEdit->clear();
		}
		if (_signalGroupRenameLineEdit)
		{
			_signalGroupRenameLineEdit->clear();
		}
		if (_userNameLineEdit)
		{
			_userNameLineEdit->clear();
		}
		if (_userAccLineEdit)
		{
			_userAccLineEdit->clear();
		}
		if (_userPsLineEdit)
		{
			_userPsLineEdit->clear();
		}
		if (_userIdLineEdit)
		{
			_userIdLineEdit->clear();
		}
		reject();
	}
}

void PropertyDialog::disposeEditingFinishedSlot()
{
	QLineEdit *pSender = dynamic_cast<QLineEdit*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	//获取信号源组名称、重命名的信号源组名称
	if (pSender == _signalGroupNameLineEdit)
	{
		if (_signalGroupNameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_signalGroupName = _signalGroupNameLineEdit->text().trimmed();
			qInfo() << _signalGroupName;
		}
	}
	else if (pSender == _signalGroupRenameLineEdit)
	{
		if (_signalGroupRenameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_signalGroupNewName = _signalGroupRenameLineEdit->text().trimmed();
		}
	}
	//信号源信息（名称、ID）
	else if (pSender == _signalNameLineEdit)
	{
		if (_signalNameLineEdit->text().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_signalName = _signalNameLineEdit->text().trimmed();
		}
	}
	else if (pSender == _signalIdLineEdit)
	{
		if (_signalIdLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_signalId = _signalIdLineEdit->text().trimmed().toInt();
		}
	}
	else if (pSender == _signalRenameLineEdit)
	{
		if (_signalRenameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_signalNewName = _signalRenameLineEdit->text().trimmed();
			qInfo() <<__LINE__ << _signalNewName;
		}
	}
}

void PropertyDialog::disposeComboxIndexChangedSlot()
{
	QComboBox *pSender = dynamic_cast<QComboBox*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	if (pSender == _is4KCombox)
	{
		_typeIndex = _is4KCombox->currentIndex();
	}
}

///初始化人员信息
void PropertyDialog::addPersonnelUI()
{
	if (this == nullptr)
	{
		qDebug() << "this ptr is nullptr";
	}
	//this->setWindowTitle(QStringLiteral("新增普通用户"));
	QVBoxLayout *vLayout = new QVBoxLayout;

	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("addUser-titleLabel");
	titleLabel->setText(tr("Add User"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("addUser-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_userNameLineEdit = new QLineEdit(this);
	_userTypeCombox = new QComboBox(this);
	_userAccLineEdit = new QLineEdit(this);
	_userPsLineEdit = new QLineEdit(this);
	_userNameLineEdit->setObjectName("add-userNameLineEdit");
	_userTypeCombox->setObjectName("add-userTypeCombox");
	_userAccLineEdit->setObjectName("add-userAccountLineEdit");
	_userPsLineEdit->setObjectName("add-userKeyLineEdit");

	/*<设置行文本编辑框支持的格式>*/
	_userNameLineEdit->setMaxLength(10);
	_userNameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	_userTypeCombox->setEditable(false);
	QRegExp regx("[0-9A-Za-z]{6,8}$");
	QValidator *validator = new QRegExpValidator(regx, _userPsLineEdit);
	validator = new QRegExpValidator(regx, _userAccLineEdit);
	_userPsLineEdit->setValidator(validator);
	_userAccLineEdit->setValidator(validator);
	_userAccLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 6-8 letters,numbers]"));
	_userPsLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 6-8 letters,numbers]"));
	_userPsLineEdit->setEchoMode(QLineEdit::Password);

	QStringList typeList;
	typeList << tr("Common user") << tr("Administrator");
	_userTypeCombox->addItems(typeList);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&User Name:"), _userNameLineEdit);
	formLayout->addRow(tr("&User Type:"), _userTypeCombox); //管理员1，2普通用户
	formLayout->addRow(tr("&User Account:"), _userAccLineEdit);
	formLayout->addRow(tr("&User Password:"), _userPsLineEdit);
	vLayout->addLayout(formLayout); //加中间内容

	QWidget * userNameLabel = formLayout->labelForField(_userNameLineEdit);
	QWidget * userTypeLabel = formLayout->labelForField(_userTypeCombox);
	QWidget * userAccountLabel = formLayout->labelForField(_userAccLineEdit);
	QWidget*  userKeyLabel = formLayout->labelForField(_userPsLineEdit);
	userNameLabel->setObjectName("add-userNameLabel");
	userTypeLabel->setObjectName("add-userTypeLabel");
	userAccountLabel->setObjectName("add-userAccountLabel");
	userKeyLabel->setObjectName("add-userKeyLabel");

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton(0);
	//_cancelPushButton = new QPushButton(0);
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout); //加按钮
	this->setLayout(vLayout);

	_okPushButton->setObjectName("add-account-okButton");
	_cancelPushButton->setObjectName("add-account-cancelButton");

	/*<因设置正则表达式后，信号槽连接失效 >*/
	connect(_userNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposePersonnelButtonSlot()));
	connect(_userAccLineEdit, &QLineEdit::textEdited, this, &PropertyDialog::disposePersonnelButtonSlot);
	connect(_userPsLineEdit, &QLineEdit::textEdited, this, &PropertyDialog::disposePersonnelButtonSlot);
	connect(_userTypeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(disposePersonnelComboxIndexChangedSlot(int)));
	//_userTypeCombox->setCurrentIndex(0);
}

void PropertyDialog::delPersonnelUI(QString userId)
{
	this->setWindowTitle(tr("Delete Common User"));
	QVBoxLayout *vLayout = new QVBoxLayout;

	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("delUser-titleLabel");
	titleLabel->setText(tr("Delete User"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("delUser-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_userIdLineEdit = new QLineEdit(this);
	_userIdLineEdit->setObjectName("del-userIdLineEdit");
	_userIdLineEdit->setText(userId);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&User ID:"), _userIdLineEdit);
	vLayout->addLayout(formLayout);

	QWidget *userIdLabel = formLayout->labelForField(_userIdLineEdit);
	userIdLabel->setObjectName("del-userIdLabel");

	_userIdLineEdit->setEnabled(false);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	//_okPushButton = new QPushButton(0);
	//_cancelPushButton = new QPushButton(0);
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("del-account-okButton");
	_cancelPushButton->setObjectName("del-account-cancelButton");

	connect(_userIdLineEdit, SIGNAL(editingFinished()), this, SLOT(disposePersonnelButtonSlot()));
}

void PropertyDialog::modPersonnelUI(QString userName, QString userId , int userType, QString userAccount, QString userPassword)
{
	//this->setWindowTitle(QStringLiteral("修改普通用户"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("modUser-titleLabel");
	titleLabel->setText(tr("Modify User"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("modUser-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_userNameLineEdit = new QLineEdit(this);
	_userIdLineEdit = new QLineEdit(this);
	_userTypeCombox = new QComboBox(this);
	_userAccLineEdit = new QLineEdit(this);
	//_userPsLineEdit = new QLineEdit(this);
	_userNameLineEdit->setObjectName("mod-userNameLineEdit");
	_userTypeCombox->setObjectName("mod-userTypeCombox");
	_userAccLineEdit->setObjectName("mod-userAccountLineEdit");
	//_userPsLineEdit->setObjectName("mod-userKeyLineEdit");
	_userIdLineEdit->setObjectName("mod-userIdLineEdit");

	QStringList typeList;
	typeList << tr("Common user") << tr("Administrator");
	_userTypeCombox->addItems(typeList);

	//qInfo() << "userType:" << userType;
	////普通用户这里要做处理！！！三目运算符处理
	//_userNameLineEdit->setText(userName);
	//_userIdLineEdit->setText(userId);
	////_userTypeCombox->setCurrentText(userType);
	//_userTypeCombox->setCurrentIndex(userType == 2 ? 0 : 1);
	//_userAccLineEdit->setText(userAccount);
	//_userPsLineEdit->setText(userPassword);
	//if (userAccount == "admin")
	//{
	//	_userAccLineEdit->setEnabled(false);
	//	_userTypeCombox->setEnabled(false);
	//}

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&User Name:"), _userNameLineEdit);
	formLayout->addRow(tr("&User ID:"), _userIdLineEdit);
	formLayout->addRow(tr("&User Type:"), _userTypeCombox);
	formLayout->addRow(tr("&User Account:"), _userAccLineEdit);
	//formLayout->addRow(QStringLiteral("&用户密码:"), _userPsLineEdit);
	vLayout->addLayout(formLayout);

	QWidget * userNameLabel = formLayout->labelForField(_userNameLineEdit);
	QWidget * userIdLabel = formLayout->labelForField(_userIdLineEdit);
	QWidget * userTypeLabel = formLayout->labelForField(_userTypeCombox);
	QWidget * userAccountLabel = formLayout->labelForField(_userAccLineEdit);
	//QWidget*  userKeyLabel = formLayout->labelForField(_userPsLineEdit);
	userNameLabel->setObjectName("mod-userNameLabel");
	userIdLabel->setObjectName("mod-userIdLineEdit");
	userTypeLabel->setObjectName("mod-userTypeLabel");
	userAccountLabel->setObjectName("mod-userAccountLabel");
	//userKeyLabel->setObjectName("mod-userKeyLabel");

	//_userPsLineEdit->style()->unpolish(_userPsLineEdit);
	//_userPsLineEdit->style()->polish(_userPsLineEdit);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("mod-account-okButton");
	_cancelPushButton->setObjectName("mod-account-cancelButton");

	/*字符限制*/
	_userNameLineEdit->setMaxLength(10);
	_userNameLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	_userTypeCombox->setEditable(false);
	_userIdLineEdit->setEnabled(false);
	QRegExp regx("[0-9A-Za-z]{6,8}$");
	QValidator *validator = new QRegExpValidator(regx, _userPsLineEdit);
	validator = new QRegExpValidator(regx, _userAccLineEdit);
	//_userPsLineEdit->setValidator(validator);
	_userAccLineEdit->setValidator(validator);
	_userAccLineEdit->setPlaceholderText(tr("Necessary filling.[Limit 1-10 characters]"));
	//_userPsLineEdit->setPlaceholderText(QStringLiteral("必填；[限制6-8位字母、数字]"));
	//_userPsLineEdit->setEchoMode(QLineEdit::Password);


	qInfo() << "userType:" << userType;
	//普通用户这里要做处理！！！三目运算符处理
	_userNameLineEdit->setText(userName);
	_userIdLineEdit->setText(userId);
	//_userTypeCombox->setCurrentText(userType);
	_userTypeCombox->setCurrentIndex(userType == 2 ? 0 : 1);
	_userAccLineEdit->setText(userAccount);
	//_userPsLineEdit->setText(userPassword);
	if (userAccount == "admin")
	{
		_userAccLineEdit->setEnabled(false);
		_userTypeCombox->setEnabled(false);
	}

	connect(_userNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposePersonnelButtonSlot()));
	connect(_userAccLineEdit, &QLineEdit::textEdited, this, &PropertyDialog::disposePersonnelButtonSlot);
	connect(_userPsLineEdit, &QLineEdit::textChanged, this, &PropertyDialog::disposePersonnelButtonSlot);
	connect(_userIdLineEdit, SIGNAL(editingFinished()), this, SLOT(disposePersonnelButtonSlot()));
	connect(_userTypeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(disposePersonnelComboxIndexChangedSlot(int)));
}

void PropertyDialog::resetPasswordUI()
{
	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("resetPassword-titleLabel");
	titleLabel->setText(tr("Modify User Password"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("resetPassword-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_resetPasswordLineEdit = new QLineEdit(this);
	_resetPasswordLineEdit->setObjectName("reset-LineEdit");

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&User Password:"), _resetPasswordLineEdit);
	vLayout->addLayout(formLayout);

	QWidget*  userKeyLabel = formLayout->labelForField(_resetPasswordLineEdit);
	userKeyLabel->setObjectName("reset-userPasswordLabel");

	//限制
	QRegExp regx("[0-9A-Za-z]{6,8}$");
	QValidator *validator = new QRegExpValidator(regx, _resetPasswordLineEdit);
	_resetPasswordLineEdit->setValidator(validator);
	_resetPasswordLineEdit->setPlaceholderText(tr("Necessary filling.[limit 6-8 letters,numbers]"));
	_resetPasswordLineEdit->setEchoMode(QLineEdit::Password);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));

	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	_okPushButton->setObjectName("reset-password-okButton");
	_cancelPushButton->setObjectName("reset-password-cancelButton");
	connect(_resetPasswordLineEdit, &QLineEdit::textChanged, this, &PropertyDialog::disposePersonnelButtonSlot);
}

void PropertyDialog::disposePersonnelComboxIndexChangedSlot(int index)
{
	if (0 == index)
	{
		g_userType = 2;
	}
	else
	{
		g_userType = 1;
	}
	qInfo() << "g_userType:" << g_userType;
}

void PropertyDialog::disposePersonnelButtonSlot()
{
	QLineEdit *pSender = dynamic_cast<QLineEdit*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	//获取添加的人员名称、类型、账户名、账户密码
	if (pSender == _userNameLineEdit)
	{
		if (_userNameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
		}
		else
		{
			g_userName = _userNameLineEdit->text().trimmed();
			qInfo() << g_userName;
		}
	}
	else if (pSender == _userAccLineEdit)
	{
		if (_userAccLineEdit->text().trimmed().isEmpty())
		{
			//必填项
		}
		else
		{
			g_userAcct = _userAccLineEdit->text().trimmed();
			qInfo() << g_userAcct;
		}
	}
	else if (pSender == _userPsLineEdit)
	{
		if (_userPsLineEdit->text().isEmpty())
		{
			//必填项
			//return;
		}
		else
		{	
			g_userPswd = _userPsLineEdit->text().trimmed();
			qInfo() << g_userPswd;
		}
	}
	else if (pSender == _userIdLineEdit)
	{
		if (_userIdLineEdit->text().isEmpty())
		{
			//必填
		}
		else
		{
			g_userId = _userIdLineEdit->text().trimmed();
			qInfo() << g_userId;
		}
	}
	else if (pSender == _resetPasswordLineEdit)
	{
		if (_resetPasswordLineEdit->text().isEmpty())
		{
			//必填
		}
		else
		{
			g_userPswd = _resetPasswordLineEdit->text().trimmed();
			qInfo() << "password:" << g_userPswd;
		}
	}
	else
	{

	}
}