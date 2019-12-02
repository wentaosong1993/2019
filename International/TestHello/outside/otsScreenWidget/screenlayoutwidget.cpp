#include "screenlayoutwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTouchEvent>
#include <QWheelEvent>
#include <QCursor>
#include <QDebug>
#include "otsDecoderVideo/ffmpeg_h264.h"
#include "ots/Common.h"
#include "screenwidget.h"
#include "ots/Logger.h"
#include "screenwidget.h"

/******************************************************************************/
ScreenLayoutWidget::ScreenLayoutWidget(QWidget *parent) : QWidget(parent)
{
}

//屏幕尺寸转换
void ScreenLayoutWidget::setRatio(QPointF pointRatio)
{
    _pointRatio = pointRatio;
    for (int i = 0; i < _vecScreenLayoutItems.size(); i++)
    {
        _vecScreenLayoutItems[i]->setRatio(_pointRatio);
    }
}

//将当前布局和编码板ID加入到大屏布局中
bool ScreenLayoutWidget::addScreenLayoutItem(QRect rect, int encoderID)
{
    ScreenLayoutItem* temp = new ScreenLayoutItem(this);
    temp->setEncoderID(encoderID);
    temp->setRatio(_pointRatio);
    temp->initOriginalRect(rect);
    temp->setVirtualRect(rect);
    temp->show();
    QObject::connect(temp, SIGNAL(signalRemoveItem()), this, SLOT(slotRemoveScreenLayoutItem()));
    QObject::connect(temp, SIGNAL(signalRectChanged(QRect)), this, SLOT(slotItemRectChanged(QRect)));
    _vecScreenLayoutItems.push_back(temp);

    QWidget* parent = this->parentWidget();
    if (parent)
    {
        ScreenWidget* parent_Screen = qobject_cast<ScreenWidget*>(parent);
        if (parent_Screen)
        {
            parent_Screen->calculateNewRect(temp, temp->geometry());
        }
    }
    //temp->sendSignal();
    return true;
}

bool ScreenLayoutWidget::addScreenLayoutItem(QRectF rect, int encoderID /*= 0*/)
{
    ScreenLayoutItem* temp = new ScreenLayoutItem(this);
    temp->setEncoderID(encoderID);
    temp->setRatio(_pointRatio);
    temp->initOriginalRectF(rect);
    temp->setVirtualRectF(rect);
    temp->show();
    QObject::connect(temp, SIGNAL(signalRemoveItem()), this, SLOT(slotRemoveScreenLayoutItem()));
    QObject::connect(temp, SIGNAL(signalRectFChanged(QRectF)), this, SLOT(slotItemRectFChanged(QRectF)));
    _vecScreenLayoutItems.push_back(temp);

    QWidget* parent = this->parentWidget();
    if (parent)
    {
        ScreenWidget* parent_Screen = qobject_cast<ScreenWidget*>(parent);
        if (parent_Screen)
        {
            parent_Screen->calculateNewRectF(temp, temp->getGeometry());
        }
    }
    //temp->sendSignal();
    return true;
}

//删除当前大屏布局item
void ScreenLayoutWidget::slotRemoveScreenLayoutItem()
{
    ScreenLayoutItem* item;
    if (sender())
    {
        item = qobject_cast<ScreenLayoutItem*>(sender());
        if (!item)
        {
            return;
        }
        _vecScreenLayoutItems.removeOne(item);
        //item->deleteLater();
        emit signalLayoutChanged();
    }
}

void ScreenLayoutWidget::slotItemRectChanged(QRect rect)
{
    qDebug() << "sender:" << sender() << ",rect:" << rect;
    emit signalItemRectChanged(sender(), rect);
}

void ScreenLayoutWidget::slotItemRectFChanged(QRectF rectf)
{
    qDebug() << "sender:" << sender() << ",rect:" << rectf;
    emit signalItemRectFChanged(sender(), rectf);
}

//清除大屏布局
void ScreenLayoutWidget::cleanScreenLayoutItems()
{
    qDeleteAll(_vecScreenLayoutItems);
    _vecScreenLayoutItems.clear();
}
/** 移除大屏中的Item 从数组删除 */
bool ScreenLayoutWidget::remvoeItemFromVector(ScreenLayoutItem* item)
{
    return _vecScreenLayoutItems.removeOne(item);
}

bool ScreenLayoutWidget::existItem(ScreenLayoutItem* indexOf)
{
    if (_vecScreenLayoutItems.indexOf(indexOf) == -1)
    {
        return false;
    }
    return true;
}

void ScreenLayoutWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&styleOpt,&painter,this);
    QWidget::paintEvent(event);
}

void ScreenLayoutWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

/******************************************************************************/

ScreenLayoutItem::ScreenLayoutItem(QWidget *parent) : QWidget(parent)
{
    _id = 0;
    _encoderID = 0;
    _strEncoderName = "";
    _bIsTwoPoint = false;
    _touchIsRelease = true;
    _mousePress = QPoint();
    _pos = QPoint();
    _space = 20;
    _mouseType = Mouse_Center;
    this->setAttribute(Qt::WA_MouseTracking,true);
    this->startTimer(15);
}

void ScreenLayoutItem::setRealityRect(QRect rect)
{
    if (_realityRect == rect)
    {
        return;
    }
    _realityRect = rect;
	QRect rect1 = Reality2VirtualRect(_realityRect, _pointRatio);
    this->setGeometry(rect1);
}

void ScreenLayoutItem::setVirtualRect(QRect rect)
{
    this->setGeometry(rect);
    _realityRect = Virtual2RealityRect(rect, _pointRatio);
}

void ScreenLayoutItem::setRatio(QPointF pointfRatio)
{
    if (_pointRatio == pointfRatio)
    {
        return;
    }
    _pointRatio = pointfRatio;
#ifdef OLD
    qDebug() << "_realityRect:" << _realityRect;
    QRect rect = Reality2VirtualRect(_realityRect, _pointRatio);
    this->setGeometry(rect);
#else
    qDebug() << "_realityRectF:" << _realityRectF;
    _currentRectF = Reality2VirtualRectF(_realityRectF, _pointRatio);
    this->setGeometry(_currentRectF.toRect());
#endif
}

void ScreenLayoutItem::initOriginalRect(QRect rect)
{
    _originalRect = rect;
}

void ScreenLayoutItem::sendSignal()
{
#ifdef OLD
    emit signalRectChanged(this->geometry());
#else
    emit signalRectFChanged(_currentRectF);
#endif
}

//绘制边缘环鼠标样式变换区间
void ScreenLayoutItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&styleOpt,&painter,this);

    ots::g_DecoderLock.lockForRead();
    if ((ots::g_mapDeviceID2H264Decoder.find(_encoderID) != ots::g_mapDeviceID2H264Decoder.end()) && ots::g_mapDeviceID2H264Decoder.find(_encoderID)->second->isStart())
    {
        painter.drawImage(this->rect(), ots::g_mapDeviceID2H264Decoder.find(_encoderID)->second->getImage().scaled(this->size()));
    }
    else
    {
        painter.fillRect(this->rect(), Qt::black);
        painter.save();
        painter.setPen(Qt::red);
        painter.drawText(this->rect(),Qt::AlignCenter,tr("No Signal..."));
        painter.restore();
    }

    ots::g_DecoderLock.unlock();

    painter.drawRect(this->rect()+QMargins(0,0,-1,-1));
    painter.drawRect(this->rect()+QMargins(-_space,-_space,-_space-1,-_space-1));
    QWidget::paintEvent(event);
}

void ScreenLayoutItem::resizeEvent(QResizeEvent *event)
{
    //if (this->parent())
    //{
    //    if (this->parent()->parent())
    //    {
    //        ScreenWidget* pScreenWidget = qobject_cast<ScreenWidget*>(this->parent()->parent());
    //        _mapDecoderRect.clear();
    //        QRect gemo = this->geometry();
    //        pScreenWidget->calculationRect2DecoderRect(this->geometry(), _mapDecoderRect);
    //    }
    //}
    QWidget::resizeEvent(event);
}

void ScreenLayoutItem::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setCursor(Qt::ArrowCursor);
    QWidget::leaveEvent(event);
}

void ScreenLayoutItem::mousePressEvent(QMouseEvent *event)
{
#ifdef OLD
    if (event->buttons()&Qt::LeftButton)
    {
        _mousePress = QCursor::pos();
        _pos = this->pos();
        _oldRect = this->geometry();
        this->raise();
        calculationMouseType();
    }
#else
    if (event->buttons()&Qt::LeftButton)
    {
        _mousePress = QCursor::pos();
        _posF = _currentRectF.topLeft();
        _oldRectF = _currentRectF;
        this->raise();
        calculationMouseType();
    }
#endif
    
    QWidget::mousePressEvent(event);
}

void ScreenLayoutItem::mouseMoveEvent(QMouseEvent *event)
{
#ifdef OLD
    if (!_bIsTwoPoint)
    {
        if (event->buttons()&Qt::LeftButton)
        {
            QSize new_size = _oldRect.size();
            QPoint new_pos = _pos;
            switch (_mouseType) {
            case Mouse_Center:
                new_pos.setX((_pos + QCursor::pos() - _mousePress).x());
                new_pos.setY((_pos + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_Left:
                new_size.setWidth(_oldRect.width() + (_mousePress - QCursor::pos()).x());
                new_pos.setX((_pos + QCursor::pos() - _mousePress).x());
                break;
            case Mouse_Right:
                new_size.setWidth(_oldRect.width() - (_mousePress - QCursor::pos()).x());
                break;
            case Mouse_Top:
                new_size.setHeight(_oldRect.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setY((_pos + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_Bottom:
                new_size.setHeight(_oldRect.height() - (_mousePress - QCursor::pos()).y());
                break;
            case Mouse_LeftTop:
                new_size.setWidth(_oldRect.width() + (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRect.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setX((_pos + QCursor::pos() - _mousePress).x());
                new_pos.setY((_pos + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_RightBottom:
                new_size.setWidth(_oldRect.width() - (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRect.height() - (_mousePress - QCursor::pos()).y());
                break;
            case Mouse_LeftBottom:
                new_size.setWidth(_oldRect.width() + (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRect.height() - (_mousePress - QCursor::pos()).y());
                new_pos.setX((_pos + QCursor::pos() - _mousePress).x());
                break;
            case Mouse_RightTop:
                new_size.setWidth(_oldRect.width() - (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRect.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setY((_pos + QCursor::pos() - _mousePress).y());
                break;
            default:
                break;
            }
            this->setGeometry(QRect(new_pos, new_size));
        }
        else
        {
            calculationMouseType();
        }
    }
#else
    if (!_bIsTwoPoint)
    {
        if (event->buttons()&Qt::LeftButton)
        {
            QSizeF new_size = _oldRectF.size();
            QPointF new_pos = _posF;
            switch (_mouseType) {
            case Mouse_Center:
                new_pos.setX((_posF + QCursor::pos() - _mousePress).x());
                new_pos.setY((_posF + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_Left:
                new_size.setWidth(_oldRectF.width() + (_mousePress - QCursor::pos()).x());
                new_pos.setX((_posF + QCursor::pos() - _mousePress).x());
                break;
            case Mouse_Right:
                new_size.setWidth(_oldRectF.width() - (_mousePress - QCursor::pos()).x());
                break;
            case Mouse_Top:
                new_size.setHeight(_oldRectF.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setY((_posF + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_Bottom:
                new_size.setHeight(_oldRectF.height() - (_mousePress - QCursor::pos()).y());
                break;
            case Mouse_LeftTop:
                new_size.setWidth(_oldRectF.width() + (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRectF.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setX((_posF + QCursor::pos() - _mousePress).x());
                new_pos.setY((_posF + QCursor::pos() - _mousePress).y());
                break;
            case Mouse_RightBottom:
                new_size.setWidth(_oldRectF.width() - (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRectF.height() - (_mousePress - QCursor::pos()).y());
                break;
            case Mouse_LeftBottom:
                new_size.setWidth(_oldRectF.width() + (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRectF.height() - (_mousePress - QCursor::pos()).y());
                new_pos.setX((_posF + QCursor::pos() - _mousePress).x());
                break;
            case Mouse_RightTop:
                new_size.setWidth(_oldRectF.width() - (_mousePress - QCursor::pos()).x());
                new_size.setHeight(_oldRectF.height() + (_mousePress - QCursor::pos()).y());
                new_pos.setY((_posF + QCursor::pos() - _mousePress).y());
                break;
            default:
                break;
            }
            _currentRectF = QRectF(new_pos, new_size);
            this->setGeometry(_currentRectF.toRect());
        }
        else
        {
            calculationMouseType();
        }
    }
#endif

    QWidget::mouseMoveEvent(event);
}

void ScreenLayoutItem::mouseReleaseEvent(QMouseEvent *event)
{
#ifdef OLD
    /*
    1.判断是否需要贴边处理 64px
    2.裁剪计算器所分割再解码器中的位置大小
    */

    if (this->parentWidget() && !this->parentWidget()->geometry().contains(mapToParent(event->pos())) && _mouseType == Mouse_Center)
    {
        emit signalRemoveItem();
    }
    else
    {
        OTS_LOG->debug("send signal recr changed...");
        _realityRect = Virtual2RealityRect(this->geometry(), _pointRatio);
        emit signalRectChanged(this->geometry());

        QPoint diff = _mousePress - QCursor::pos();
        if ((abs(diff.x()) > 10) || (abs(diff.y()) > 10))
        {
            _originalRect = this->geometry();
        }
    }
#else
    /*
    1.判断是否需要贴边处理 64px
    2.裁剪计算器所分割再解码器中的位置大小
    */

    if (this->parentWidget() && !this->parentWidget()->geometry().contains(mapToParent(event->pos())) && _mouseType == Mouse_Center)
    {
        emit signalRemoveItem();
    }
    else
    {
        OTS_LOG->debug("send signal recr changed...");
        _realityRectF = Virtual2RealityRectF(_currentRectF, _pointRatio);
        emit signalRectFChanged(_currentRectF);

        QPoint diff = _mousePress - QCursor::pos();
        if ((abs(diff.x()) > 10) || (abs(diff.y()) > 10))
        {
            _originalRectF = _currentRectF;
        }
    }
#endif

    QWidget::mouseReleaseEvent(event);
}

void ScreenLayoutItem::mouseDoubleClickEvent(QMouseEvent *event)
{
#ifdef OLD
    int diffwidth = this->geometry().width() - _originalRect.width();
    int diffheight = this->geometry().height() - _originalRect.height();
    if ((abs(diffwidth) > 10) && (abs(diffheight) > 10))/* 计算怕存在一定误差所以进行了差值计算 */
    {
        this->setVirtualRect(_originalRect);
    }
    else if (this->parentWidget() && this->parentWidget()->parentWidget())
    {
        ScreenWidget* pGrandParent = qobject_cast<ScreenWidget*>(this->parentWidget()->parentWidget());
        if (pGrandParent)
        {
            QRect rect = pGrandParent->calculateRectMax(this->geometry());
            this->setVirtualRect(rect);
        }
    }
#else
    qreal diffwidth = _currentRectF.width() - _originalRectF.width();
    qreal diffheight = _currentRectF.height() - _originalRectF.height();
    if ((abs(diffwidth) > 10) && (abs(diffheight) > 10))/* 计算怕存在一定误差所以进行了差值计算 */
    {
        this->setVirtualRectF(_originalRectF);
    }
    else if (this->parentWidget() && this->parentWidget()->parentWidget())
    {
        ScreenWidget* pGrandParent = qobject_cast<ScreenWidget*>(this->parentWidget()->parentWidget());
        if (pGrandParent)
        {
            QRectF rect = pGrandParent->calculateRectFMax(_currentRectF);
            this->setVirtualRectF(rect);
        }
    }
#endif

    QWidget::mouseDoubleClickEvent(event);
}

void ScreenLayoutItem::wheelEvent(QWheelEvent *event)
{
#ifdef OLD
    qreal currentScaleFactor = 0.8;
    if (event->angleDelta().y() > 0)
    {
        currentScaleFactor = 1.2;
    }

    QRectF rectf = calculateScaleRectF(this->geometry(), currentScaleFactor);
    this->setVirtualRect(rectf.toRect());
    emit signalRectChanged(this->geometry());
#else
    qreal currentScaleFactor = 0.8;
    if (event->angleDelta().y() > 0)
    {
        currentScaleFactor = 1.2;
    }

    QRectF rectf = calculateScaleRectF(_currentRectF, currentScaleFactor);
    this->setVirtualRectF(rectf);
    emit signalRectFChanged(_currentRectF);
#endif

    QWidget::wheelEvent(event);
}

void ScreenLayoutItem::timerEvent(QTimerEvent *event)
{
    update();
    QWidget::timerEvent(event);
}

bool ScreenLayoutItem::event(QEvent *event)
{
#ifdef OLD
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        if (touchEvent)
        {
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            if (touchPoints.count() == 2) {
                if (_touchIsRelease)
                {
                    _oldRect = this->geometry();
                    _touchIsRelease = false;
                }
                _bIsTwoPoint = true;/* 两指时不让移动 */
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
                QRectF rectf = calculateScaleRectF(_oldRect, currentScaleFactor);
                this->setVirtualRect(rectf.toRect());
            }
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased&&touchPoints.count() == 1 && !_touchIsRelease) {/** 释放之后才进入 */
                emit signalRectChanged(this->geometry());
                _touchIsRelease = true;
                _bIsTwoPoint = false;
            }
        }
        return true;/** 必须返回true否则无法使用 */
    }
    default:
        break;
    }
#else
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        if (touchEvent)
        {
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            if (touchPoints.count() == 2) {
                if (_touchIsRelease)
                {
                    _oldRectF = _currentRectF;
                    _touchIsRelease = false;
                }
                _bIsTwoPoint = true;/* 两指时不让移动 */
                const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
                const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
                qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
                QRectF rectf = calculateScaleRectF(_oldRectF, currentScaleFactor);
                this->setVirtualRectF(rectf);
            }
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased&&touchPoints.count() == 1 && !_touchIsRelease) {/** 释放之后才进入 */
                emit signalRectFChanged(_currentRectF);
                _touchIsRelease = true;
                _bIsTwoPoint = false;
            }
        }
        return true;/** 必须返回true否则无法使用 */
    }
    default:
        break;
    }
#endif
    
    return QWidget::event(event);
}

void ScreenLayoutItem::calculationMouseType()
{
    _mouseType = Mouse_Center;
    QPoint mousePoint = mapFromGlobal(QCursor::pos());
    if(mousePoint.x()<_space)
    {
        _mouseType|=Mouse_Left;
    }
    else if(mousePoint.x()+_space>this->width())
    {
        _mouseType|=Mouse_Right;
    }

    if(mousePoint.y()<_space)
    {
        _mouseType|=Mouse_Top;
    }
    else if(mousePoint.y()+_space>this->height())
    {
        _mouseType|=Mouse_Bottom;
    }

    switch (_mouseType) {
    case Mouse_Center:
        this->setCursor(Qt::SizeAllCursor);
        break;
    case Mouse_Left:
    case Mouse_Right:
        this->setCursor(Qt::SizeHorCursor);
        break;
    case Mouse_Top:
    case Mouse_Bottom:
        this->setCursor(Qt::SizeVerCursor);
        break;
    case Mouse_LeftTop:
    case Mouse_RightBottom:
        this->setCursor(Qt::SizeFDiagCursor);
        break;
    case Mouse_LeftBottom:
    case Mouse_RightTop:
        this->setCursor(Qt::SizeBDiagCursor);
        break;
    default:
        this->setCursor(Qt::ArrowCursor);
        break;
    }
}

QRectF ScreenLayoutItem::calculateScaleRectF(const QRectF& oldRectf, const qreal scaleFactor)
{
    QPointF centerPoint = oldRectf.center();
    qreal oldWidth = oldRectf.width();
    qreal oldHeight = oldRectf.height();
    qreal scaleFactor_two = 0.15;/** 第二个比例因子，用来计算缩放大小的 */
    qreal scaleFactor_last = 1.0 - scaleFactor_two + scaleFactor * scaleFactor_two;/** 计算的最终比例因子 */
    qreal newWidth = oldWidth * scaleFactor_last;
    qreal newHeight = oldHeight * scaleFactor_last;

    if (newWidth < _minSize * _pointRatio.x())
        newWidth = _minSize * _pointRatio.x();
    if (newHeight < _minSize * _pointRatio.x())
        newHeight = _minSize * _pointRatio.x();

    QRectF temp = QRectF(centerPoint - QPointF(newWidth / 2, newHeight / 2), QSizeF(newWidth, newHeight));

    if (temp.left() < 0)
    {
        temp.setLeft(0);
    }
    if (temp.top() < 0)
    {
        temp.setTop(0);
    }
    if (temp.right() > this->parentWidget()->rect().right())
    {
        temp.setRight(this->parentWidget()->rect().right());
    }
    if (temp.bottom() > this->parentWidget()->rect().bottom())
    {
        temp.setBottom(this->parentWidget()->rect().bottom());
    }

    return temp;
}

QRect ScreenLayoutItem::Reality2VirtualRect(QRect realityRect, QPointF pointRatio)
{
    int realityWidth = realityRect.width();
    int realityHeight = realityRect.height();
    int realityX = realityRect.left();
    int realityY = realityRect.top();

    return QRect(realityX*pointRatio.x(), realityY*pointRatio.y(), realityWidth*pointRatio.x(), realityHeight*pointRatio.y());
}

int getNumforT_1(int num, int T)
{
    int result = num;
    int temp = num % T;
    if (temp != 0)
    {
        qreal median = T / 2.0;
        if (temp >= median)
        {
            result += (T-temp);
        }
        else
        {
            result -= temp;
        }
    }
    return result;
}
QRect ScreenLayoutItem::Virtual2RealityRect(QRect virtualRect, QPointF pointRatio)
{
    int virtualWidth = virtualRect.width();
    int virtualHeight = virtualRect.height();
    int virtualX = virtualRect.left();
    int virtualY = virtualRect.top();

#define OLD_CALCULATE
#ifdef OLD_CALCULATE
    return QRect(virtualX / pointRatio.x(), virtualY / pointRatio.y(),
        virtualWidth / pointRatio.x(), virtualHeight / pointRatio.y());
#else
    return QRect(getNumforT_1(virtualX / pointRatio.x(), 4), getNumforT_1(virtualY / pointRatio.y(), 4),
        getNumforT_1(virtualWidth / pointRatio.x(), 4), getNumforT_1(virtualHeight / pointRatio.y(), 4));
#endif
}
/*
Date: 2019-10-28
Author: John_Yang
*/
QRectF ScreenLayoutItem::Reality2VirtualRectF(QRectF realityRect, QPointF pointRatio)
{
    qreal realityWidth = realityRect.width();
    qreal realityHeight = realityRect.height();
    qreal realityX = realityRect.left();
    qreal realityY = realityRect.top();

    return QRectF(realityX*pointRatio.x(), realityY*pointRatio.y(), realityWidth*pointRatio.x(), realityHeight*pointRatio.y());
}

QRectF ScreenLayoutItem::Virtual2RealityRectF(QRectF virtualRect, QPointF pointRatio)
{
    qreal virtualWidth = virtualRect.width();
    qreal virtualHeight = virtualRect.height();
    qreal virtualX = virtualRect.left();
    qreal virtualY = virtualRect.top();

    return QRectF(virtualX / pointRatio.x(), virtualY / pointRatio.y(),
        virtualWidth / pointRatio.x(), virtualHeight / pointRatio.y());
}

void ScreenLayoutItem::setRealityRectF(QRectF rectf)
{
    if (_realityRectF == rectf)
    {
        return;
    }
    _realityRectF = rectf;
    _currentRectF = Reality2VirtualRectF(_realityRectF, _pointRatio);
    this->setGeometry(_currentRectF.toRect());
}

void ScreenLayoutItem::setVirtualRectF(QRectF rectf)
{
    if (_currentRectF == rectf)
    {
        return;
    }
    _currentRectF = rectf;
    this->setGeometry(_currentRectF.toRect());
    _realityRectF = Virtual2RealityRectF(rectf, _pointRatio);
}

void ScreenLayoutItem::initOriginalRectF(QRectF rectf)
{
    _originalRectF = rectf;
}
