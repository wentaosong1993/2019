#pragma once

#include <QPushButton>
#include <QVector>
#include <QRect>

class LayoutButton :public QPushButton
{
    Q_OBJECT
public:
    enum LayoutType
    {
        Model_1 = 1,
        Model_2H = 2,
        Model_2V = 3,
        Model_3 = 4,
        Model_4 = 5,
        Model_6 = 6,
        Model_6L = 7,
        Model_6L2 = 8,
        Model_9 = 9,
        Model_13 = 10,
        Model_16 = 11
    };
public:
    explicit LayoutButton(LayoutButton::LayoutType type, QWidget *parent = Q_NULLPTR);

    int getLayoutButtonType(){ return _type; }

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
	void changeEvent(QEvent *event);

private slots:
    void slotStatusChanged(bool status);

private:
    QVector<QRect> calculatePaintRect();

private:
    LayoutButton::LayoutType _type;
};