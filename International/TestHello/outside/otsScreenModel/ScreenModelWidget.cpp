#include "ScreenModelWidget.h"
#include <QListWidgetItem>
#include <QPushButton>
#include "otsServer/DataSession.h"
#include <QDebug>
#include "ots/Logger.h"
#include "otsXML/globalVar.h"
#include "ui_ScreenModelWidget.h"
#include <QPainter>
#include "otsCommon/MessageBox.h"

/*********************************** ScreenModelWidget *********************************/
ScreenModelWidget::ScreenModelWidget(QWidget *parent /*= 0*/)
:QWidget(parent), ui(new Ui::ScreenModelWidget), _isAutoPreview(true)
{
    ui->setupUi(this);
	ui->label->setText(tr("Screen Module"));
    /*QObject::connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
        this, SLOT(slotListItemChange(QListWidgetItem*, QListWidgetItem*)));*/
    QObject::connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(slotItemClicked(QListWidgetItem*)));
    QObject::connect(ui->add_pushButton, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    QObject::connect(ui->del_pushButton, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    QObject::connect(ui->auto_pushButton, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
}

ScreenModelWidget::~ScreenModelWidget()
{
    delete ui;
}

void ScreenModelWidget::setCurrentItem(QListWidgetItem *item)
{
    ui->listWidget->setCurrentItem(item);
}

void ScreenModelWidget::delSelectedItem()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (item)
    {
        ui->listWidget->setCurrentItem(NULL);
        ui->listWidget->takeItem(ui->listWidget->row(item));
        delete item;
    }
}

//大屏切换时，更新预案列表
void ScreenModelWidget::slotScreenChanged(int screenID)
{
    _screenID = screenID;
    createData(screenID);
    /* 设置当前预案 */
    std::vector<std::tuple<int, std::string>> vecScreenCurrentModelData;
    otsServer::DataSession::instance()->getActiveMouldByScreenID(screenID, vecScreenCurrentModelData);

    if (vecScreenCurrentModelData.size() != 1)
    {
        OTS_LOG->warn("current active model data error");
    }
    else if (vecScreenCurrentModelData.size() == 1)
    {
        OTS_LOG->debug("current active model data ID:{} Name:{}", std::get<0>(vecScreenCurrentModelData[0]), std::get<1>(vecScreenCurrentModelData[0]));
    }
}

void ScreenModelWidget::slotAddModelItem(int modelID, QString modelName)
{
    OTS_LOG->debug("add model item...");
    QListWidgetItem* item = new QListWidgetItem;
    item->setData(Qt::UserRole, modelID);
    item->setData(Qt::DisplayRole, modelName);
    ui->listWidget->insertItem(ui->listWidget->count(), item);
}

void ScreenModelWidget::slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current)
    {
        emit signalScreenModelID(current->data(Qt::UserRole).toInt());
    }
}

void ScreenModelWidget::slotItemClicked(QListWidgetItem *current)
{
    if (current)
    {
        emit signalScreenModelID(current->data(Qt::UserRole).toInt());
    }
}

void ScreenModelWidget::slotButtonClicked()
{
    QString strSenderObject = sender()->objectName();
    if ("add_pushButton" == strSenderObject)
    {
        emit signalSaveModel();
    }
    else if ("del_pushButton" == strSenderObject)
    {
        int count = ui->listWidget->count();
        if (count == 0)
        {
            otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Module Hint"),tr("The current screen has no Module to be deleted."),otsCommon::SCMessageBox::Information,this);
            warning_widget->exec();
            warning_widget->deleteLater();
            return;
        }
        QListWidgetItem* item = ui->listWidget->currentItem();
        if (item)
        {
            emit signalDelModel(item->data(Qt::UserRole).toInt());
        }
        else
        {
            otsCommon::SCMessageBox* warning_widget = new otsCommon::SCMessageBox(tr("Delete Module Hint"), tr("Please select a Module first."), otsCommon::SCMessageBox::Information, this);
            warning_widget->exec();
            warning_widget->deleteLater();
            return;
        }
    }
    else if ("auto_pushButton" == strSenderObject)
    {
        QPushButton* pAutoButton = qobject_cast<QPushButton*>(sender());
        if (pAutoButton)
        {
            QString strText = pAutoButton->text();
            if ("Auto Preview" == strText)
            {
				pAutoButton->setText(tr("First Preview"));
                _isAutoPreview = false;
            }
            else if ("First Preview" == strText)
            {
				pAutoButton->setText(tr("Auto Preview"));
                _isAutoPreview = true;
            }
 /*           else if (QStringLiteral("先预览") == strText)
            {
                pAutoButton->setText(QStringLiteral("自动预览"));
                _isAutoPreview = true;
            }
            else if (QStringLiteral("自动预览") == strText)
            {
                pAutoButton->setText(QStringLiteral("先预览"));
                _isAutoPreview = false;
            }*/
            pAutoButton->style()->unpolish(pAutoButton);
            pAutoButton->style()->polish(pAutoButton);
        }
    }
}

//创建大屏预案模板 
void ScreenModelWidget::createData(int screenID)
{
    ui->listWidget->clear();
    std::vector<std::pair<int, std::string> > vecMouldData;
    otsServer::DataSession::instance()->getAllMouldDateByScreenID(screenID, vecMouldData);

    for (int i = 0; i < vecMouldData.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem;
        int id = vecMouldData[i].first;
        QString name = vecMouldData[i].second.c_str();
        item->setData(Qt::UserRole, id);
        item->setData(Qt::DisplayRole, name);
        ui->listWidget->addItem(item);
    }
}

void ScreenModelWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
    QWidget::paintEvent(event);
}


void ScreenModelWidget::changeEvent(QEvent *event)
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