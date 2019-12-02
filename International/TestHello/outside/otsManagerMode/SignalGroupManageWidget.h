#pragma once
#include <QWidget>

namespace Ui
{
	class SignalGroupManageWidget;
}

class SignalGroupTreeWidget;
class QTreeWidgetItem;

class SignalGroupManageWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SignalGroupManageWidget(QWidget *parent = Q_NULLPTR);
	~SignalGroupManageWidget();

public:
	void createUI();

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::SignalGroupManageWidget* ui;
};
