#include "SignalGroupTreeWidget.h"
#include "signalgrouptree.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCompleter>
#include <QDebug>
#include "otsServer/DataSession.h"
#include <QStringListModel>
#include <QList>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QToolButton>
#include <QWidgetAction>
#include "ui_SignalGroupTreeWidget.h"

SignalGroupTreeWidget::SignalGroupTreeWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SignalGroupTreeWidget)
{
    _pCompleter = NULL;

    ui->setupUi(this);
    createUI();
}

SignalGroupTreeWidget::~SignalGroupTreeWidget()
{
    delete ui;
}

void SignalGroupTreeWidget::initUIData()
{
    ui->treeWidget->createTreeData();
    /** 创建搜索数据 */
    if (_pCompleter->model())
    {
        _pCompleter->model()->deleteLater();
    }
    _pCompleter->setModel(new QStringListModel(ui->treeWidget->getAllTreeNodeName(), this));
}

void SignalGroupTreeWidget::setDefaultSelectItem()
{
    ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(0));
}

void SignalGroupTreeWidget::changeEvent(QEvent* event)
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

//自动匹配-点击-检索出匹配项，并定位到当前项
void SignalGroupTreeWidget::slotCompleterSelectItem(const QString& text)
{
    QList<QTreeWidgetItem*> findItmes = ui->treeWidget->findItems(text, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
    if (findItmes.size() == 1)
    {
        ui->treeWidget->scrollToItem(findItmes[0]);
        ui->treeWidget->setCurrentItem(findItmes[0]);
    }
}

//回车-检索出匹配项，并定位到当前项
void SignalGroupTreeWidget::slotSelectItem()
{
	QLineEdit *pSender = qobject_cast<QLineEdit*>(sender());
	if (!pSender)
	{
		return;
	}
	QString searchText = pSender->text().trimmed();
    QList<QTreeWidgetItem*> findItmes = ui->treeWidget->findItems(searchText, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive | Qt::MatchContains);
	if (findItmes.size() == 1)
	{
        ui->treeWidget->scrollToItem(findItmes[0]);
        ui->treeWidget->setCurrentItem(findItmes[0]);
	}
}

void SignalGroupTreeWidget::slotSearchClicked()
{
    QString searchText = ui->LineEditSearch->text();
    QList<QTreeWidgetItem*> findItmes = ui->treeWidget->findItems(searchText, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive | Qt::MatchContains);
    if (findItmes.size() == 1)
    {
        ui->treeWidget->scrollToItem(findItmes[0]);
        ui->treeWidget->setCurrentItem(findItmes[0]);
    }
}

void SignalGroupTreeWidget::createUI()
{
    /** 创建搜索框 */
    ui->LineEditSearch->setObjectName("SearchLineEdit");
    ui->SearchButton->setObjectName("SearchToolButton");
    ui->SearchButton->setCursor(Qt::ArrowCursor);//如果不设置鼠标样式，鼠标移动到按钮上依旧显示为编辑框的鼠标样式。
    QObject::connect(ui->SearchButton, SIGNAL(clicked()), this, SLOT(slotSearchClicked()));
    _pCompleter = new QCompleter(this);
    _pCompleter->setFilterMode(Qt::MatchContains);
    _pCompleter->setObjectName("SearchCompleter");
    QAbstractItemView *popup = _pCompleter->popup();
    popup->setObjectName("SearchCompleterView");
    ui->LineEditSearch->setCompleter(_pCompleter);
    QObject::connect(_pCompleter, SIGNAL(activated(const QString&)),
        this, SLOT(slotCompleterSelectItem(const QString&)));

	//QObject::connect(ui->LineEditSearch, SIGNAL(textEdited(const QString &)),
	//	this, SLOT(slotCompleterSelectItem(const QString&)));

    QObject::connect(ui->LineEditSearch, SIGNAL(editingFinished()), this, SLOT(slotSelectItem()));

    /** 创建信号源组树状图 */
    //ui->treeWidget->createTreeData();
    QObject::connect(ui->treeWidget, SIGNAL(signalItemClicked(int, int, QString)),
                    this, SIGNAL(signalItemClicked(int, int, QString)));

    /** 创建搜索数据 */
    _pCompleter->setModel(new QStringListModel(ui->treeWidget->getAllTreeNodeName(), this));
}

void SignalGroupTreeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

    QWidget::paintEvent(event);
}
