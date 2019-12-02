#include "OperationWidget.h"
#include "ui_AddModelWidget.h"
#include "ui_AddPollWidget.h"
#include "ui_AddMirrorWidget.h"
#include "ui_PreviewDialog.h"
#include "otsServer/DataSession.h"
#include <QCheckBox>
#include <QDebug>
#include "otsCommon/MessageBox.h"

using namespace otsCommon;

AddModelWidget::AddModelWidget(QWidget *parent) : QDialog(parent), _ui(new Ui::AddModelWidget)
{
    _ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
}

AddModelWidget::~AddModelWidget()
{
    delete _ui;
}

QString AddModelWidget::getModelName()
{
	return _ui->moduleLineEdit->text();
}

void AddModelWidget::clearContent()
{
	_ui->moduleLineEdit->clear();
}

void AddModelWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			qInfo() << "AddModelWidget:" << "changeLang";
			_ui->retranslateUi(this);
		}
		break;
	default:
		break;
	}
}


void AddModelWidget::slotOk()
{
	if (_ui->moduleLineEdit->text().isEmpty())
	{
	}
	else
	{
		this->accept();
	}
}

void AddModelWidget::slotCancel()
{
	this->reject();
}

/************************************************************/
AddPollWidget::AddPollWidget(QWidget *parent /*= 0*/) : QDialog(parent), _ui(new Ui::AddPollWidget)
{
    _ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
    //this->setWindowTitle(tr("Add Poll Widget"));
    _ui->srcListWidget->setDragDropMode(QAbstractItemView::DragOnly);
    _ui->desListWidget->setAcceptDrops(true);

    QObject::connect(_ui->DelPushButton, SIGNAL(clicked()), this, SLOT(slotDelItem()));
}

AddPollWidget::~AddPollWidget()
{
    delete _ui;
}

QString AddPollWidget::getPollName()
{
    return _ui->nameLineEdit->text();
}

//获取轮训预案中所有ID
std::vector<int> AddPollWidget::getPollModels()
{
    std::vector<int> a;
    int row = 0;
    int modelID;
    while (row < (_ui->desListWidget->count()))
    {
        modelID = _ui->desListWidget->item(row)->data(Qt::UserRole).toInt();
        a.push_back(modelID);
        row++;
    }
    return a;
}

int AddPollWidget::getTimes()
{
    return _ui->timeSpinBox->value();
}

//当前大屏下的预案列表
void AddPollWidget::creatSrcList(int screenID)
{
    _ui->srcListWidget->clear();
    _ui->desListWidget->clear();
	_ui->nameLineEdit->clear();
    std::vector<std::pair<int, std::string> > vecMouldData;
    otsServer::DataSession::instance()->getAllMouldDateByScreenID(screenID, vecMouldData);

    for (int i = 0; i < vecMouldData.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem;
        int id = vecMouldData[i].first;
        QString name = vecMouldData[i].second.c_str();
        item->setData(Qt::UserRole, id);
        item->setData(Qt::DisplayRole, name);
        _ui->srcListWidget->addItem(item);
    }
}


void AddPollWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_ui->retranslateUi(this);
		}
	default:
		break;
	}
}

void AddPollWidget::slotDelItem()
{
    QListWidgetItem* currentItem = _ui->desListWidget->currentItem();
    if (currentItem)
    {
		delete currentItem;
		currentItem = nullptr;
    }
}

void AddPollWidget::slotOK()
{
    if (_ui->nameLineEdit->text().isEmpty() || _ui->desListWidget->count() < 1 )
    {
    }
    else
    {
        this->accept();
    }
}

void AddPollWidget::slotCancel()
{
	_ui->nameLineEdit->clear();
	_ui->timeSpinBox->setValue(1);
    this->reject();
}

AddMirrorWidget::AddMirrorWidget(QWidget *parent) : QDialog(parent), _ui(new Ui::AddMirrorWidget)
{
	_ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	this->setAttribute(Qt::WA_ShowModal, true);
}

AddMirrorWidget::~AddMirrorWidget()
{
    delete _ui;
}

//创建大屏列表
void AddMirrorWidget::creatSrcList(int screenID)
{
	_ui->screenListWidget->clear();
	_ui->mirrorNameLineEdit->clear();
	std::vector<std::pair<int, std::string> > vecScreensData;
	bool retCode = otsServer::DataSession::instance()->getAllScreenData(vecScreensData);
	for (int i = 0; i < vecScreensData.size(); i++)
	{
		int id = vecScreensData[i].first;
		QString name = vecScreensData[i].second.c_str();
		QListWidgetItem* item = new QListWidgetItem;
		item->setData(Qt::UserRole, id);
		QCheckBox *checkBox = new QCheckBox;
		checkBox->setObjectName("mirrorChannelCheckBox");
		checkBox->setCheckable(true);
		checkBox->setChecked(false);
		checkBox->setText(name);
		if (id == screenID)
		{
			checkBox->setChecked(true);
			checkBox->setEnabled(false);
		}
		_ui->screenListWidget->addItem(item);
		_ui->screenListWidget->setItemWidget(item, checkBox);
	}
}


QString AddMirrorWidget::getMirrorName()
{
	_mirrorName = _ui->mirrorNameLineEdit->text();
	return _mirrorName;
}

/* 获取镜像ID数据 */
std::vector<int> AddMirrorWidget::getMirrorData()
{
	std::vector<int> a;
	int row = 0;
	int screenID;
	while (row < (_ui->screenListWidget->count()))
	{
		QListWidgetItem * tempItem = _ui->screenListWidget->item(row);
		QCheckBox* checkBox_ = qobject_cast<QCheckBox*>(_ui->screenListWidget->itemWidget(tempItem));
		if (checkBox_ ->checkState() == Qt::Checked)
		{
			screenID = _ui->screenListWidget->item(row)->data(Qt::UserRole).toInt();
			a.push_back(screenID);
		}
		row++;
	}
	return a;
}

void AddMirrorWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			_ui->retranslateUi(this);
		}
		break;
	default:
		break;
	}
}

void AddMirrorWidget::slotOk()
{
	int row = 0;
	int screenID = 0;
	std::vector<int> a;
	while (row < (_ui->screenListWidget->count()))
	{
		QListWidgetItem * tempItem = _ui->screenListWidget->item(row);
		QCheckBox* checkBox_ = qobject_cast<QCheckBox*>(_ui->screenListWidget->itemWidget(tempItem));
		if (checkBox_->checkState() == Qt::Checked)
		{
			screenID = _ui->screenListWidget->item(row)->data(Qt::UserRole).toInt();
			a.push_back(screenID);
		}
		row++;
	}

	if (_ui->mirrorNameLineEdit->text().isEmpty())
	{
		SCMessageBox* addMirrorDlg = new SCMessageBox(tr("Add Mirror Channel Failed"), tr("The Mirror Channel name field is required!"), otsCommon::SCMessageBox::Information, this);
		addMirrorDlg->exec();
		addMirrorDlg->deleteLater();
		addMirrorDlg = nullptr;
		return;
	}

	if (a.size() < 2)
	{
		SCMessageBox* addMirrorDlg = new SCMessageBox(tr("Add Mirror Channel Failed"), tr("At least two screens are selected to add!"), otsCommon::SCMessageBox::Information, this);
		addMirrorDlg->exec();
		addMirrorDlg->deleteLater();
		addMirrorDlg = nullptr;
		return;
	}
	else
	{
		this->accept();
	}
}

void AddMirrorWidget::slotCancel()
{
	_ui->mirrorNameLineEdit->clear();
	this->reject();
}

/**************************************************/
PreviewDialog::PreviewDialog(QWidget *parent /*= 0*/) : QDialog(parent), ui(new Ui::PreviewDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
}

PreviewDialog::~PreviewDialog()
{
    delete ui;
}

void PreviewDialog::setModel(int modelID)
{
    ui->previewWidget->setScreenModel(modelID);
}

void PreviewDialog::setScreen(int screenID)
{
	ui->previewWidget->slotScreenChanged(screenID);
}


void PreviewDialog::changeEvent(QEvent * event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			ui->retranslateUi(this);
		}
	break;
	default:
		break;
	}
}