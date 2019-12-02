#include "LayoutButton.h"
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <QEvent>

LayoutButton::LayoutButton(LayoutButton::LayoutType type, QWidget *parent) 
:QPushButton(parent), _type(type)
{
    QGridLayout* pLayout = new QGridLayout(this);
    pLayout->setMargin(0);
    pLayout->setSpacing(0);
    QVector<QRect> vectRect = calculatePaintRect();
    QLabel* pLabel;
    for (int i = 0; i < vectRect.size(); i++)
    {
        pLabel = new QLabel(this);
        pLabel->setProperty("n_h_pStatus", 0);
        pLabel->style()->unpolish(pLabel);
        pLabel->style()->polish(pLabel);
        pLayout->addWidget(pLabel, vectRect[i].x(), vectRect[i].y(), vectRect[i].width(), vectRect[i].height());
    }
    QObject::connect(this, SIGNAL(toggled(bool)), this, SLOT(slotStatusChanged(bool)));
}

void LayoutButton::enterEvent(QEvent *event)
{
    int temp = 1;
    if (this->isChecked())
    {
        temp = 2;
    }
    QObjectList objectList = this->children();
    for (int i = 0; i < objectList.size(); i++)
    {
        QLabel* pLabel = qobject_cast<QLabel*>(objectList[i]);
        if (pLabel)
        {
            pLabel->setProperty("n_h_pStatus", temp);
            pLabel->style()->unpolish(pLabel);
            pLabel->style()->polish(pLabel);
        }
    }
    QPushButton::enterEvent(event);
}

void LayoutButton::leaveEvent(QEvent *event)
{
    int temp = 0;
    if (this->isChecked())
    {
        temp = 2;
    }
    QObjectList objectList = this->children();
    for (int i = 0; i < objectList.size(); i++)
    {
        QLabel* pLabel = qobject_cast<QLabel*>(objectList[i]);
        if (pLabel)
        {
            pLabel->setProperty("n_h_pStatus", temp);
            pLabel->style()->unpolish(pLabel);
            pLabel->style()->polish(pLabel);
        }
    }
    QPushButton::leaveEvent(event);
}

void LayoutButton::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			this->setText(this->text());
		}
		break;
	default:
		break;
	}
}
void LayoutButton::slotStatusChanged(bool status)
{
    int temp = 0;
    if (status)
    {
        temp = 2;
    }
    QObjectList objectList = this->children();
    for (int i = 0; i < objectList.size(); i++)
    {
        QLabel* pLabel = qobject_cast<QLabel*>(objectList[i]);
        if (pLabel)
        {
            pLabel->setProperty("n_h_pStatus", temp);
            pLabel->style()->unpolish(pLabel);
            pLabel->style()->polish(pLabel);
        }
    }
}

QVector<QRect> LayoutButton::calculatePaintRect()
{
    QVector<QRect> vectRect;
    vectRect.clear();
    switch (_type)
    {
    case LayoutButton::Model_1:
    {
        vectRect.push_back(QRect(0,0,1,1));
    }
        break;
    case LayoutButton::Model_2H:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
    }
        break;
    case LayoutButton::Model_2V:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
    }
        break;
    case LayoutButton::Model_3:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(0, 2, 1, 1));
    }
        break;
    case LayoutButton::Model_4:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(1, 1, 1, 1));
    }
        break;
    case LayoutButton::Model_6:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(0, 2, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(1, 1, 1, 1));
        vectRect.push_back(QRect(1, 2, 1, 1));
    }
        break;
    case LayoutButton::Model_6L:
    {
        vectRect.push_back(QRect(0, 0, 2, 2));
        vectRect.push_back(QRect(0, 2, 1, 1));
        vectRect.push_back(QRect(1, 2, 1, 1));
        vectRect.push_back(QRect(2, 0, 1, 1));
        vectRect.push_back(QRect(2, 1, 1, 1));
        vectRect.push_back(QRect(2, 2, 1, 1));
    }
        break;
    case LayoutButton::Model_6L2:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 2, 2));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(2, 0, 1, 1));
        vectRect.push_back(QRect(2, 1, 1, 1));
        vectRect.push_back(QRect(2, 2, 1, 1));
    }
        break;
    case LayoutButton::Model_9:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(0, 2, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(1, 1, 1, 1));
        vectRect.push_back(QRect(1, 2, 1, 1));
        vectRect.push_back(QRect(2, 0, 1, 1));
        vectRect.push_back(QRect(2, 1, 1, 1));
        vectRect.push_back(QRect(2, 2, 1, 1));
    }
        break;
    case LayoutButton::Model_13:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(0, 2, 1, 1));
        vectRect.push_back(QRect(0, 3, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(1, 1, 2, 2));
        vectRect.push_back(QRect(1, 3, 1, 1));
        vectRect.push_back(QRect(2, 0, 1, 1));
        vectRect.push_back(QRect(2, 3, 1, 1));
        vectRect.push_back(QRect(3, 0, 1, 1));
        vectRect.push_back(QRect(3, 1, 1, 1));
        vectRect.push_back(QRect(3, 2, 1, 1));
        vectRect.push_back(QRect(3, 3, 1, 1));
    }
        break;
    case LayoutButton::Model_16:
    {
        vectRect.push_back(QRect(0, 0, 1, 1));
        vectRect.push_back(QRect(0, 1, 1, 1));
        vectRect.push_back(QRect(0, 2, 1, 1));
        vectRect.push_back(QRect(0, 3, 1, 1));
        vectRect.push_back(QRect(1, 0, 1, 1));
        vectRect.push_back(QRect(1, 1, 1, 1));
        vectRect.push_back(QRect(1, 2, 1, 1));
        vectRect.push_back(QRect(1, 3, 1, 1));
        vectRect.push_back(QRect(2, 0, 1, 1));
        vectRect.push_back(QRect(2, 1, 1, 1));
        vectRect.push_back(QRect(2, 2, 1, 1));
        vectRect.push_back(QRect(2, 3, 1, 1));
        vectRect.push_back(QRect(3, 0, 1, 1));
        vectRect.push_back(QRect(3, 1, 1, 1));
        vectRect.push_back(QRect(3, 2, 1, 1));
        vectRect.push_back(QRect(3, 3, 1, 1));
    }
        break;
    default:
        break;
    }
    return vectRect;
}
