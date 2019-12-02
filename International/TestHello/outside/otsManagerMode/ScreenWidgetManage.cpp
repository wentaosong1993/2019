#include "ScreenWidgetManage.h"
#include <vector>
#include <tuple>
#include "otsServer/DataSession.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDebug>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>
#include "otsCommon/MessageBox.h"

using namespace otsCommon;

ScreenManageWidget::ScreenManageWidget(QWidget *parent) : QWidget(parent)
{
	initUI(); 
	//createData();
	connect(_addScreenButton, SIGNAL(clicked()), this, SLOT(disposeScreenEditSlot()));
	connect(_modScreenButton, SIGNAL(clicked()), this, SLOT(disposeScreenEditSlot()));
	connect(_delScreenButton, SIGNAL(clicked()), this, SLOT(disposeScreenEditSlot()));
	connect(_screenListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(disposeSelectScreenSlot(QListWidgetItem *)));
	connect(this, SIGNAL(updateScreenData()), this, SLOT(createData()));
}

ScreenManageWidget::~ScreenManageWidget()
{
}

void ScreenManageWidget::disposeSelectScreenSlot(QListWidgetItem *selectItem)
{
	if (!selectItem)
	{
		return;
	}
	QString screenName = selectItem->text(); //大屏名称
	QString screenId = selectItem->data(Qt::UserRole).toString();
	qInfo() << "screenName:" << screenName << "screenId:" << screenId;
	setScreenName(screenName);
	setScreenID(screenId);
}

void ScreenManageWidget::changeEvent(QEvent* event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_addScreenButton->setText(_addScreenButton->text());
			_delScreenButton->setText(_delScreenButton->text());
			_modScreenButton->setText(_modScreenButton->text());
		}
		break;
	default:
		break;
	}
}

//初始化界面布局
void ScreenManageWidget::initUI()
{
	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->setContentsMargins(20, 20, 20, 0);
	QHBoxLayout *hLayout = new QHBoxLayout();

	_screenListWidget = new QListWidget(this);
	_screenListWidget->setObjectName("screenList");

	_addScreenButton = new QPushButton(this);
	_addScreenButton->setText(tr("Add Screen"));
	_addScreenButton->setObjectName("addScreen");
	_modScreenButton = new QPushButton(this);
	_modScreenButton->setText(tr("Modify Screen"));
	_modScreenButton->setObjectName("modScreen");
	_delScreenButton = new QPushButton(this);
	_delScreenButton->setText(tr("Delete Screen"));
	_delScreenButton->setObjectName("delScreen");
	
	vLayout->addWidget(_screenListWidget);

	hLayout->addWidget(_addScreenButton);
	hLayout->addWidget(_modScreenButton);
	hLayout->addWidget(_delScreenButton);

	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);
}

//初始化大屏列表
void ScreenManageWidget::createData()
{
	_screenListWidget->clear();
	std::vector<std::tuple<int, std::string, int> > vecScreensData;
	bool retCode = otsServer::DataSession::instance()->getAllScreenData(vecScreensData);
	for (int i = 0; i < vecScreensData.size(); i++)
	{
		QListWidgetItem* item = new QListWidgetItem;
		int id = std::get<0>(vecScreensData[i]);
		QString name = std::get<1>(vecScreensData[i]).c_str();
		int type = std::get<2>(vecScreensData[i]);
		item->setData(Qt::UserRole, id);
		item->setData(Qt::DisplayRole, name);
		item->setData(Qt::StatusTipRole, type);
		_screenListWidget->addItem(item);
	}
}

void ScreenManageWidget::disposeScreenEditSlot()
{
	QPushButton * pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		return;
	}
	_enrollDialog = new EnrollDataDialog(this);
	QString objName = pSender->objectName();
	if (objName == "addScreen")
	{
		_enrollDialog->setObjectName("addScreenDialog");
		_enrollDialog->AddScreenDataUI();
		int ret = _enrollDialog->exec();
		if (ret == QDialog::Accepted)
		{
			clearScreenInfoAndEmitUpdateSignal();
		}
	}
	else if (objName == "modScreen")
	{
		qInfo() << "getScreenName():" << getScreenName();
		_enrollDialog->setObjectName("modScreenDialog");
		/*<必须选中item>*/
		QListWidgetItem *item = _screenListWidget->currentItem();
		if (!item)
		{
			SCMessageBox* myBox0 = new SCMessageBox(tr("Modify Screen Information Hint"), tr("Please select the screen you want to modify from the list above!"), otsCommon::SCMessageBox::Information, this);
			myBox0->exec();
			myBox0->deleteLater();
			myBox0 = nullptr;
			if (_enrollDialog)
			{
				_enrollDialog->deleteLater();
				_enrollDialog = nullptr;
			}
			return;
		}

		QString screenID = item->data(Qt::UserRole).toString();
		QString screenName = item->data(Qt::DisplayRole).toString();
		bool ScreenType = item->data(Qt::StatusTipRole).toBool();
		_enrollDialog->g_screenID = screenID.toInt();
		_enrollDialog->g_screenName = screenName;
		_enrollDialog->ModScreenDataUI(screenID, screenName, ScreenType);
		int ret = _enrollDialog->exec();
		if (ret == QDialog::Accepted)
		{
			clearScreenInfoAndEmitUpdateSignal();
		}
	}
	else if (objName == "delScreen")
	{
		/*<必须选中item>*/
		QListWidgetItem *item = _screenListWidget->currentItem();
		if (!item)
		{
			SCMessageBox* myBox1 = new SCMessageBox(tr("Delete Screen Hint"), tr("Please select the screen you want to delete from the list above!"), otsCommon::SCMessageBox::Information, this);
			myBox1->exec();
			myBox1->deleteLater();
			myBox1 = nullptr;
			if (_enrollDialog)
			{
				_enrollDialog->deleteLater();
				_enrollDialog = nullptr;
			}
			return;
		}

		SCMessageBox* myBox11 = new SCMessageBox(tr("Delete Screen Hint"), tr("Are you sure to delete current screen?"), otsCommon::SCMessageBox::Warning, this);
		myBox11->exec();

		QString screenId = item->data(Qt::UserRole).toString();

		if (myBox11->result() == QDialog::Accepted)
		{
			int retCode = otsServer::DataSession::instance()->delScreen(screenId.toInt());
			if (retCode)
			{
				qInfo() << (QStringLiteral("删除大屏,ID = %1 成功！！！").arg(screenId.toInt()));
			}
			clearScreenInfoAndEmitUpdateSignal();
		}
		myBox11->deleteLater();
		myBox11 = nullptr;
	}

	if (_enrollDialog)
	{
		_enrollDialog->deleteLater();
		_enrollDialog = nullptr;
	}
}

void ScreenManageWidget::clearScreenInfoAndEmitUpdateSignal()
{
	 _screenId.clear(); 
	 _screenName.clear(); 
	 emit updateScreenData();
}

void ScreenManageWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

	QWidget::paintEvent(event);
}

DecorderInfo _decoderInfo = { "", "", -1, -1 };
QList<DecorderInfo> _decoderList = {};

/*------------ - 录入大屏数据弹框---------- - */
EnrollDataDialog::EnrollDataDialog(QWidget *parent)
:QDialog(parent)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	_decoderList.clear();
	_okPushButton = new QPushButton(tr("OK"), this);
	_cancelPushButton = new QPushButton(tr("Cancel"), this);
	QObject::connect(_okPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
	QObject::connect(_cancelPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
}

void EnrollDataDialog::AddScreenDataUI()
{
	//this->setWindowTitle(QStringLiteral("新增大屏"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("addScreen-hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("addScreen-titleLabel");
	titleLabel->setText(tr("Add Screen"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("add-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_screenNameLineEdit = new QLineEdit(this);
	_screenNameLineEdit->setObjectName("addScreenNameLineEdit");
	_screenRowsLineEdit = new QLineEdit(this);
	_screenRowsLineEdit->setObjectName("addScreenRowsLineEdit");
	_screenColsLineEdit = new QLineEdit(this);
	_screenColsLineEdit->setObjectName("addScreenColsLineEdit");
	_is4KCombobox = new QComboBox(this);
	_is4KCombobox->setObjectName("add-ScreenTypeCombox");
	_screenDecoderWidget = new QWidget(this);
	_screenDecoderWidget->setObjectName("addScreenDecoderWidget");

	QStringList typeList;
	typeList << tr("No") << tr("Yes");
	_is4KCombobox->addItems(typeList);

	_formLayout = new QFormLayout;
	_formLayout->addRow(tr("&Screen Name:"), _screenNameLineEdit);
	_formLayout->addRow(tr("&support 4K:"), _is4KCombobox);
	_formLayout->addRow(tr("&Screen Rows:"), _screenRowsLineEdit);
	_formLayout->addRow(tr("&Screen Cols:"), _screenColsLineEdit);
	_formLayout->addRow(tr("&Decoder Info:"), _screenDecoderWidget);
	vLayout->addLayout(_formLayout);

	QWidget* screenNameLabel = _formLayout->labelForField(_screenNameLineEdit);
	QWidget* screen4KLabel = _formLayout->labelForField(_is4KCombobox);
	QWidget* screenRowsLabel = _formLayout->labelForField(_screenRowsLineEdit);
	QWidget* screenColsLabel = _formLayout->labelForField(_screenColsLineEdit);
	QWidget* screenDecoderLabel = _formLayout->labelForField(_screenDecoderWidget);

	screenNameLabel->setObjectName("addScreenNameLabel");
	screen4KLabel->setObjectName("addScreen4KLabel");
	screenRowsLabel->setObjectName("addScreenRowsLabel");
	screenColsLabel->setObjectName("addScreenColsLabel");
	screenDecoderLabel->setObjectName("addScreenDecoderLabel");

	/*<设置行编辑框的样式>*/
	_screenNameLineEdit->setMaxLength(10);
	_screenNameLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-10 characters.]"));
	_screenRowsLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-3 characters]"));
	_screenColsLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-3 characters]"));
	QRegExp regx("[1-3]{1}$");
	QValidator *validator = new QRegExpValidator(regx, _screenRowsLineEdit);
	_screenRowsLineEdit->setValidator(validator);
	validator = new QRegExpValidator(regx, _screenColsLineEdit);
	_screenColsLineEdit->setValidator(validator);


	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));

	_okPushButton->setObjectName("add-screen");
	_cancelPushButton->setObjectName("cancel-add-screen");
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	connect(_screenNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
	connect(_screenRowsLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(disposeEditingFinishedSlot()));
	connect(_screenColsLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(disposeEditingFinishedSlot()));
	connect(_is4KCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(disposeComboxIndexChangedSlot(int)));
}

void EnrollDataDialog::ModScreenDataUI(QString screenId, QString screenName, int screenType)
{
	//this->setWindowTitle(QStringLiteral("修改大屏信息"));
	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("modScreen-hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("modScreen-titleLabel");
	titleLabel->setText(tr("Modify Screen"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("mod-screenGap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_screenNameLineEdit = new QLineEdit(this);
	_screenNameLineEdit->setObjectName("modScreenNameLineEdit");
	_screenIdLineEdit = new QLineEdit(this);
	_screenIdLineEdit->setObjectName("modScreenIdLineEdit");

	_is4KCombobox = new QComboBox(this);
	_is4KCombobox->setObjectName("mod-ScreenTypeCombox");

	QStringList typeList;
	typeList << tr("No") << tr("Yes");
	_is4KCombobox->addItems(typeList);
	_is4KCombobox->setCurrentIndex((screenType ? 1 : 0));
	_formLayout = new QFormLayout;
	_formLayout->addRow(tr("&Screen Name:"), _screenNameLineEdit);
	_formLayout->addRow(tr("&Screen ID:"), _screenIdLineEdit);
	_formLayout->addRow(tr("&Support 4K:"), _is4KCombobox);

	QWidget* screenNameLabel = _formLayout->labelForField(_screenNameLineEdit);
	QWidget* screenIdLabel = _formLayout->labelForField(_screenIdLineEdit);
	QWidget* screen4KLabel = _formLayout->labelForField(_is4KCombobox);
	screenNameLabel->setObjectName("modScreenNameLabel");
	screenIdLabel->setObjectName("modScreenIdLabel");
	screen4KLabel->setObjectName("modScreen4KLabel");
	vLayout->addLayout(_formLayout);

	_screenNameLineEdit->setText(screenName);
	_screenIdLineEdit->setText(screenId);

	/*<设置行编辑框的样式>*/
	_screenNameLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-10 characters.]"));
	_screenNameLineEdit->setMaxLength(10);
	_screenIdLineEdit->setEnabled(false);
	_is4KCombobox->setEditable(false);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addSpacing(20);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));

	_okPushButton->setObjectName("mod-screen");
	_cancelPushButton->setObjectName("cancel-mod-screen");
	hLayout->addWidget(_okPushButton);
	hLayout->addWidget(_cancelPushButton);
	hLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Preferred));
	vLayout->addLayout(hLayout);
	this->setLayout(vLayout);

	connect(_screenNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
	connect(_is4KCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(disposeComboxIndexChangedSlot(int)));
}

void EnrollDataDialog::disposeEditingFinishedSlot()
{
	QLineEdit *pSender = dynamic_cast<QLineEdit*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	//获取大屏名称、大屏行、列数
	if (pSender == _screenNameLineEdit)
	{
		if (_screenNameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			g_screenName = _screenNameLineEdit->text().trimmed();
			qInfo() << g_screenName;
		}
	}

	else if (pSender == _screenIdLineEdit)
	{
		if (_screenIdLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			g_screenID = _screenIdLineEdit->text().trimmed().toInt();
		}
	}

	else if (pSender == _screenRowsLineEdit)
	{
		if (_screenRowsLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			if (_screenRowsLineEdit->text().trimmed().toInt() < 1)
			{
				//输入错误，请输入不小于1的正整数
				return;
			}
		}
		else
		{
			_decoderList.clear();
			g_screenRow = _screenRowsLineEdit->text().trimmed().toInt();
			if (g_screenRow >= 1)
			{
				if (g_screenCol >= 1)
				{
					qDeleteAll(_screenDecoderWidget->children());
					setDecoderWidget(g_screenRow, g_screenCol);
				}
			}
		}
	}

	else if (pSender == _screenColsLineEdit)
	{
		if (_screenColsLineEdit->text().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_decoderList.clear();
			g_screenCol = _screenColsLineEdit->text().trimmed().toInt();
			if (g_screenRow >= 1)
			{
				if (g_screenCol >= 1)
				{
					qDeleteAll(_screenDecoderWidget->children());
					setDecoderWidget(g_screenRow, g_screenCol);
				}
			}
			else
			{
				//输入错误，请输入不小于1的正整数
			}
		}
	}
	else
	{

	}
}

void EnrollDataDialog::disposeComboxIndexChangedSlot(int index)
{
	qInfo() << "disposeComboxIndexChangedSlot:" << index;
	g_screenType = index;
}

void EnrollDataDialog::disposeButtonSlot()
{
	QPushButton *pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}

	if (pSender == _okPushButton)
	{
		if (_okPushButton->objectName() == "add-screen")
		{
			int screenID = 0;
			std::string screenName = g_screenName.toStdString();
			int screenRows = g_screenRow;
			int screenCols = g_screenCol;
			int screenType = g_screenType;
			//大屏信息 <大屏名字,行数,列数,是否4K>
			std::tuple<std::string, int, int, int > screenInfo;
			screenInfo = std::make_tuple(screenName, screenRows, screenCols, screenType);

			//解码板信息 <解码器ID，行, 列,解码器名称>
			std::vector<std::tuple<int, int, int, std::string> > decoderVector;
			std::tuple<int, int, int, std::string> decoderInfo;
			qInfo() << "listSize:" << _decoderList.size();
			for (int i = 0; i < _decoderList.size(); i++)
			{
				decoderInfo = std::make_tuple(_decoderList[i]._decoderId.toInt(), _decoderList[i]._decoderRow, _decoderList[i]._decoderCol, _decoderList[i]._decoderName.toStdString());
				decoderVector.push_back(decoderInfo);
				qInfo() << "decoderId,decoderRow,decoderCol,decoderName:" << _decoderList[i]._decoderId.toInt() << _decoderList[i]._decoderRow << _decoderList[i]._decoderCol << _decoderList[i]._decoderName;
			}
			//必填提示
			qInfo() << "DecoderInfo:" << screenRows*screenCols << g_screenName << g_screenRow << g_screenCol;
			//if (decoderVector.size() != screenRows * screenCols || g_screenName.isEmpty() || QString::number(g_screenRow).isEmpty() || QString::number(g_screenCol).isEmpty())
			//{
			//	return;
			//}
			if (decoderVector.size() != screenRows * screenCols)
			{
				SCMessageBox* addScreenInfoDlg = new SCMessageBox(tr("Add Screen"), tr("Please make sure that the information filled in is complete!"), otsCommon::SCMessageBox::Information, this);
				addScreenInfoDlg->exec();
				addScreenInfoDlg->deleteLater();
				addScreenInfoDlg = nullptr;
			}

			//for (int decoderIndex = 0; decoderIndex = decoderVector.size(); decoderIndex++)
			//{
			//	if (std::get<0>(decoderVector[decoderIndex]))
			//}
			int retCode = otsServer::DataSession::instance()->addScreenOnce(screenID, screenInfo, decoderVector);
			if (1 == retCode)
			{
				qInfo() << (QStringLiteral("新增大屏，ID = %1 成功！！！").arg(screenID));
				_decoderList.clear();
			}
			else if (retCode == -99)
			{
				//重复
				qInfo() << QStringLiteral("新增大屏成功！！！");
				SCMessageBox* addScreenRepeatDlg = new SCMessageBox(tr("Add Screen Repeated Hint"), tr("Please make sure information you filled in is repeated or not!"), otsCommon::SCMessageBox::Information, this);
				addScreenRepeatDlg->exec();
				addScreenRepeatDlg->deleteLater();
				_decoderList.clear();
				return;
			}
			else
			{
				//0,失败
				qInfo() << QStringLiteral("新增大屏失败！！！");
				SCMessageBox* addScreenFailDlg = new SCMessageBox(tr("Add Screen Failed Hint"), tr("Sorry,Please add Screen again!"), otsCommon::SCMessageBox::Information, this);
				addScreenFailDlg->exec();
				addScreenFailDlg->deleteLater();
				_decoderList.clear();
				return;
			}
		}

		else if (_okPushButton->objectName() == "del-screen")
		{
			int screenID = g_screenID;
			int retCode = otsServer::DataSession::instance()->delScreen(screenID);
			if (retCode)
			{
				qInfo() << (QStringLiteral("删除大屏,ID = %1 成功！！！").arg(screenID));
			}
			else
			{
				qInfo() << QStringLiteral("删除大屏失败！！！");
			}

		}
		else if (_okPushButton->objectName() == "mod-screen")
		{
			int screenID = g_screenID;
			std::string screenName = g_screenName.toStdString();
			int screenType = g_screenType;

			std::tuple<int, std::string, int> info;
			info = std::make_tuple(screenID, screenName, g_screenType);
			//必填提示
			if (g_screenName.isEmpty())
			{
				return;
			}
			int retCode = otsServer::DataSession::instance()->setScreen(screenID, info);
			if (1 == retCode)
			{
				qInfo() << QStringLiteral("修改大屏成功！！！");
			}
			else if (-99 == retCode)
			{
				//重复
				SCMessageBox* modScreenRepeatDlg = new SCMessageBox(tr("Modify Screen Information Repeated Hint"), tr("Please make sure information you filled in is repeated or not!"), otsCommon::SCMessageBox::Information, this);
				modScreenRepeatDlg->exec();
				qInfo() << QStringLiteral("修改大屏信息重复！！！");
				modScreenRepeatDlg->deleteLater();
			}
			else
			{
				//0,失败
				SCMessageBox* modScreenFailDlg = new SCMessageBox(tr("Modify Screen Information Failed Hint"), tr("Sorry,please modify screen information again!"), otsCommon::SCMessageBox::Information, this);
				modScreenFailDlg->exec();
				qInfo() << QStringLiteral("修改大屏失败！！！");
				modScreenFailDlg->deleteLater();
			}
		}
		accept();
	}
	else
	{
		if (_cancelPushButton->objectName() == "cancel-add-screen")
		{
			_decoderInfo._decoderCol = -1;
			_screenNameLineEdit->clear();
			_screenRowsLineEdit->clear();
			_screenColsLineEdit->clear();
		}
		else if (_cancelPushButton->objectName() == "cancel-del-screen")
		{
			g_screenName = "";
			g_screenID = -1;
			g_screenRow = -1;
			g_screenCol = -1;
			g_screenDecoderID = -1;
			g_screenType = -1;
		}
		else if (_cancelPushButton->objectName() == "cancel-mod-screen")
		{
			_screenNameLineEdit->clear();
			_screenIdLineEdit->clear();
		}
		reject();
	}
}

void EnrollDataDialog::setDecoderWidget(const int &nRow, const int &nCol)
{
	QGridLayout* gridLayout = new QGridLayout(_screenDecoderWidget);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->addItem(new QSpacerItem(432, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0, 1, 1);
	gridLayout->addItem(new QSpacerItem(20, 302, QSizePolicy::Minimum, QSizePolicy::Expanding), 0, 1, 1, 1);

	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			QPushButton* pushButton = new QPushButton(_screenDecoderWidget);
			pushButton->setObjectName("screen-decoder-button");
			pushButton->setText(tr("R%1L%2-Decoder").arg(i).arg(j));
			pushButton->setProperty("curRow", i);
			pushButton->setProperty("curCol", j);
			QObject::connect(pushButton, SIGNAL(clicked()), this, SLOT(disposeGetDecoderInfo()));
			gridLayout->addWidget(pushButton, i, j, 1, 1);
			gridLayout->setMargin(5);
		}
	}

	gridLayout->addItem(new QSpacerItem(20, 302, QSizePolicy::Minimum, QSizePolicy::Expanding), nRow + 1, 0, 1, 1);
	gridLayout->addItem(new QSpacerItem(432, 20, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, nCol + 1, 1, 1);

	_screenDecoderWidget->setLayout(gridLayout);
}

void EnrollDataDialog::disposeGetDecoderInfo()
{
	QPushButton *pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}

	QString decoderName = pSender->text();

	qInfo() << "decoderName:" << decoderName;
	int row = pSender->property("curRow").toInt();
	int col = pSender->property("curCol").toInt();

	DecorderInfo info;
	info._decoderName = "";
	info._decoderId = "";
	info._decoderRow = row;
	info._decoderCol = col;

	for (int index = 0; index < _decoderList.size(); index++)
	{
		if (_decoderList[index]._decoderRow == row && _decoderList[index]._decoderCol == col)
		{
			info._decoderName = _decoderList[index]._decoderName;
			info._decoderId = _decoderList[index]._decoderId;
			break;
		}
	}

	_decoderDialog = new DecoderDialog(this);
	_decoderDialog->setWindowTitle(tr("edit-%1").arg(decoderName));
	_decoderDialog->initUI(info);
	int ret = _decoderDialog->exec();
	if (ret == QDialog::Accepted)
	{
		//数据暂存，待点击一级弹框确定按钮后，调用新增接口
		//判断为空操作……
		//不为空
		//写入List
		//解码板ID、解码板名称、所在行、所在列、是否支持4k
		qInfo() << "_decoderInfo:(id,name,row,col:)" << _decoderInfo._decoderId << _decoderInfo._decoderName << _decoderInfo._decoderRow << _decoderInfo._decoderCol;
	}
	if (_decoderDialog)
	{
		_decoderDialog->deleteLater();
		_decoderDialog = nullptr;
	}
}

/*----------录入大屏解码板数据弹框----------*/
DecoderDialog::DecoderDialog(QWidget *parent)
	:QDialog(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
}

DecoderDialog::~DecoderDialog()
{
	if (_decorderNameLineEdit)
	{
		delete _decorderNameLineEdit;
		_decorderNameLineEdit = nullptr;
	}
	if (_decorderIdLineEdit)
	{
		delete _decorderIdLineEdit;
		_decorderIdLineEdit = nullptr;
	}

	if (_decoderRowLineEdit)
	{
		delete _decoderRowLineEdit;
		_decoderRowLineEdit = nullptr;
	}

	if (_decoderColLineEdit)
	{
		delete _decoderColLineEdit;
		_decoderColLineEdit = nullptr;
	}

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
}

/*三级弹框确认按钮*/
void DecoderDialog::disposeButtonSlot()
{
	QPushButton * pSender = dynamic_cast<QPushButton*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is null";
		return;
	}

	QString buttonText = pSender->text();
	if (buttonText == "OK")
	{
		//判断是否为空，完成则执行
		if (_decorderNameLineEdit->text().isEmpty() || _decorderIdLineEdit->text().isEmpty())
		{
			return;
		}

		qInfo() << "decoderInfo:" << _decoderInfo._decoderName << _decoderInfo._decoderId << _decoderInfo._decoderRow << _decoderInfo._decoderCol;
		//_decoderList.push_back(_decoderInfo);

		_decoderList.push_back(_decoderInfo);

		int i = 0;
		for (int decoderIndex = 0; decoderIndex < _decoderList.size(); decoderIndex++)
		{
			qInfo() << "decoderId1:" << _decoderList[decoderIndex]._decoderId;
			qInfo() << "decoderId2:" << _decoderInfo._decoderId;
			if (_decoderList[decoderIndex]._decoderId == _decoderInfo._decoderId)
			{
				++i;
				qInfo() << "i:" << i;
				if (i == 2)
				{
					//重复
					SCMessageBox* addDecoderDlg = new SCMessageBox(tr("Add Screen Information Repeated Hint"), tr("Please make sure the information you filled in is repeated or not!"), otsCommon::SCMessageBox::Information, this);
					addDecoderDlg->exec();
					qInfo() << QStringLiteral("修改大屏信息重复！！！");
					addDecoderDlg->deleteLater();
					_decoderList.removeAt(decoderIndex);
					return;
				}
			}
		}
		
		for (int index = 0; index < _decoderList.size(); index++)
		{
			if (_decoderList[index]._decoderRow == _decoderInfo._decoderRow && _decoderList[index]._decoderCol == _decoderInfo._decoderCol)
			{
				_decoderList.removeAt(index);
				_decoderList.push_back(_decoderInfo);
				break;
			}
			/*else
			{
				_decoderList.push_back(_decoderInfo);
				break;
			}*/
		}

		accept();
	}
	else
	{
		reject();
	}
}

void DecoderDialog::initUI(DecorderInfo info)
{
	QVBoxLayout *vLayout = new QVBoxLayout;
	//vLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Preferred));
	vLayout->setMargin(0);
	QHBoxLayout *hTitleLayout = new QHBoxLayout;
	hTitleLayout->setObjectName("hTitleLayout");
	QLabel *titleLabel = new QLabel(this);
	titleLabel->setObjectName("editDecoder-titleLabel");
	titleLabel->setText(tr("Edit Decoder Info"));
	hTitleLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
	hTitleLayout->addWidget(titleLabel);
	hTitleLayout->addSpacerItem(new QSpacerItem(5, 0, QSizePolicy::Expanding, QSizePolicy::Fixed));
	vLayout->addLayout(hTitleLayout); //加标题

	QFrame *hFrameGap = new QFrame(this);
	hFrameGap->setObjectName("editDecoder-Gap");
	hFrameGap->setFrameShape(QFrame::HLine);
	vLayout->addWidget(hFrameGap);//加间隔线

	_decorderNameLineEdit = new QLineEdit(this);
	_decorderIdLineEdit = new QLineEdit(this);
	_decoderRowLineEdit = new QLineEdit(this);
	_decoderColLineEdit = new QLineEdit(this);
	_okPushButton = new QPushButton(tr("OK"), this);
	_cancelPushButton = new QPushButton(tr("Cancel"), this);
	_okPushButton->setObjectName("decoder-okButton");
	_cancelPushButton->setObjectName("decoder-cancelButton");

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Decoder Name:"), _decorderNameLineEdit);
	formLayout->addRow(tr("&Decoder ID:"), _decorderIdLineEdit);
	formLayout->addRow(tr("&Current Row:"), _decoderRowLineEdit);
	formLayout->addRow(tr("&Current Col:"), _decoderColLineEdit);

	vLayout->addLayout(formLayout);

	_decorderNameLineEdit->setObjectName("edit-decoderNameLineEdit");
	_decorderIdLineEdit->setObjectName("edit-decoderIdLineEdit");
	_decoderRowLineEdit->setObjectName("edit-decoderRowLineEdit");
	_decoderColLineEdit->setObjectName("edit-decoderColLineEdit");

	QWidget* decoderNameLabel = formLayout->labelForField(_decorderNameLineEdit);
	QWidget* decoderIdLabel = formLayout->labelForField(_decorderIdLineEdit);
	QWidget* decoderRowLabel = formLayout->labelForField(_decoderRowLineEdit);
	QWidget* decoderColLabel = formLayout->labelForField(_decoderColLineEdit);

	decoderNameLabel->setObjectName("edit-decoderNameLabel");
	decoderIdLabel->setObjectName("edit-decoderIdLabel");
	decoderRowLabel->setObjectName("edit-decoderRowLabel");
	decoderColLabel->setObjectName("edit-decoderColLabel");


	_decorderNameLineEdit->setMaxLength(10);
	_decorderNameLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-10 characters]"));
	_decorderIdLineEdit->setPlaceholderText(tr("Necessary filling.[limit 1-5 numbers]"));
	QRegExp regx("[0-9]{1,5}$");
	QValidator *validator = new QRegExpValidator(regx, _decorderIdLineEdit);
	_decorderIdLineEdit->setValidator(validator);

	QRegExp regxLayout("[0-3]{1}$");
	validator = new QRegExpValidator(regxLayout, _decoderRowLineEdit);
	validator = new QRegExpValidator(regxLayout, _decoderColLineEdit);
	_decoderRowLineEdit->setValidator(validator);
	_decoderColLineEdit->setValidator(validator);
	_decoderRowLineEdit->setPlaceholderText(tr("Necessary filling.[limit 0-3 numbers]"));
	_decoderColLineEdit->setPlaceholderText(tr("Necessary filling.[limit 0-3 numbers]"));
	_decorderNameLineEdit->setText(info._decoderName);
	_decorderIdLineEdit->setText(info._decoderId);
	_decoderRowLineEdit->setText(QString::number(info._decoderRow));
	_decoderColLineEdit->setText(QString::number(info._decoderCol));

	_decoderRowLineEdit->setEnabled(false);
	_decoderColLineEdit->setEnabled(false);

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

	_decoderInfo._decoderRow = info._decoderRow;
	_decoderInfo._decoderCol = info._decoderCol;

	connect(_decorderNameLineEdit, SIGNAL(editingFinished()), this, SLOT(disposeEditingFinishedSlot()));
	connect(_decorderIdLineEdit, &QLineEdit::textEdited, this, &DecoderDialog::disposeEditingFinishedSlot);
	//connect(_decoderRowLineEdit, &QLineEdit::textEdited, this, &DecoderDialog::disposeEditingFinishedSlot);
	//connect(_decoderColLineEdit, &QLineEdit::textEdited, this, &DecoderDialog::disposeEditingFinishedSlot);

	connect(_okPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
	connect(_cancelPushButton, SIGNAL(clicked()), this, SLOT(disposeButtonSlot()));
}

void DecoderDialog::disposeEditingFinishedSlot()
{
	QLineEdit *pSender = dynamic_cast<QLineEdit*>(sender());
	if (!pSender)
	{
		qInfo() << "pSender is NULL";
		return;
	}
	//解码器名称
	if (pSender == _decorderNameLineEdit)
	{
		if (_decorderNameLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_decoderInfo._decoderName = _decorderNameLineEdit->text().trimmed();
			qInfo() << _decoderInfo._decoderName;
		}
	}
	//解码器ID
	else if (pSender == _decorderIdLineEdit)
	{
		if (_decorderIdLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_decoderInfo._decoderId = _decorderIdLineEdit->text().trimmed();
			qInfo() << _decoderInfo._decoderId;
		}
	}
	//所在行
	else if (pSender == _decoderRowLineEdit)
	{
		if (_decoderRowLineEdit->text().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_decoderInfo._decoderRow = _decoderRowLineEdit->text().trimmed().toInt();
			qInfo() << _decoderInfo._decoderRow;
		}
	}
	//所在列
	else if (pSender == _decoderColLineEdit)
	{
		if (_decoderColLineEdit->text().trimmed().isEmpty())
		{
			//必填项
			return;
		}
		else
		{
			_decoderInfo._decoderCol = _decoderColLineEdit->text().trimmed().toInt();
			qInfo() << _decoderInfo._decoderCol;
		}
	}
	else
	{

	}
}
