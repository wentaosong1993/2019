#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QWidget>
#include <QMap>

/** line 数据结构 */
typedef struct _Line_ST
{
    int x0;
    int y0;
    int x1;
    int y1;
}Line_ST;

/** 绘制解码板线框类 */
class LinesJson
{
public:
    LinesJson();

    std::string createLineJson2Decoder();

    std::string strLineColor;/**< 线条颜色 */
    int iLineWidth;/**< 线宽 */
    int iLineType;/**< 线类型 */
    std::vector<Line_ST> vecLine;/**< 存储所有线条 */

private:
    std::string _strLineJson;/**< 最终返回的json */
};

class ScreenTopSplit;
class ScreenLayoutWidget;
class ScreenLayoutItem;
class ScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenWidget(QWidget *parent = 0, bool onlyPreview = true);

    enum Model_Type
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

    void createAllWidget();

    void setScreenRowCol(int row,int col);
    void setScreenModeRowCol(int row,int col);

	ScreenLayoutWidget* getScreenLayoutWidget() { return  _pScreenLayoutWidget; }

    /*! \fn 
    计算矩形所在解码板实际位置 计算发送解码板数据json使用
    inut:
    */
    void calculationRect2DecoderRect(const QRect& itemRect, QMap<int, QVector<QRect>>& mapDecoderRect);
    /*!
    返回大屏LayoutItems
    */
	QList<ScreenLayoutItem*> getScreenLayoutItems();

	void setScreenLayout();

    /*!
    返回大屏排布类型
    */
    int getScreenLayouType(){ return _layoutModelType; }
    /*!
    返回大屏ID
    */

	/*!
	设置大屏界面布局
	*/
	/*

	*/
	void setScreenLayoutWidget(int moduleType, std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> &vecLayoutInfo);
    void setScreenLayoutWidget_net(int moduleType, std::vector<std::tuple<std::string, int, int, int, int, int>> &vecLayoutInfo);

	/*
	<大屏切换时，仅设置界面显示，不投屏>
	*/
	void onlySetScreenLayoutWidget(int moduleType, std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> &vecLayoutInfo);

    int getScreenID(){ return _screenID; }

	int getScreenModuleID() { return _screenModelID; }
    /*!
    调用顶层显示窗口的计算方法，计算Item最大矩形
    */
    QRect calculateRectMax(const QRect& rect);
    QRectF calculateRectFMax(const QRectF& rectf);

    /** 用在先预览后应用的操作中 */
    void setScreenModel(int screenModelID);//2

    /** 更新背景图片 */
    void resetBgPixmap();
    /** 提供光计算大小的接口 */
    void calculateNewRect(QObject* pItem, QRect newRect);
    void calculateNewRectF(QObject* pItem, QRectF newRect);

signals:
    void signalCurrentScreenModelType(int type);

	/*!
	在鼠标Release事件和drop事件时做保存帧数据的操作
	*/
	void signalSaveFrameToDB();

public slots:
    void slotLayoutModelChanged(int modelType);//1
    void slotScreenChanged(int screenID);
    void slotScreenModelChanged(int screenModelID);//2
    void slotApplyScreen();
    void slotApplyScreenNoSave();
    void slotItemRectChanged(QObject* pItem, QRect newRect);
    void slotItemRectFChanged(QObject* pItem, QRectF newRect);
    void slotClearScreen();
	/*!
	设置镜像通道开启、关闭状态
	*/

	/*!
	在鼠标Release事件和drop事件时做保存帧数据的操作
	*/
	void slotSaveFrameToDB();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    /** 拖拽进入事件 */
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

    /* 大小事件 */
    void resizeEvent(QResizeEvent *event);

    /** 显示界面事件处理 */
    void showEvent(QShowEvent *event);

private:
    void calculationRatio();
    /** 实现贴边功能 */
    int checkRectPos(int nPos, int nUnitSize, int nMaxOffset);
    /** 大屏排布方式发生变化 */
    void setScreenLayoutType(int modelType);

private:
    struct Decoder_ST
    {
        Decoder_ST(){id=0;is4k=false;}
        int id;
        QString strName;
        bool is4k;
    };

private:
    int _screenID;/**< 大屏ID */
    int _screen_row;/**< 大屏行 */
    int _screen_col;/**< 大屏列 */
    int _screenResolution;/**< 大屏分辨率 */
    QMap<int,ScreenWidget::Decoder_ST> _mapDecoder;/**< 大屏对应解码板信息 */
    int _screenModelID;/**< 当前大屏预案ID */
    int _layout_row;/**< 排布行 */
    int _layout_col;/**< 排布列 */
    int _layoutModelType;/**< 排布类型 */
    QPointF _pointRatio;/**< 记录虚拟大屏与实际大屏换算比率 */
    ScreenTopSplit* _pScreenTopSplit;/**< 大屏顶层分割界面 */
    ScreenLayoutWidget* _pScreenLayoutWidget;/**< 大屏排布界面 */
    QRect _dragEnterRect;/**< 拖拽信号进入的矩形 */
	int _saveTemporaryFrameIndex = -1;  /* <保存当前帧索引>*/
    QPixmap _bgPixmap;/**< 背景图片 */
	int _mirrorId = 0; /**<镜像通道id>*/
    bool _isOnlyPreview;/**< 是否只进行预览 */
};
#endif // SCREENWIDGET_H