#include "OperationLayoutWidget.h"
#include "ui_SetCommonLayoutDialog.h"
#include "otsServer/DataSession.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QMimeData>
#include <QDataStream>
#include <QListWidget>
#include <QDrag>
#include <QMessageBox>
#include "LayoutButton.h"
#include "otsCommon/MessageBox.h"

using namespace otsCommon;

DragListWidget::DragListWidget(QWidget *parent)
	:QListWidget(parent)
{
	this->setAcceptDrops(true);
	//m_moduleTypeList.clear();
	m_moduleTypeVector.clear();
}

DragListWidget::~DragListWidget()
{
}

void DragListWidget::mousePressEvent(QMouseEvent *event)
{
	//确保左键拖拽.
	if (event->button() == Qt::LeftButton)
	{
		//先保存拖拽的起点.
		m_dragPoint = event->pos();
		m_dragItem = this->itemAt(event->pos());
	}
	//保留原QListWidget部件的鼠标点击操作.
	QListWidget::mousePressEvent(event);
}

void DragListWidget::mouseMoveEvent(QMouseEvent *event)
{
	//确保按住左键移动.
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint temp = event->pos() - m_dragPoint;
		if (temp.manhattanLength() > QApplication::startDragDistance())
		{
			QDrag *drag = new QDrag(this);
			QMimeData *mimeData = new QMimeData;

			mimeData->setData("moduleType", m_dragItem->data(Qt::UserRole).toByteArray());
			mimeData->setData("moduleName", m_dragItem->text().toUtf8());
			drag->setMimeData(mimeData);
			auto action = drag->exec(Qt::CopyAction | Qt::MoveAction);
		}
	}
	QListWidget::mouseMoveEvent(event);

}

void DragListWidget::dragEnterEvent(QDragEnterEvent *event)
{
	event->setDropAction(Qt::MoveAction);
    this->setCursor(Qt::DragMoveCursor);
	event->accept();
}

void DragListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    this->setCursor(Qt::DragMoveCursor);
    //然后接受事件.这个一定要写.
    event->accept();
}

void DragListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("moduleType"))
    {
        QByteArray exData = event->mimeData()->data("moduleType");
        QByteArray exData2 = event->mimeData()->data("moduleName");

        int type = QString::fromLocal8Bit(exData).toInt();
        QString strName = QString::fromLocal8Bit(exData2);
		qInfo() << "vectorSize:" << m_moduleTypeVector.size();
		if (m_moduleTypeVector.size() < 1)
		{
			m_moduleTypeVector.append(type);
			QListWidgetItem* item = new QListWidgetItem;
			/*LayoutWidget* pLayoutWidget = new LayoutWidget;
			pLayoutWidget->setType((Model_Type)type);
			item->setData(Qt::UserRole, (Model_Type)type);
			item->setText(strName);
			item->setTextAlignment(Qt::AlignCenter);*/
			item->setData(Qt::UserRole, (Model_Type)type);
			item->setText(strName);
			item->setTextAlignment(Qt::AlignCenter);
			LayoutButton* button = new LayoutButton((LayoutButton::LayoutType)type, this);
			button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			button->setCheckable(false);
			this->insertItem(0, item); //每个排布都是插入到index = 0 的位置
			this->setItemWidget(item, button);
		}

		if (!m_moduleTypeVector.contains(type))
		{
			m_moduleTypeVector.append(type);
			QListWidgetItem* item = new QListWidgetItem;
			//LayoutWidget* pLayoutWidget = new LayoutWidget;
			//pLayoutWidget->setType((Model_Type)type);
			//item->setData(Qt::UserRole, (Model_Type)type);
			//item->setText(strName);
			//item->setTextAlignment(Qt::AlignCenter);
			//this->insertItem(0, item); //每个排布都是插入到index = 0 的位置
			//this->setItemWidget(item, pLayoutWidget);
			item->setData(Qt::UserRole, (Model_Type)type);
			item->setText(strName);
			item->setTextAlignment(Qt::AlignCenter);
			LayoutButton* button = new LayoutButton((LayoutButton::LayoutType)type, this);
			button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
			button->setCheckable(false);
			int newRow = this->count();
			this->insertItem(newRow, item); //每个排布都是插入到index = 0 的位置
			this->setItemWidget(item, button);
		}
    }

	event->accept();
    this->setCursor(Qt::ArrowCursor);
	QListWidget::dropEvent(event);
}
SetCommonLayoutDialog::SetCommonLayoutDialog(QWidget *parent /*= 0*/) : QDialog(parent), _ui(new Ui::SetCommonLayoutDialog)
{
    _ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setAttribute(Qt::WA_ShowModal, true);
    this->setWindowTitle(tr("Set Common Layout"));
	_ui->desListWidget->viewport()->installEventFilter(this);
    _ui->srcListWidget->setDragDropMode(QAbstractItemView::DragOnly);
	_ui->srcListWidget->setFlow(QListView::LeftToRight);
	_ui->desListWidget->setFlow(QListView::LeftToRight);
    _ui->desListWidget->setAcceptDrops(true);

    QObject::connect(_ui->DelPushButton, SIGNAL(clicked()), this, SLOT(slotDelItem()));

	//QObject::connect(this, SIGNAL(accepted()), this, SLOT(slotOkButton()));
	QObject::connect(_ui->okButton, SIGNAL(clicked()), this, SLOT(slotOkButton()));
	QObject::connect(_ui->cancelButton, SIGNAL(clicked()), this, SLOT(slotCancelButton()));
//	QObject::connect(_ui->srcListWidget, SIGNAL(), this, SLOT());
}

/*
void SetCommonLayoutDialog::dragEnterEvent(QDragEnterEvent *event)
{
}

void SetCommonLayoutDialog::dropEvent(QDropEvent *event)
{
	qInfo() << event->mimeData();
	__super::dropEvent(event);
}
*/

/*
bool SetCommonLayoutDialog::eventFilter(QObject *obj, QEvent *event)
{
	
	if (obj != this) 
	{
		if (event->type() == QEvent::Drop)
		{
			if (obj->parent())
			{
				QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
				if (dropEvent)
				{
					//qInfo() << "dropEvent:" << dropEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist");
					//qInfo() << "dropEvent data:" << dropEvent->mimeData()->data("application/x-qabstractitemmodeldatalist");

					//QByteArray temp = dropEvent->mimeData()->data("application/x-qabstractitemmodeldatalist");
					//qInfo() << "temp:" << QString::fromLocal8Bit(temp);
					//qInfo() << "tempData:" << QVariant(temp);
					//qInfo() << "dropEvent:" << dropEvent->mimeData()->hasFormat("");
					//qInfo() << "dropEvent:" << dropEvent->mimeData()->hasFormat("");
					//QList<QListWidgetItem*> itemList;
					//QDataStream dataStream(&(dropEvent->mimeData()->data("application/x-qabstractitemmodeldatalist")), QIODevice::ReadOnly);
					//dataStream << itemList;
					//qInfo() << "itemList:" << itemList;
				}
				//this->dropEvent(dropEvent);
				qInfo() << "obj-pparent:" << obj->parent() << "destionWidget:" << _ui->desListWidget;
			}
			else
			{
				qInfo() << "obj:" << obj;
			}
			
			QDropEvent *dropEvent = static_cast<QDropEvent*>(event);
			this->dropEvent(dropEvent);
			return QDialog::eventFilter(obj, event);
		}
		else if (event->type() == QEvent::DragEnter)
		{
			QDragEnterEvent *dropEvent = static_cast<QDragEnterEvent*>(event);
			this->dropEvent(dropEvent);
		}
	}
	return QDialog::eventFilter(obj, event);
}
*/

void SetCommonLayoutDialog::createSrcList()
{
	this->show(); 
	_ui->srcListWidget->clear();
	std::vector<Model_Type> vecModelType;
	vecModelType.push_back(Model_1);
	vecModelType.push_back(Model_2H);
	vecModelType.push_back(Model_2V);
	vecModelType.push_back(Model_3);
	vecModelType.push_back(Model_4);
	vecModelType.push_back(Model_6);
	vecModelType.push_back(Model_6L);
	vecModelType.push_back(Model_6L2);
	vecModelType.push_back(Model_9);
	vecModelType.push_back(Model_13);
	vecModelType.push_back(Model_16);

	//倒序访问……
	for (int i = (vecModelType.size() - 1); i >= 0; i--)
	{
		QListWidgetItem* item = new QListWidgetItem;
		/*LayoutWidget* pLayoutWidget = new LayoutWidget;
		pLayoutWidget->setType(vecModelType[i]);*/
		item->setData(Qt::UserRole, vecModelType[i]);
		item->setText(QString::number(vecModelType[i]));
		item->setTextAlignment(Qt::AlignCenter);
		LayoutButton* button = new LayoutButton((LayoutButton::LayoutType)vecModelType[i], this);
		button->setObjectName("layoutButton");
		button->setAttribute(Qt::WA_TransparentForMouseEvents, true);
		button->setCheckable(true);
		_ui->srcListWidget->insertItem(0, item); //每个排布都是插入到index = 0 的位置
		_ui->srcListWidget->setItemWidget(item, button);
	}

	//QObject::connect(_ui->srcListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
	//	this, SLOT(slotListItemChange(QListWidgetItem*, QListWidgetItem*)));
}

void SetCommonLayoutDialog::saveCommonModelListData()
{
	QList<int> moduleTypeList;
	int row = 0; 
	int moduleType; 
	moduleTypeList.clear();
	
	while (row < _ui->desListWidget->count())
	{
		moduleType = _ui->desListWidget->item(row)->data(Qt::UserRole).toInt();
		moduleTypeList.append(moduleType);
		++row;
	}

	//if (_ui->desListWidget->count() < 1)
	//{
	//	SCMessageBox* myBox1 = new SCMessageBox(QStringLiteral("设置常用排布"), QStringLiteral("设置失败！\n常用排布最少设置1个."), otsCommon::SCMessageBox::Warning, this);
	//	myBox1->exec();
	//	myBox1->deleteLater();
	//	myBox1 = nullptr;
	//	return;
	//}

	/*if (m_moduleTypeVector.size() > 4)
	{
		SCMessageBox* myBox1 = new SCMessageBox(QStringLiteral("设置常用排布"), QStringLiteral("设置失败！\n常用排布限制1-4个."), otsCommon::SCMessageBox::Warning, this);
		myBox1->exec();
		myBox1->deleteLater();
		myBox1 = nullptr;
		return;
	}*/

	//qInfo() << QStringLiteral("应用程序目录：") << QCoreApplication::applicationDirPath();

	QFile file("module.ini");
	if (!file.open(QIODevice::WriteOnly))
		return;

	QTextStream out(&file);

	for (int index = 0; index < moduleTypeList.size(); index++)
	{
		out << moduleTypeList[index] << "\n";
	}
	file.close();
}

void SetCommonLayoutDialog::changeEvent(QEvent *event)
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

void SetCommonLayoutDialog::slotOkButton()
{
	qInfo() << "OK Button Press!!!!";
	if (_ui->desListWidget->count() < 1 || _ui->desListWidget->count() > 4)
	{
		SCMessageBox* myBox1 = new SCMessageBox(tr("Set Common Layout"), tr("Set failed! The common layouts limits are 1-4."), otsCommon::SCMessageBox::Warning, this);
		myBox1->exec();
		myBox1->deleteLater();
		myBox1 = nullptr;
		return;
	}
	saveCommonModelListData();
	emit saveModuleSignal();
}

void SetCommonLayoutDialog::slotCancelButton()
{
	reject();
}

void SetCommonLayoutDialog::slotDelItem()
{
    QListWidgetItem* currentItem = _ui->desListWidget->currentItem();
	//qInfo() << QStringLiteral("常用排布类型,删除前:") << curModuleType << _ui->desListWidget->getModuleTypeVector().size();
    if (currentItem)
    {
		int curModuleType = currentItem->data(Qt::UserRole).toInt();
		if (_ui->desListWidget->getModuleTypeVector().contains(curModuleType))
		{
			//qInfo() << QStringLiteral("常用排布类型,删除后:") << _ui->desListWidget->getModuleTypeList().size() << "state:" << state;
			int index = _ui->desListWidget->getModuleTypeVector().indexOf(curModuleType);
			_ui->desListWidget->getModuleTypeVector().removeAt(index);
			//qInfo() << QStringLiteral("常用排布类型,删除后:") << _ui->desListWidget->getModuleTypeVector().size();
		}
        delete currentItem;
		currentItem = nullptr;
    }
}

LayoutWidget::LayoutWidget(QWidget *parent /*= 0*/) : QWidget(parent)
{
}

void LayoutWidget::setType(int type)
{
	_modelType = type;
	calculatePaintRect();
	update();
}

void LayoutWidget::calculatePaintRect()
{
	_vecLayoutRangeRatio.clear();
	switch (_modelType)
	{
	case Model_1:
	{
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_2H:
	{
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(0.5, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_2V:
	{
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(1.0, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.5), QPointF(1.0, 1.0)));
	}
	break;
	case Model_3:
	{
		qreal ratio = 1.0 / 3.0;
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.0), QPointF(ratio * 2.0, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, 0.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_4:
	{
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(0.5, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.0), QPointF(1.0, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.5), QPointF(0.5, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.5), QPointF(1.0, 1.0)));
	}
	break;
	case Model_6:
	{
		qreal ratio = 1.0 / 3.0;
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.0), QPointF(ratio*2.0, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio*2.0, 0.0), QPointF(1.0, 0.5)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.5), QPointF(ratio, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.5), QPointF(ratio*2.0, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio*2.0, 0.5), QPointF(1.0, 1.0)));
	}
	break;
	case Model_6L:
	{
		qreal ratio = 1.0 / 3.0;
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio * 2.0, ratio * 2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, 0.0), QPointF(1.0, ratio)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, ratio), QPointF(1.0, ratio * 2)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, ratio * 2.0), QPointF(ratio, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, ratio * 2.0), QPointF(ratio * 2.0, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, ratio * 2.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_6L2:
	{
		qreal ratio = 1.0 / 3.0;
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.0), QPointF(1.0, ratio * 2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio, ratio)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, ratio), QPointF(ratio, ratio * 2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, ratio * 2.0), QPointF(ratio, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, ratio * 2.0), QPointF(ratio * 2.0, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, ratio * 2.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_9:
	{
		qreal ratio = 1.0 / 3.0;
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio, ratio)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.0), QPointF(ratio * 2.0, ratio)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, 0.0), QPointF(1.0, ratio)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, ratio), QPointF(ratio, ratio*2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, ratio), QPointF(ratio * 2.0, ratio*2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, ratio), QPointF(1.0, ratio*2.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, ratio * 2.0), QPointF(ratio, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, ratio * 2.0), QPointF(ratio * 2.0, 1.0)));
		_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, ratio * 2.0), QPointF(1.0, 1.0)));
	}
	break;
	case Model_13:
	{
		qreal ratio = 1.0 / 4.0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (i == 1 && j == 1)
				{
					_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(i * ratio, j* ratio), QPointF(i * ratio + ratio * 2.0, j * ratio + ratio * 2.0)));
					continue;
				}
				if ((i == 1 && j == 2) || (i == 2 && j == 1) || (i == 2 && j == 2))
				{
					continue;
				}
				_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(i * ratio, j* ratio), QPointF(i * ratio + ratio, j * ratio + ratio)));
			}
		}
	}
	break;
	case Model_16:
	{
		qreal ratio = 1.0 / 4.0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				_vecLayoutRangeRatio.push_back(std::make_pair(QPointF(i * ratio, j* ratio), QPointF(i * ratio + ratio, j * ratio + ratio)));
			}
		}
	}
	break;
	default:
		break;
	}
}

void LayoutWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
		QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

	qreal qWidth = this->geometry().width();
	qreal qHeight = this->geometry().height();
	QRectF layRect;

	for (std::size_t i = 0; i < _vecLayoutRangeRatio.size(); ++i)
	{
		painter.setPen(QPen(QColor(56, 196, 255, 255), 1.2, Qt::SolidLine));
		layRect = QRectF(_vecLayoutRangeRatio[i].first.x() * qWidth, _vecLayoutRangeRatio[i].first.y() * qHeight,
			(_vecLayoutRangeRatio[i].second.x() - _vecLayoutRangeRatio[i].first.x()) * qWidth, (_vecLayoutRangeRatio[i].second.y() - _vecLayoutRangeRatio[i].first.y()) * qHeight);
		painter.drawRect(layRect);
	}
	QWidget::paintEvent(event);
}

void LayoutWidget::resizeEvent(QResizeEvent *event)
{
	//calculatePaintRect();
	QWidget::resizeEvent(event);
}
