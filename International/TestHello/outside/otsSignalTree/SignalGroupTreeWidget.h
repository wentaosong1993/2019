#pragma once

#include <QWidget>

namespace Ui
{
    class SignalGroupTreeWidget;
}

class QLineEdit;
class QCompleter;
class SignalGroupTree;
class QTreeWidgetItem;
class SignalGroupTreeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalGroupTreeWidget(QWidget *parent = Q_NULLPTR);
    ~SignalGroupTreeWidget();

    void initUIData();

    void setDefaultSelectItem();

protected:
	void changeEvent(QEvent *event);

signals:
    void signalItemClicked(int ID, int type, QString name);

private slots:
    void slotCompleterSelectItem(const QString& text);
	void slotSelectItem();
    void slotSearchClicked();

private:
    void createUI();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QCompleter* _pCompleter;/**< ËÑË÷½á¹û¿ò */
    Ui::SignalGroupTreeWidget* ui;
};

