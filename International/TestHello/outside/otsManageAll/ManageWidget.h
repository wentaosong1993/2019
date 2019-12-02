#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include <QWidget>

namespace Ui {
class ManageWidget;
}

class GUIEvent;
class ManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManageWidget(QWidget *parent = 0);
    ~ManageWidget();

protected:
	void changeEvent(QEvent *event);

protected:
    virtual bool event(QEvent *event);
    virtual void GUIChanged(GUIEvent* event);

private:
    Ui::ManageWidget *ui;
};

#endif // MANAGEWIDGET_H
