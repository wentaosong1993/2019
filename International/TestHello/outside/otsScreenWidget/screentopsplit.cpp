#include "screentopsplit.h"
#include <QPainter>
#include <QPen>
#include <QStyleOption>
#include <QDebug>
#include "screenwidget.h"

//单屏界面布局分割方式
ScreenTopSplit::ScreenTopSplit(QWidget *parent) : QWidget(parent)
{
    _row = 1;
    _col = 1;
    _modelType = ScreenWidget::Model_1;
    setAttribute(Qt::WA_TransparentForMouseEvents,true);
}

void ScreenTopSplit::setRowCol(int row, int col)
{
    if (row < 1 || col < 1)
    {
        return;
    }
    _row = row;
    _col = col;
    calculationPoints();
    update();
}

//界面排布比例划分
void ScreenTopSplit::setLayoutModelType(int type)
{
    _modelType = type;
	if (type != 0)
	{
		calculatePaintRect();
		update(); //调用paintEvent(QPaintEvent *event)方法
	}
}

//界面排布尺寸划分
QRect ScreenTopSplit::calculateRect(const QPoint& currentPointf)
{
    int qWidth = this->size().width();
    int qHeight = this->size().height();
    for (std::size_t i = 0; i < _vecLayoutRangeRatio.size(); ++i)
    {
        QRect layRect(_vecLayoutRangeRatio[i].first.x() * qWidth, _vecLayoutRangeRatio[i].first.y() * qHeight,
            (_vecLayoutRangeRatio[i].second.x() - _vecLayoutRangeRatio[i].first.x()) * qWidth, (_vecLayoutRangeRatio[i].second.y() - _vecLayoutRangeRatio[i].first.y()) * qHeight);
        if (layRect.contains(currentPointf))
        {
            return layRect;
        }
    }
    return QRect();
}

QRectF ScreenTopSplit::calculateRectF(const QPointF& currentPointf)
{
    qreal qWidth = this->size().width();
    qreal qHeight = this->size().height();
    for (std::size_t i = 0; i < _vecLayoutRangeRatio.size(); ++i)
    {
        QRectF layRect(_vecLayoutRangeRatio[i].first.x() * qWidth, _vecLayoutRangeRatio[i].first.y() * qHeight,
            (_vecLayoutRangeRatio[i].second.x() - _vecLayoutRangeRatio[i].first.x()) * qWidth, (_vecLayoutRangeRatio[i].second.y() - _vecLayoutRangeRatio[i].first.y()) * qHeight);
        if (layRect.contains(currentPointf))
        {
            return layRect;
        }
    }
    return QRectF();
}

//双击放大时有用
QRect ScreenTopSplit::calculateRectMax(const QRect& rect)
{
    QRect rect_1 = calculateRect(rect.topLeft() + QPoint(3, 3));
    QRect rect_2 = calculateRect(rect.topRight() + QPoint(-3, 3));
    QRect rect_3 = calculateRect(rect.bottomLeft() + QPoint(3, -3));
    QRect rect_4 = calculateRect(rect.bottomRight() + QPoint(-3, -3));

    return (rect_1 | rect_2 | rect_3 | rect_4);
}

QRectF ScreenTopSplit::calculateRectFMax(const QRectF& rectf)
{
    QRectF rect_1 = calculateRectF(rectf.topLeft() + QPoint(3, 3));
    QRectF rect_2 = calculateRectF(rectf.topRight() + QPoint(-3, 3));
    QRectF rect_3 = calculateRectF(rectf.bottomLeft() + QPoint(3, -3));
    QRectF rect_4 = calculateRectF(rectf.bottomRight() + QPoint(-3, -3));

    return (rect_1 | rect_2 | rect_3 | rect_4);
}

//根据界面比例划分，绘制分割后的矩形--界面排布
void ScreenTopSplit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&styleOpt,&painter,this);

    painter.setPen(QPen(QColor(56, 196, 255, 255), 1.2, Qt::DotLine));
    //painter.drawLines(_vecPoint);

    qreal qWidth = this->geometry().width();
    qreal qHeight = this->geometry().height();
    QRectF layRect;

    for (std::size_t i = 0; i < _vecLayoutRangeRatio.size(); ++i)
    {
        painter.setPen(QPen(QColor(56, 196, 255, 255), 1.2, Qt::SolidLine));
		//painter.setPen(QPen(QColor(255,0,0, 255), 3, Qt::SolidLine));
        layRect = QRectF(_vecLayoutRangeRatio[i].first.x() * qWidth, _vecLayoutRangeRatio[i].first.y() * qHeight,
            (_vecLayoutRangeRatio[i].second.x() - _vecLayoutRangeRatio[i].first.x()) * qWidth, (_vecLayoutRangeRatio[i].second.y() - _vecLayoutRangeRatio[i].first.y()) * qHeight);
        painter.drawRect(layRect);
    }
    QWidget::paintEvent(event);
}
//
//void ScreenTopSplit::resizeEvent(QResizeEvent *event)
//{
//    calculationPoints();
//    QWidget::resizeEvent(event);
//}

void ScreenTopSplit::calculationPoints()
{
    _vecPoint.clear();
    qreal width_ = this->width();
    qreal height_ = this->height();
    qreal avg_width = width_ / _col;
    qreal avg_height = height_ / _row;
    for (int i = 1; i < _row; i++)
    {
        _vecPoint.push_back(QPoint(0, i*avg_height));
        _vecPoint.push_back(QPoint(width_, i*avg_height));
    }
    for (int i = 1; i < _col; i++)
    {
        _vecPoint.push_back(QPoint(i*avg_width, 0));
        _vecPoint.push_back(QPoint(i*avg_width, height_));
    }
}

//比例划分数据写入_vecLayoutRangeRatio
void ScreenTopSplit::calculatePaintRect()
{
    _vecLayoutRangeRatio.clear();
    switch (_modelType)
    {
    case ScreenWidget::Model_1:
    {
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(1.0, 1.0)));
    }
        break;
    case ScreenWidget::Model_2H:
    {
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(0.5, 1.0)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.0), QPointF(1.0, 1.0)));
    }
        break;
    case ScreenWidget::Model_2V:
    {
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(1.0, 0.5)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.5), QPointF(1.0, 1.0)));
    }
        break;
    case ScreenWidget::Model_3:
    {
        qreal ratio = 1.0 / 3.0;
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(ratio, 1.0)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio, 0.0), QPointF(ratio * 2.0, 1.0)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(ratio * 2.0, 0.0), QPointF(1.0, 1.0)));
    }
        break;
    case ScreenWidget::Model_4:
    {
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.0), QPointF(0.5, 0.5)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.0), QPointF(1.0, 0.5)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.0, 0.5), QPointF(0.5, 1.0)));
        _vecLayoutRangeRatio.push_back(std::make_pair(QPointF(0.5, 0.5), QPointF(1.0, 1.0)));
    }
        break;
    case ScreenWidget::Model_6:
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
    case ScreenWidget::Model_6L:
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
    case ScreenWidget::Model_6L2:
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
    case ScreenWidget::Model_9:
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
    case ScreenWidget::Model_13:
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
    case ScreenWidget::Model_16:
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
