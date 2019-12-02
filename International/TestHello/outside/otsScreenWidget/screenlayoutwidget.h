#ifndef SCREENLAYOUTWIDGET_H
#define SCREENLAYOUTWIDGET_H

#include <QWidget>
#include <QString>
#include <QMap>
#include <QVector>
#include <QRect>

class ScreenLayoutItem;
class ScreenLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenLayoutWidget(QWidget *parent = 0);

    void setRatio(QPointF pointRatio);
    bool addScreenLayoutItem(QRect rect, int encoderID = 0);
    bool addScreenLayoutItem(QRectF rect, int encoderID = 0);
    void cleanScreenLayoutItems();
	QVector<ScreenLayoutItem*> getVecScreenLayoutItems() { return _vecScreenLayoutItems; }
    /** 移除大屏中的Item 从数组删除 */
    bool remvoeItemFromVector(ScreenLayoutItem* item);

    bool existItem(ScreenLayoutItem* item);

signals:
    void signalItemRectChanged(QObject* pItem, QRect newRect);
    void signalItemRectFChanged(QObject* pItem, QRectF newRect);
    void signalLayoutChanged();

public slots:
    void slotRemoveScreenLayoutItem();

private slots:
    void slotItemRectChanged(QRect rect);
    void slotItemRectFChanged(QRectF rectf);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QVector<ScreenLayoutItem*> _vecScreenLayoutItems;/**< 保存所有Item */
    QPointF _pointRatio;/**< 记录虚拟与实际换算比率 */
};

class ScreenLayoutItem : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenLayoutItem(QWidget *parent = 0);

    void setItemName(QString strName){ _strItemName = strName; }
    QString getItemName(){ return _strItemName; }
    void setEncoderID(int encoderID){ _encoderID = encoderID; }
    int getEncoderID(){ return _encoderID; }
    //void setMapDeocerRect(QMap<int, QVector<QRect>> mapDecoderRect){ _mapDecoderRect = mapDecoderRect; }
    //QMap<int, QVector<QRect>> getMapDecoderRect(){ return _mapDecoderRect; }
    void setRealityRect(QRect rect);
    /** 获取排布的真实矩形 */
    QRect getRealityRect(){ return _realityRect; }
    void setVirtualRect(QRect rect);
    void setRatio(QPointF pointfRatio);
    /** 初始化原始矩形 */
    void initOriginalRect(QRect rect);
    /** 主动发送贴边信号 */
    void sendSignal();

    /* 实际矩形转换为虚拟矩形-- 实际大屏对应虚拟矩形，电子沙盘显示屏对应实际矩形 */
    static QRect Reality2VirtualRect(QRect realityRect, QPointF pointRatio);
    /* 虚拟矩形转换为实际矩形 */
    static QRect Virtual2RealityRect(QRect virtualRect, QPointF pointRatio);

    /*
    Date: 2019-10-28
    Author: John_Yang
    Note: 添加的新接口去处理4个像素黑边
    */
public:
    /*
    Data: 2019-10-28
    Author: John_Yang
    Out: QRectF 当前对象的矩形
    Note: 返回当前窗口的矩形
    */
    QRectF getGeometry(){ return _currentRectF; }
    /* 实际矩形转换为虚拟矩形-- 实际大屏对应虚拟矩形，电子沙盘显示屏对应实际矩形 */
    static QRectF Reality2VirtualRectF(QRectF realityRect, QPointF pointRatio);
    /* 虚拟矩形转换为实际矩形 */
    static QRectF Virtual2RealityRectF(QRectF virtualRect, QPointF pointRatio);
    /* 设置实际矩形 */
    void setRealityRectF(QRectF rectf);
    /** 获取排布的真实矩形 */
    QRectF getRealityRectF(){ return _realityRectF; }
    /* 设置界面上的矩形 */
    void setVirtualRectF(QRectF rectf);
    /** 初始化原始矩形 */
    void initOriginalRectF(QRectF rectf);

signals:
    void signalRemoveItem();
    void signalRectChanged(QRect newRect);
    void signalRectFChanged(QRectF newRectF);

private:
    enum Mouse_Type
    {
        Mouse_Center = 0x00,
        Mouse_Left = 0x01,
        Mouse_Top = 0x02,
        Mouse_Right = 0x04,
        Mouse_Bottom = 0x08,
        Mouse_LeftTop = Mouse_Left|Mouse_Top,
        Mouse_RightTop = Mouse_Right|Mouse_Top,
        Mouse_LeftBottom = Mouse_Left|Mouse_Bottom,
        Mouse_RightBottom = Mouse_Right|Mouse_Bottom,
    };
    Q_DECLARE_FLAGS(Mouse_Types, Mouse_Type)

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    void leaveEvent(QEvent *event);
    /** 鼠标按键操作 */
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    /** 鼠标滚轮操作 */
    void wheelEvent(QWheelEvent *event);
    /** 手势操作 */
    bool event(QEvent *event);
    /** 定时器 */
    void timerEvent(QTimerEvent *event);

private:
    void calculationMouseType();
    /** 缩放计算 */
    QRectF calculateScaleRectF(const QRectF& oldRectf, const qreal scaleFactor);

private:
    int _id;/**< Item  ID */
    QString _strItemName;/**< Item名称 */
    int _encoderID;/**< 编码板ID */
    QString _strEncoderName;/**< 编码板名字 */
    QPoint _mousePress;/**< 鼠标按下位置绝对坐标(全局) */
    QPoint _pos;/**< 窗口坐标绝对坐标(全局) */
    QRect _oldRect;/**< 窗口在鼠标/手指按下时的矩形 */
    QRect _originalRect;/**< 双击之前的原始矩形 */
    int _space;/**< 鼠标拉伸间距 */
    Mouse_Types _mouseType;/**< 鼠标样式 */
    QRect _realityRect;/**< 真实矩形 */
    QPointF _pointRatio;/**< 记录虚拟与实际换算比率 */
    //QMap<int, QVector<QRect>> _mapDecoderRect;/**< 保存对应解码板位置以及信号源大小 */
    int _minSize{ 300 };
    bool _bIsTwoPoint;/** 手指是否是两个 */
    bool _touchIsRelease;/**< 手指是否释放 */
    /*
    Date: 2019-10-28
    Author: John_Yang
    Note: 为优化黑边做的数据保存
    */
    QRectF _oldRectF;/**< 窗口在鼠标/手指按下时的矩形 */
    QRectF _currentRectF;/**< 当前窗口的矩形 */
    QRectF _originalRectF;/**< 双击之前的原始矩形 */
    QRectF _realityRectF;/**< 真实矩形 */
    QPointF _posF;/**< 窗口坐标绝对坐标(全局) */
};

#endif // SCREENLAYOUTWIDGET_H
