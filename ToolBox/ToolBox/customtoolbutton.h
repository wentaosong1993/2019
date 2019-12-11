#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QWidget>

namespace Ui {
class CustomToolButton;
}

class CustomToolButton : public QWidget
{
    Q_OBJECT

public:
    explicit CustomToolButton(QWidget *parent = 0);
    ~CustomToolButton();

public slots:
    void slotButton(bool isChecked);

private:
    Ui::CustomToolButton *ui;
};

#endif // CUSTOMTOOLBUTTON_H
