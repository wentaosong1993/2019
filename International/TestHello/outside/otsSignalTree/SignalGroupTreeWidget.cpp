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
    /** ������������ */
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

//�Զ�ƥ��-���-������ƥ�������λ����ǰ��
void SignalGroupTreeWidget::slotCompleterSelectItem(const QString& text)
{
    QList<QTreeWidgetItem*> findItmes = ui->treeWidget->findItems(text, Qt::MatchFixedString | Qt::MatchCaseSensitive | Qt::MatchRecursive);
    if (findItmes.size() == 1)
    {
        ui->treeWidget->scrollToItem(findItmes[0]);
        ui->treeWidget->setCurrentItem(findItmes[0]);
    }
}

//�س�-������ƥ�������λ����ǰ��
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
    /** ���������� */
    ui->LineEditSearch->setObjectName("SearchLineEdit");
    ui->SearchButton->setObjectName("SearchToolButton");
    ui->SearchButton->setCursor(Qt::ArrowCursor);//��������������ʽ������ƶ�����ť��������ʾΪ�༭��������ʽ��
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

    /** �����ź�Դ����״ͼ */
    //ui->treeWidget->createTreeData();
    QObject::connect(ui->treeWidget, SIGNAL(signalItemClicked(int, int, QString)),
                    this, SIGNAL(signalItemClicked(int, int, QString)));

    /** ������������ */
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
