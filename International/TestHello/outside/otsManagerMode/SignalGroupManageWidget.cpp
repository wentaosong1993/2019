#include "signalgroupManagerTree.h"
#include <QVBoxLayout>
#include <QDebug>
#include "otsServer/DataSession.h"
#include <QTreeWidgetItem>
#include <QPainter>
#include "ui_SignalGroupManageWidget.h"
#include "SignalGroupManageWidget.h"


SignalGroupManageWidget::SignalGroupManageWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SignalGroupManageWidget)
{
	ui->setupUi(this);
	//createUI();
}

SignalGroupManageWidget::~SignalGroupManageWidget()
{
	delete ui;
}
void SignalGroupManageWidget::createUI()
{
	/** 创建信号源组树状图 */
	ui->M_treeWidget->createTreeData();
}

void SignalGroupManageWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QStyleOption styleOpt;
	styleOpt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);

	QWidget::paintEvent(event);
}