#include "screenwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QStackedLayout>
#include <QSizePolicy>
#include "screentopsplit.h"
#include "screenlayoutwidget.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include "otsServer/DataSession.h"
#include "ots/Logger.h"
#include <QMessageBox>
#include "otsControler/ControlerData.h"
#include <tuple>
#include <vector>
#include <Poco/Tuple.h>
#include "otsControler/net_globle_data.h"
#include "otsControler/mould_op.h"

#define _VARIADIC_MAX  10
#define OLD_CALCULATE_

int getNumforT(int num, int T)
{
    int result = num;
    int temp = num % T;
    if (temp != 0)
    {
#ifdef OLD_CALCULATE
        result -= temp;
#else
        qreal median = T / 2.0;
        if (temp >= median)
        {
            result += (T - temp);
        }
        else
        {
            result -= temp;
        }
#endif
    }
    return result;
}

ScreenWidget::ScreenWidget(QWidget *parent, bool onlyPreview) : QWidget(parent)
{
    _screenID = 0;
    _screenResolution = 18;
    _screen_row = 1;
    _screen_col = 1;
    _screenModelID = 0;
    _layout_row = 1;
    _layout_col = 1;
    _layoutModelType = Model_1;
    _mapDecoder.clear();
    _mapDecoder[0] = ScreenWidget::Decoder_ST();
    _pScreenTopSplit = NULL;
    _pScreenLayoutWidget = NULL;
    _isOnlyPreview = onlyPreview;

    _bgPixmap = QPixmap(1920, 1080);
    _bgPixmap.fill(Qt::transparent);

    createAllWidget();

    if (!_isOnlyPreview)
    {
		  /** 开启拖拽事件 */
        this->setAcceptDrops(true);
        this->setAttribute(Qt::WA_MouseTracking, true);
        //connect(this, SIGNAL(signalSaveFrameToDB()), this, SLOT(slotSaveFrameToDB()));
    }
}

void ScreenWidget::createAllWidget()
{
    QStackedLayout* pStackedLayout = new QStackedLayout;
    pStackedLayout->setStackingMode(QStackedLayout::StackAll);

    if(!_pScreenTopSplit)
    {
        _pScreenTopSplit = new ScreenTopSplit(this);
        pStackedLayout->addWidget(_pScreenTopSplit);
    }

    if(!_pScreenLayoutWidget)
    {
        _pScreenLayoutWidget =new ScreenLayoutWidget(this);
        if (!_isOnlyPreview)
        {
#ifdef OLD
            QObject::connect(_pScreenLayoutWidget, SIGNAL(signalItemRectChanged(QObject*, QRect)), this, SLOT(slotItemRectChanged(QObject*, QRect)));
            QObject::connect(_pScreenLayoutWidget, SIGNAL(signalLayoutChanged()), this, SLOT(slotApplyScreen()), Qt::QueuedConnection);
#else
            QObject::connect(_pScreenLayoutWidget, SIGNAL(signalItemRectFChanged(QObject*, QRectF)), this, SLOT(slotItemRectFChanged(QObject*, QRectF)));
            QObject::connect(_pScreenLayoutWidget, SIGNAL(signalLayoutChanged()), this, SLOT(slotApplyScreen()), Qt::QueuedConnection);
#endif
        }
        pStackedLayout->addWidget(_pScreenLayoutWidget);
    }

    this->setLayout(pStackedLayout);
}

void ScreenWidget::setScreenRowCol(int row, int col)
{
    if (row < 1 || col < 1)
    {
        return;
    }
    _screen_row = row;
    _screen_col = col;
}

void ScreenWidget::setScreenModeRowCol(int row, int col)
{
    if (row < 1 || col < 1)
    {
        return;
    }
    _layout_row = row;
    _layout_col = col;
    if(_pScreenTopSplit)
    {
        _pScreenTopSplit->setRowCol(row,col);
    }
}

void ScreenWidget::calculationRect2DecoderRect(const QRect& itemRect, QMap<int, QVector<QRect>>& mapDecoderRect)
{
    OTS_LOG->debug("start calculation...");

    if ((_screen_col*_screen_row) != _mapDecoder.size())
    {
        return;
    }

    /* 计算解码板所在矩形 */
    QRect itemRealityRect= ScreenLayoutItem::Virtual2RealityRect(itemRect, _pointRatio);/** 编码板在实际大屏上的矩形 */
    qreal ratioWidth = 1920.0 / itemRealityRect.width();/** 编码板实/虚比例 */
    qreal ratioHeight = 1080.0 / itemRealityRect.height();/** 编码板实/虚比例 */
    for (int i = 0; i < _screen_row; i++)
    {
        for (int j = 0; j < _screen_col; j++)
        {
            QRect decoderRect(1920*j, 1080*i, 1920, 1080);/** 对应位置的解码板在大屏上的实际矩形 */
			//有交汇的像素点的话
            if (itemRealityRect.intersects(decoderRect))
            {
                /** 解码板矩形区域计算出来了 */
                QRect encoderINDecoderRect = itemRealityRect.intersected(decoderRect);/** 编码板与解码板相交的矩形 */
                QRect encoderSRC = itemRealityRect.intersected(decoderRect);/** 编码板与解码板相交的矩形 */
                encoderINDecoderRect.translate(QPoint(-1920 * j, -1080 * i));/** 将大屏的坐标原点移动到解码板的左上角，使相交的矩形映射到解码板区域 */
                encoderSRC.translate(itemRealityRect.topLeft()*-1);/** 将大屏的坐标原点移动到编码板码板的左上角，使相交的矩形映射到编码板区域 */
                encoderSRC.setWidth(encoderSRC.width() *ratioWidth);/** 计算实际的编码板信号宽度 */
                encoderSRC.setHeight(encoderSRC.height() *ratioHeight);/** 计算实际的编码板信号高度 */
                QVector<QRect> vecRect;
                vecRect.push_back(encoderINDecoderRect);
                vecRect.push_back(encoderSRC);
                mapDecoderRect[_mapDecoder[i*_screen_col + j].id] = vecRect;
            }
        }
    }
    OTS_LOG->debug("end calculation...");
}

QList<ScreenLayoutItem*> ScreenWidget::getScreenLayoutItems()
{
	return _pScreenLayoutWidget->findChildren<ScreenLayoutItem*>();
}

/*重新设置大屏布局信息*/
void ScreenWidget::setScreenLayoutWidget(int moduleType, std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> &vecLayoutInfo)
{
	onlySetScreenLayoutWidget(moduleType, vecLayoutInfo);
    slotApplyScreenNoSave();
}

void ScreenWidget::setScreenLayoutWidget_net(int moduleType, std::vector<std::tuple<std::string, int, int, int, int, int>> &vecLayoutInfo)
{
    setScreenLayoutType(moduleType);//1
    qInfo() << "_pointRatio:" << _pointRatio;
    emit signalCurrentScreenModelType(moduleType);

    for (int i = 0; i < vecLayoutInfo.size(); i++)
    {
        //坐标变换
        int left = std::get<1>(vecLayoutInfo[i]);
        int top = std::get<2>(vecLayoutInfo[i]);
        int right = std::get<3>(vecLayoutInfo[i]);
        int bottom = std::get<4>(vecLayoutInfo[i]);
        int encoderId = std::get<5>(vecLayoutInfo[i]);
        QRectF rect = ScreenLayoutItem::Reality2VirtualRectF(QRectF(left, top, right - left, bottom - top), _pointRatio);
        //给大屏添加对应的布局信息
        _pScreenLayoutWidget->addScreenLayoutItem(rect, encoderId);
    }
}

void ScreenWidget::onlySetScreenLayoutWidget(int moduleType, std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> &vecLayoutInfo)
{
	setScreenLayoutType(moduleType);//1
	emit signalCurrentScreenModelType(moduleType);
	qInfo() << "_pointRatio:" << _pointRatio;
	for (int i = 0; i < vecLayoutInfo.size(); i++)
	{
		//坐标变换
		int left = std::get<2>(vecLayoutInfo[i]);
		int top = std::get<3>(vecLayoutInfo[i]);
		int right = std::get<4>(vecLayoutInfo[i]);
		int bottom = std::get<5>(vecLayoutInfo[i]);
		int encoderId = std::get<8>(vecLayoutInfo[i]);
#ifdef OLD
        QRect rect = ScreenLayoutItem::Reality2VirtualRect(QRect(left, top, right - left, bottom - top), _pointRatio);
#else
        QRectF rect = ScreenLayoutItem::Reality2VirtualRectF(QRectF(left, top, right - left, bottom - top), _pointRatio);
#endif
		//给大屏添加对应的布局信息
		_pScreenLayoutWidget->addScreenLayoutItem(rect, encoderId);
	}
}

QRect ScreenWidget::calculateRectMax(const QRect& rect)
{
    return _pScreenTopSplit->calculateRectMax(rect);
}

QRectF ScreenWidget::calculateRectFMax(const QRectF& rectf)
{
    return _pScreenTopSplit->calculateRectFMax(rectf);
}

void ScreenWidget::setScreenModel(int screenModelID)
{
    _screenModelID = screenModelID;
    int type = 1;
    otsServer::DataSession::instance()->getMouldTypeByID(screenModelID, type);

    /*更新大屏顶层显示的界面布局*/
    setScreenLayoutType(type);
    //emit signalCurrentScreenModelType(type);
    std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;

    otsServer::DataSession::instance()->getLayoutInfo(screenModelID, vecTupleLayout);
    _pScreenLayoutWidget->cleanScreenLayoutItems();
    calculationRatio();
    for (int i = 0; i < vecTupleLayout.size(); i++)
    {
#ifdef OLD
        //坐标变换
        QRect rect = ScreenLayoutItem::Reality2VirtualRect(std::get<3>(vecTupleLayout[i]), _pointRatio);
#else
        //坐标变换
        QRectF rect = ScreenLayoutItem::Reality2VirtualRectF(std::get<3>(vecTupleLayout[i]), _pointRatio);
#endif
        //给大屏添加对应的布局信息
        _pScreenLayoutWidget->addScreenLayoutItem(rect, std::get<0>(vecTupleLayout[i]));
    }
    OTS_LOG->debug("end create layoutItem.");
}

void ScreenWidget::resetBgPixmap()
{
    QString screenBgName = "./resource/" + QString::number(_screenID) + ".png";
    if (QFile::exists(screenBgName))
    {
        _bgPixmap = QPixmap(screenBgName);
    }
    else
    {
        _bgPixmap = QPixmap(1920, 1080);
        _bgPixmap.fill(Qt::transparent);
    }
    update();
}

void ScreenWidget::calculateNewRect(QObject* pItem, QRect newRect)
{
        //OTS_LOG->debug("start time...");
        ScreenLayoutItem* pSendItem = qobject_cast<ScreenLayoutItem*>(pItem);
        if (!pSendItem)
        {
            return;
        }
        QRect videoRect = pSendItem->getRealityRect();

        /* 判断是否贴合边界，通过视频布局判断 */
        int n2PixConvert = (int)(2.0 / _pointRatio.x() + 0.9);
        const int min_MAGNETIC_SPACE = 64;
        n2PixConvert = n2PixConvert < min_MAGNETIC_SPACE ? min_MAGNETIC_SPACE : n2PixConvert;

        videoRect.setLeft(checkRectPos(videoRect.left(), 1920 - 1, n2PixConvert));
        videoRect.setTop(checkRectPos(videoRect.top(), 1080 - 1, n2PixConvert));
        videoRect.setRight(checkRectPos(videoRect.right(), 1920 - 1, n2PixConvert));
        videoRect.setBottom(checkRectPos(videoRect.bottom(), 1080 - 1, n2PixConvert));

        if (videoRect.right() % 1920 == 0)
        {
            videoRect.setRight(videoRect.right() - 1);
        }

        if (videoRect.bottom() % 1080 == 0)
        {
            videoRect.setBottom(videoRect.bottom() - 1);
        }

        /* 设置最小大小 */
        if (videoRect.width() < 200)
        {
            videoRect.setRight(videoRect.left() + 199);
        }
        if (videoRect.height() < 200)
        {
            videoRect.setBottom(videoRect.top() + 199);
        }

        /* 判断是否越界 */
        if (videoRect.right() >= _screen_col * 1920)
        {
            videoRect.setRight(_screen_col * 1920 - 1);
        }
        if (videoRect.bottom() >= _screen_row * 1080)
        {
            videoRect.setBottom(_screen_row * 1080 - 1);
        }

        /* 判断与其它边是否贴合 */
        QList<ScreenLayoutItem*> listItems = getScreenLayoutItems();
        for (auto iter = listItems.begin(); iter != listItems.end(); ++iter)
        {
            if (*iter == pSendItem)
            {
                continue;
            }
            /* 判断水平方向 */
            QRect vRect = (*iter)->getRealityRect();
            if ((vRect.top() <= videoRect.top() && videoRect.top() <= vRect.bottom())				/* rt1 的上边界在 rt2之间 */
                || (vRect.top() <= videoRect.bottom() && videoRect.bottom() <= vRect.bottom())	/* rt1 的下边界在 rt2之间 */
                || (videoRect.top() <= vRect.top() && vRect.top() <= videoRect.bottom())			/* rt2 的上边界在 rt1之间 */
                || (videoRect.top() <= vRect.bottom() && vRect.bottom() <= videoRect.bottom())	/* rt2 的下边界在 rt1之间 */
                )
            {
                /* 新位置的左边界 */
                if (std::abs(vRect.right() - videoRect.left()) <= n2PixConvert)
                    videoRect.setLeft(vRect.right() + 1);
                /* 右边界 */
                if (std::abs(vRect.left() - videoRect.right()) <= n2PixConvert)
                    videoRect.setRight(vRect.left() - 1);
            }
            /* 判断垂直方向 */
            if ((vRect.left() <= videoRect.left() && videoRect.left() <= vRect.right())		/* rt1 的左边界在 rt2之间 */
                || (vRect.left() <= videoRect.right() && videoRect.right() <= vRect.right())	/* rt1 的右边界在 rt2之间 */
                || (videoRect.left() <= vRect.left() && vRect.left() <= videoRect.right())	/* rt2 的左边界在 rt1之间 */
                || (videoRect.left() <= vRect.right() && vRect.right() <= videoRect.right())	/* rt2 的右边界在 rt1之间 */
                )
            {
                /* 新位置的上边界 */
                if (abs(videoRect.top() - vRect.bottom()) <= n2PixConvert)
                    videoRect.setTop(vRect.bottom() + 1);
                /* 下边界 */
                if (abs(videoRect.bottom() - vRect.top()) <= n2PixConvert)
                    videoRect.setBottom(vRect.top() - 1);
            }
        }

        /* 首先移动到新的位置 */
        pSendItem->setRealityRect(videoRect);
}

void ScreenWidget::calculateNewRectF(QObject* pItem, QRectF newRect)
{
    //OTS_LOG->debug("start time...");
    ScreenLayoutItem* pSendItem = qobject_cast<ScreenLayoutItem*>(pItem);
    if (!pSendItem)
    {
        return;
    }
    QRectF videoRect = pSendItem->getRealityRectF();

    /* 判断是否贴合边界，通过视频布局判断 */
    int n2PixConvert = (int)(2.0 / _pointRatio.x() + 0.9);
    const int min_MAGNETIC_SPACE = 64;
    n2PixConvert = n2PixConvert < min_MAGNETIC_SPACE ? min_MAGNETIC_SPACE : n2PixConvert;

    videoRect.setLeft(checkRectPos(videoRect.left(), 1920, n2PixConvert));
    videoRect.setTop(checkRectPos(videoRect.top(), 1080, n2PixConvert));
    videoRect.setRight(checkRectPos(videoRect.right(), 1920, n2PixConvert));
    videoRect.setBottom(checkRectPos(videoRect.bottom(), 1080, n2PixConvert));

    //if (videoRect.right() == 1920)
    //{
    //    videoRect.setRight(1919);
    //}

    //if (videoRect.bottom() == 1080)
    //{
    //    videoRect.setBottom(1079);
    //}

    /* 设置最小大小 */
    if (videoRect.width() < 200)
    {
        videoRect.setRight(videoRect.left() + 200);
    }
    if (videoRect.height() < 200)
    {
        videoRect.setBottom(videoRect.top() + 200);
    }

    /* 判断是否越界 */
    if (videoRect.right() >= _screen_col * 1920)
    {
        videoRect.setRight(_screen_col * 1920);
    }
    if (videoRect.bottom() >= _screen_row * 1080)
    {
        videoRect.setBottom(_screen_row * 1080);
    }

    /* 判断与其它边是否贴合 */
    QList<ScreenLayoutItem*> listItems = getScreenLayoutItems();
    for (auto iter = listItems.begin(); iter != listItems.end(); ++iter)
    {
        if (*iter == pSendItem)
        {
            continue;
        }
        /* 判断水平方向 */
        QRectF vRect = (*iter)->getRealityRectF();
        if ((vRect.top() <= videoRect.top() && videoRect.top() <= vRect.bottom())				/* rt1 的上边界在 rt2之间 */
            || (vRect.top() <= videoRect.bottom() && videoRect.bottom() <= vRect.bottom())	/* rt1 的下边界在 rt2之间 */
            || (videoRect.top() <= vRect.top() && vRect.top() <= videoRect.bottom())			/* rt2 的上边界在 rt1之间 */
            || (videoRect.top() <= vRect.bottom() && vRect.bottom() <= videoRect.bottom())	/* rt2 的下边界在 rt1之间 */
            )
        {
            /* 新位置的左边界 */
            if (std::abs(vRect.right() - videoRect.left()) <= n2PixConvert)
                videoRect.setLeft(vRect.right());
            /* 右边界 */
            if (std::abs(vRect.left() - videoRect.right()) <= n2PixConvert)
                videoRect.setRight(vRect.left());
        }
        /* 判断垂直方向 */
        if ((vRect.left() <= videoRect.left() && videoRect.left() <= vRect.right())		/* rt1 的左边界在 rt2之间 */
            || (vRect.left() <= videoRect.right() && videoRect.right() <= vRect.right())	/* rt1 的右边界在 rt2之间 */
            || (videoRect.left() <= vRect.left() && vRect.left() <= videoRect.right())	/* rt2 的左边界在 rt1之间 */
            || (videoRect.left() <= vRect.right() && vRect.right() <= videoRect.right())	/* rt2 的右边界在 rt1之间 */
            )
        {
            /* 新位置的上边界 */
            if (abs(videoRect.top() - vRect.bottom()) <= n2PixConvert)
                videoRect.setTop(vRect.bottom());
            /* 下边界 */
            if (abs(videoRect.bottom() - vRect.top()) <= n2PixConvert)
                videoRect.setBottom(vRect.top());
        }
    }

    /* 首先移动到新的位置 */
    pSendItem->setRealityRectF(videoRect);
}

//切换大屏界面排布，清除大屏界面布局
void ScreenWidget::slotLayoutModelChanged(int modelType)
{
    //_layoutModelType = modelType;
    //_pScreenTopSplit->setLayoutModelType(_layoutModelType);
    //_pScreenLayoutWidget->cleanScreenLayoutItems();
    setScreenLayoutType(modelType);
	slotApplyScreen();
}

//大屏改变时，获取大屏解码板信息，并保存
void ScreenWidget::slotScreenChanged(int screenID)
{
    _screenID = screenID;
    _saveTemporaryFrameIndex = -1;
    otsServer::DataSession::instance()->getIndexByScreenID(screenID, _saveTemporaryFrameIndex);

    std::tuple<int, int, int> rowColPro;/*<大屏属性：行，列，4K类型>*/
    std::vector<std::tuple<int, int, int> > vecDecoderID;/*<解码板所在行，列，解码板ID>*/
    _mapDecoder.clear();
    //获取当前大屏解码板信息 
    if (otsServer::DataSession::instance()->getScreenDecoderInfo(screenID, rowColPro, vecDecoderID))
    {
        _screen_row = std::get<0>(rowColPro);
        _screen_col = std::get<1>(rowColPro);
        _screenResolution = std::get<2>(rowColPro);
        for (int i = 0; i < vecDecoderID.size(); i++)
        {
            Decoder_ST decoderInfo;
            decoderInfo.id = std::get<2>(vecDecoderID[i]);
            decoderInfo.is4k = _screenResolution;
			/*<解码板在大屏上的坐标x,y乘积与解码板ID映射的map>*/
            _mapDecoder[std::get<0>(vecDecoderID[i])*_screen_col + std::get<1>(vecDecoderID[i])] = decoderInfo;
        }
        calculationRatio();
        _pScreenLayoutWidget->setRatio(_pointRatio);
    }

#if 1
	//获取大屏帧数据
	int backIndex = -1; //临时用，返回值无意义
	std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> newVecLayoutInfo;/*<界面排布基本信息vector>*/
	std::tuple<int, int, std::string, int, int, int > newMouldInfo;
	int retCode = otsServer::DataSession::instance()->getMouldFrameFromDB_ByFrameIndex(backIndex, _saveTemporaryFrameIndex, 0, screenID, newVecLayoutInfo, newMouldInfo);

	int mouldID = std::get<1>(newMouldInfo);
	std::string mouldName = std::get<2>(newMouldInfo);

	int mouldType = std::get<3>(newMouldInfo);
	if (_saveTemporaryFrameIndex != -1)
	{
		this->onlySetScreenLayoutWidget(mouldType, newVecLayoutInfo);
	}
	else
	{
		slotLayoutModelChanged(1);//1
		emit signalCurrentScreenModelType(1);
	}
#endif 

#if 0 
	//待优化的地方，先不动，直接在在上面写，获取最后一帧数据的方法
	//恢复当前大屏切换前保存的帧数据,
	int beforeIndex;
	std::vector<std::tuple<int, int, int, int, int, int, int, int, int, int>> vecLayoutInfo;
	std::tuple<int, int, std::string, int, int, int > mouldInfo;
	int retCode = otsServer::DataSession::instance()->getLastFrameFromDB(beforeIndex, 0, screenID, vecLayoutInfo, mouldInfo); //第一次
	if (1 == retCode)
	{
		qInfo() << "vecLayoutInfoSize:" << vecLayoutInfo.size();
		int mouldID = std::get<1>(mouldInfo);
		std::string mouldName = std::get<2>(mouldInfo);

		int mouldType = std::get<3>(mouldInfo);
		if (vecLayoutInfo.size() >= 1)
		{
			this->setScreenLayoutWidget(mouldType, vecLayoutInfo);
		}
        else
        {
            slotLayoutModelChanged(1);//1
            emit signalCurrentScreenModelType(1);
        }
	}
#endif 

    resetBgPixmap();
}

//预案切换时，更新大屏
void ScreenWidget::slotScreenModelChanged(int screenModelID)
{
    _screenModelID = screenModelID;
    int type = 1;
    otsServer::DataSession::instance()->getMouldTypeByID(screenModelID, type);

	/*更新大屏顶层显示的界面布局*/
    //slotLayoutModelChanged(type);
    setScreenLayoutType(type);
    emit signalCurrentScreenModelType(type);
    std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;

    otsServer::DataSession::instance()->getLayoutInfo(screenModelID, vecTupleLayout);
    _pScreenLayoutWidget->cleanScreenLayoutItems();
    for (int i = 0; i < vecTupleLayout.size(); i++)
    {
#ifdef OLD
        //坐标变换
        QRect rect = ScreenLayoutItem::Reality2VirtualRect(std::get<3>(vecTupleLayout[i]), _pointRatio);
#else
        //坐标变换
        QRectF rect = ScreenLayoutItem::Reality2VirtualRectF(std::get<3>(vecTupleLayout[i]), _pointRatio);
#endif
		//给大屏添加对应的布局信息
        _pScreenLayoutWidget->addScreenLayoutItem(rect, std::get<0>(vecTupleLayout[i]));
    }

    OTS_LOG->debug("end create layoutItem.");
    slotApplyScreen();
}

void ScreenWidget::slotApplyScreen()
{
#ifdef OLD
    if (_isOnlyPreview)
    {
        return;
    }
    if ((_screen_row * _screen_col) != _mapDecoder.size())
    {
        OTS_LOG->error("[{}] screen row:{}, {col:}, decoder size:{}", __FUNCTION__, _screen_row, _screen_col, _mapDecoder.size());
        return;
    }

    QList<ScreenLayoutItem*> listItem;
    listItem = this->getScreenLayoutItems();
    qInfo() << "size:" << listItem.size();
    /* 没有排布的时候是否可以投屏 */
    /*if (listItem.size() == 0)
    {
    QMessageBox::warning(this, tr("Invalid Model"), tr("The big screen is not arranged"));
    return;
    }*/
    OTS_LOG->debug("start apply .....................");
    std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;
    vecDecodeToIRect.clear();
    /*<保存本大屏帧数据>*/
    slotSaveFrameToDB();
    otsServer::DataSession::instance()->setIndexByScreenID(_screenID, _saveTemporaryFrameIndex);

    QVector<QRect> temp_vectItem;

    for (int i = 0; i < _screen_row; ++i)
    {
        for (int j = 0; j < _screen_col; ++j)
        {
            ots::PairDecodeToIRect pairInfo;
            int num = i * _screen_col + j;
            pairInfo.first = _mapDecoder[num].id; //页面布局index对应的解码器ID
            int layout = 0;/* 层级从0层开始 */

            std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int > > vecIRect;
            QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
            //Todo: //解码器上的Rect，编码器ID，信号源Rect，lay,码流类型（用实际矩形大小layRect判断）
            temp_vectItem.clear();
            for (int itemLayout = 0; itemLayout < listItem.size(); itemLayout++)
            {
                if (!_pScreenLayoutWidget->existItem(listItem[itemLayout]))
                {
                    listItem[itemLayout]->deleteLater();
                    continue;
                }
                QRect layRect = listItem[itemLayout]->getRealityRect(); //布局Rect

                QRect rectInter = layRect.intersected(devRect);//重叠区域

                if (rectInter.width() > 1 && rectInter.height() > 1)
                {
                    //计算解码器画面在拼接大屏上的位置信息
                    QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
                    ots::IRect destRt;
                    destRt.left = getNumforT(rtDest.left(), 4);
                    destRt.top = getNumforT(rtDest.top(), 4);
                    destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
                    destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

                    QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                    /*
                    如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
                    */
                    for (int temp = 0; temp < temp_vectItem.size(); temp++)
                    {
                        if (tempDesRect.intersected(temp_vectItem[temp]) != QRect())
                        {
                            layout++;
                            temp_vectItem.clear();
                            break;
                        }
                    }

                    //计算编码器画面在拼接大屏上的位置
                    QRect rtran = rectInter.translated(layRect.topLeft() * -1);
                    float fx = (float(layRect.width())) / 1920.0;
                    float fy = (float(layRect.height())) / 1080.0;
                    ots::IRect srcRt;
                    /* 是否需要对视频源进行4的倍数计算 */

#ifdef SRC_4
                    srcRt.left = getNumforT((long)(rtran.left() / fx), 4);
                    srcRt.top = getNumforT((long)(rtran.top() / fy), 4);
                    srcRt.right = getNumforT((long)((rtran.left() + rtran.width()) / fx), 4);
                    srcRt.bottom = getNumforT((long)((rtran.top() + rtran.height()) / fy), 4); #else
#else
                    srcRt.left = (long)(rtran.left() / fx);
                    srcRt.top = (long)(rtran.top() / fy);
                    srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                    srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);
#endif

                    vecIRect.push_back(std::make_tuple(destRt, listItem[itemLayout]->getEncoderID(), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

                    /*Todo:保存矩形大小*/
                    temp_vectItem.push_back(tempDesRect);
                }
            }
            pairInfo.second = vecIRect;
            vecDecodeToIRect.push_back(pairInfo);
        }
    }

    OTS_LOG->debug("111111 apply .....................");
    // 发送应用命令
    otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

    //若开启了镜像通道，则在切换预案时，发送投屏指令
    std::vector<std::tuple<int, std::string, int> > allMirror;
    int retCode = otsServer::DataSession::instance()->getMirrorByScreenID(_screenID, allMirror);
    for (int i = 0; i < allMirror.size(); i++)
    {
        if (std::get<2>(allMirror[i]) == 1)
        {
            /* 通过镜像通道ID拉取该方案下的大屏ID */
            QVector<QRect> temp_vectRectItem;
            std::vector<int> vecAllScreenID;
            otsServer::DataSession::instance()->getScreenByMirrorID(std::get<0>(allMirror[i]), vecAllScreenID);

            /* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
            std::tuple<int, int, int> rowColPro;
            std::vector<std::tuple<int, int, int> > vecDecoderID;

            qInfo() << "当前大屏ID：" << _screenID;
            bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(_screenID, rowColPro, vecDecoderID);
            if (!retCode)
            {
                qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                return;
            }

            int row = std::get<0>(rowColPro);
            int col = std::get<1>(rowColPro);
            int totalWidth = col * 1920;
            int totalHeight = row * 1080;
            //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
            std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;
            std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

            //从当前界面获取排布信息
            QList<ScreenLayoutItem*> listScreenLayout = this->getScreenLayoutItems();
            qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
            for (int i = 0; i < listScreenLayout.size(); i++)
            {
                if (!_pScreenLayoutWidget->existItem(listScreenLayout[i]))
                {
                    listScreenLayout[i]->deleteLater();
                    continue;
                }

                int left = listScreenLayout[i]->getRealityRect().left();
                int top = listScreenLayout[i]->getRealityRect().top();
                int right = listScreenLayout[i]->getRealityRect().right();
                int bottom = listScreenLayout[i]->getRealityRect().bottom();
                int sequence = i;
                int encoderId = listScreenLayout[i]->getEncoderID();
                qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
                std::tuple<int, std::string, int, QRect> layoutInfo;
                layoutInfo = std::make_tuple(encoderId, "", sequence, QRect(left, top, right - left, bottom - top));
                vecTupleLayout.push_back(layoutInfo);
            }

            for (int i = 0; i < vecTupleLayout.size(); i++)
            {
                //计算当前编码器信号在当前大屏上的屏幕占比
                int encoderId = std::get<0>(vecTupleLayout[i]);
                std::string layoutName = std::get<1>(vecTupleLayout[i]);
                int sequence = std::get<2>(vecTupleLayout[i]);
                QRect curRect = std::get<3>(vecTupleLayout[i]);
                QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

                vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
                qInfo() << std::get<3>(vecNewTupleLayout[i]);
            }

            /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
            std::vector<ots::PairDecodeToIRect> vecDecoderToIRect;

            //通过大屏ID获取大屏信息
            for (int k = 0; k < vecAllScreenID.size(); k++)
            {
                /*<自身大屏不重复投屏>*/
                if (vecAllScreenID[k] == _screenID)
                {
                    continue;
                }

                std::map<int, int> mapIndex2Decoder;
                mapIndex2Decoder.clear();

                std::tuple<int, int, int> rowCol;
                std::vector<std::tuple<int, int, int> > vecDecoderInfo;
                bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
                if (!retCode)
                {
                    qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                    return;
                }
                //大屏的行列，宽高信息
                int newRow = std::get<0>(rowCol);
                int newCol = std::get<1>(rowCol);
                int newTotalWidth = newCol * 1920;
                int newTotalHeight = newRow * 1080;

                /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
                for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
                {
                    int decoderId = std::get<2>(vecDecoderInfo[i]);
                    int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
                    mapIndex2Decoder[num] = decoderId;
                }

                //保存帧数据
                bool tempState = true;
                std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
                //<left, top, right, botto, seq, encoderid>

                for (int i = 0; i < newRow; ++i)
                {
                    for (int j = 0; j < newCol; ++j)
                    {
                        temp_vectRectItem.clear();
                        int lay = 0; //默认在第0层

                        ots::PairDecodeToIRect pairInfo;
                        int num = i * newCol + j;
                        pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

                        std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                        QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
                        for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                        {
                            QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                            layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                            QRect newLayRect = layRectF.toRect();

                            if (tempState)
                            {
                                std::tuple<int, int, int, int, int, int > layoutInfo;
                                layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                                vecLayoutInfo.push_back(layoutInfo);
                            }

                            QRect rectInter = newLayRect.intersected(devRect);//重叠区域

                            if (rectInter.width() > 1 && rectInter.height() > 1)
                            {
                                //计算解码器画面在拼接大屏上的位置信息
                                QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
                                ots::IRect destRt;
                                destRt.left = getNumforT(rtDest.left(), 4);
                                destRt.top = getNumforT(rtDest.top(), 4);
                                destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
                                destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

                                //2019年10月14日11:05:49
                                QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                                /*
                                如果listItem[itemLayout]的矩形跟temp_vectRectItem  每个item比较是否有交集，有则将layout++，temp_vectRectItem清除，如果没有交集，
                                */
                                for (int temp = 0; temp < temp_vectRectItem.size(); temp++)
                                {
                                    if (tempDesRect.intersected(temp_vectRectItem[temp]) != QRect())
                                    {
                                        lay++;
                                        temp_vectRectItem.clear();
                                        break;
                                    }
                                }

                                //计算编码器画面在拼接大屏上的位置
                                QRect rtran = rectInter.translated(newLayRect.topLeft() * -1);
                                float fx = (float(newLayRect.width())) / 1920.0;
                                float fy = (float(newLayRect.height())) / 1080.0;
                                ots::IRect srcRt;
                                srcRt.left = (long)(rtran.left() / fx);
                                srcRt.top = (long)(rtran.top() / fy);
                                srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                                srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                                temp_vectRectItem.push_back(tempDesRect);
                                vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, lay, 0));//目的矩形、编码器ID、源矩形
                            }
                        }
                        tempState = false;
                        pairInfo.second = vecIRect;
                        vecDecoderToIRect.push_back(pairInfo);
                    }
                }

                //镜像开启后,保存每块大屏的帧数据
                int tempIndex = -1;
                otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
                qInfo() << "index:" << tempIndex;
                int backIndexOfOtherScreen = -1;
                retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, _layoutModelType);/*<开启镜像，排布类型相同>*/
                retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
            }

            // 发送应用命令
            otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecoderToIRect);
        }
    }
#else
    if (_isOnlyPreview)
    {
        return;
    }
    if ((_screen_row * _screen_col) != _mapDecoder.size())
    {
        OTS_LOG->error("[{}] screen row:{}, {col:}, decoder size:{}", __FUNCTION__, _screen_row, _screen_col, _mapDecoder.size());
        return;
    }

    QList<ScreenLayoutItem*> listItem;
    listItem = this->getScreenLayoutItems();
    qInfo() << "size:" << listItem.size();
    /* 没有排布的时候是否可以投屏 */
    /*if (listItem.size() == 0)
    {
    QMessageBox::warning(this, tr("Invalid Model"), tr("The big screen is not arranged"));
    return;
    }*/
    OTS_LOG->debug("start apply .....................");
    std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;
    vecDecodeToIRect.clear();
    /*<保存本大屏帧数据>*/
    slotSaveFrameToDB();
    otsServer::DataSession::instance()->setIndexByScreenID(_screenID, _saveTemporaryFrameIndex);

    QVector<QRect> temp_vectItem;

    for (int i = 0; i < _screen_row; ++i)
    {
        for (int j = 0; j < _screen_col; ++j)
        {
            ots::PairDecodeToIRect pairInfo;
            int num = i * _screen_col + j;
            pairInfo.first = _mapDecoder[num].id; //页面布局index对应的解码器ID
            int layout = 0;/* 层级从0层开始 */

            std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int > > vecIRect;
            QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
            //Todo: //解码器上的Rect，编码器ID，信号源Rect，lay,码流类型（用实际矩形大小layRect判断）
            temp_vectItem.clear();
            for (int itemLayout = 0; itemLayout < listItem.size(); itemLayout++)
            {
                if (!_pScreenLayoutWidget->existItem(listItem[itemLayout]))
                {
                    listItem[itemLayout]->deleteLater();
                    continue;
                }
                QRectF layRect = listItem[itemLayout]->getRealityRectF(); //布局Rect

                QRectF rectInter = layRect.intersected(devRect);//重叠区域

                if (rectInter.width() > 1 && rectInter.height() > 1)
                {
                    //计算解码器画面在拼接大屏上的位置信息
                    QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                    qDebug() << "rtDest:" << rtDest;
                    ots::IRect destRt;
                    destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                    destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                    destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                    destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

                    QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                    /*
                    如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
                    */
                    for (int temp = 0; temp < temp_vectItem.size(); temp++)
                    {
                        if (tempDesRect.intersected(temp_vectItem[temp]) != QRect())
                        {
                            layout++;
                            temp_vectItem.clear();
                            break;
                        }
                    }

                    //计算编码器画面在拼接大屏上的位置
                    QRectF rtran = rectInter.translated(layRect.topLeft() * -1);
                    float fx = (float(layRect.width())) / 1920.0;
                    float fy = (float(layRect.height())) / 1080.0;
                    ots::IRect srcRt;
                    /* 是否需要对视频源进行4的倍数计算 */

    #ifdef SRC_4
                    srcRt.left = getNumforT((long)(rtran.left() / fx), 4);
                    srcRt.top = getNumforT((long)(rtran.top() / fy), 4);
                    srcRt.right = getNumforT((long)((rtran.left() + rtran.width()) / fx), 4);
                    srcRt.bottom = getNumforT((long)((rtran.top() + rtran.height()) / fy), 4); #else
    #else
                    srcRt.left = (long)(rtran.left() / fx);
                    srcRt.top = (long)(rtran.top() / fy);
                    srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                    srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);
    #endif

                    vecIRect.push_back(std::make_tuple(destRt, listItem[itemLayout]->getEncoderID(), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

                    /*Todo:保存矩形大小*/
                    temp_vectItem.push_back(tempDesRect);
                }
            }
            pairInfo.second = vecIRect;
            vecDecodeToIRect.push_back(pairInfo);
        }
    }

    OTS_LOG->debug("111111 apply .....................");
    // 发送应用命令
    otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

    //若开启了镜像通道，则在切换预案时，发送投屏指令
    std::vector<std::tuple<int, std::string, int> > allMirror;
    int retCode = otsServer::DataSession::instance()->getMirrorByScreenID(_screenID, allMirror);
    for (int i = 0; i < allMirror.size(); i++)
    {
        if (std::get<2>(allMirror[i]) == 1)
        {
            /* 通过镜像通道ID拉取该方案下的大屏ID */
            QVector<QRectF> temp_vectRectItem;
            std::vector<int> vecAllScreenID;
            otsServer::DataSession::instance()->getScreenByMirrorID(std::get<0>(allMirror[i]), vecAllScreenID);

            /* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
            std::tuple<int, int, int> rowColPro;
            std::vector<std::tuple<int, int, int> > vecDecoderID;

            qInfo() << "当前大屏ID：" << _screenID;
            bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(_screenID, rowColPro, vecDecoderID);
            if (!retCode)
            {
                qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                return;
            }

            int row = std::get<0>(rowColPro);
            int col = std::get<1>(rowColPro);
            int totalWidth = col * 1920;
            int totalHeight = row * 1080;
            //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
            std::vector<std::tuple<int, std::string, int, QRectF> > vecTupleLayout;
            std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

            //从当前界面获取排布信息
            QList<ScreenLayoutItem*> listScreenLayout = this->getScreenLayoutItems();
            qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
            for (int i = 0; i < listScreenLayout.size(); i++)
            {
                if (!_pScreenLayoutWidget->existItem(listScreenLayout[i]))
                {
                    listScreenLayout[i]->deleteLater();
                    continue;
                }

                qreal left = listScreenLayout[i]->getRealityRectF().left();
                qreal top = listScreenLayout[i]->getRealityRectF().top();
                qreal right = listScreenLayout[i]->getRealityRectF().right();
                qreal bottom = listScreenLayout[i]->getRealityRectF().bottom();
                int sequence = i;
                int encoderId = listScreenLayout[i]->getEncoderID();
                qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
                std::tuple<int, std::string, int, QRectF> layoutInfo;
                layoutInfo = std::make_tuple(encoderId, "", sequence, QRectF(left, top, right - left, bottom - top));
                vecTupleLayout.push_back(layoutInfo);
            }

            for (int i = 0; i < vecTupleLayout.size(); i++)
            {
                //计算当前编码器信号在当前大屏上的屏幕占比
                int encoderId = std::get<0>(vecTupleLayout[i]);
                std::string layoutName = std::get<1>(vecTupleLayout[i]);
                int sequence = std::get<2>(vecTupleLayout[i]);
                QRectF curRect = std::get<3>(vecTupleLayout[i]);
                QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

                vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
                qInfo() << std::get<3>(vecNewTupleLayout[i]);
            }

            /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
            std::vector<ots::PairDecodeToIRect> vecDecoderToIRect;

            //通过大屏ID获取大屏信息
            for (int k = 0; k < vecAllScreenID.size(); k++)
            {
                /*<自身大屏不重复投屏>*/
                if (vecAllScreenID[k] == _screenID)
                {
                    continue;
                }

                std::map<int, int> mapIndex2Decoder;
                mapIndex2Decoder.clear();

                std::tuple<int, int, int> rowCol;
                std::vector<std::tuple<int, int, int> > vecDecoderInfo;
                bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
                if (!retCode)
                {
                    qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                    return;
                }
                //大屏的行列，宽高信息
                int newRow = std::get<0>(rowCol);
                int newCol = std::get<1>(rowCol);
                int newTotalWidth = newCol * 1920;
                int newTotalHeight = newRow * 1080;

                /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
                for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
                {
                    int decoderId = std::get<2>(vecDecoderInfo[i]);
                    int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
                    mapIndex2Decoder[num] = decoderId;
                }

                //保存帧数据
                bool tempState = true;
                std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
                //<left, top, right, botto, seq, encoderid>

                for (int i = 0; i < newRow; ++i)
                {
                    for (int j = 0; j < newCol; ++j)
                    {
                        temp_vectRectItem.clear();
                        int lay = 0; //默认在第0层

                        ots::PairDecodeToIRect pairInfo;
                        int num = i * newCol + j;
                        pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

                        std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                        QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
                        for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                        {
                            QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                            layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                            QRectF newLayRect = layRectF;

                            if (tempState)
                            {
                                std::tuple<int, int, int, int, int, int > layoutInfo;
                                layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                                vecLayoutInfo.push_back(layoutInfo);
                            }

                            QRectF rectInter = newLayRect.intersected(devRect);//重叠区域

                            if (rectInter.width() > 1 && rectInter.height() > 1)
                            {
                                //计算解码器画面在拼接大屏上的位置信息
                                QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                                ots::IRect destRt;
                                destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                                destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                                destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                                destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

                                //2019年10月14日11:05:49
                                QRectF tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                                /*
                                如果listItem[itemLayout]的矩形跟temp_vectRectItem  每个item比较是否有交集，有则将layout++，temp_vectRectItem清除，如果没有交集，
                                */
                                for (int temp = 0; temp < temp_vectRectItem.size(); temp++)
                                {
                                    if (tempDesRect.intersected(temp_vectRectItem[temp]) != QRect())
                                    {
                                        lay++;
                                        temp_vectRectItem.clear();
                                        break;
                                    }
                                }

                                //计算编码器画面在拼接大屏上的位置
                                QRectF rtran = rectInter.translated(newLayRect.topLeft() * -1);
                                float fx = (float(newLayRect.width())) / 1920.0;
                                float fy = (float(newLayRect.height())) / 1080.0;
                                ots::IRect srcRt;
                                srcRt.left = (long)(rtran.left() / fx);
                                srcRt.top = (long)(rtran.top() / fy);
                                srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                                srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                                temp_vectRectItem.push_back(tempDesRect);
                                vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, lay, 0));//目的矩形、编码器ID、源矩形
                            }
                        }
                        tempState = false;
                        pairInfo.second = vecIRect;
                        vecDecoderToIRect.push_back(pairInfo);
                    }
                }

                //镜像开启后,保存每块大屏的帧数据
                int tempIndex = -1;
                otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
                qInfo() << "index:" << tempIndex;
                int backIndexOfOtherScreen = -1;
                retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, _layoutModelType);/*<开启镜像，排布类型相同>*/
                retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
            }

            // 发送应用命令
            otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecoderToIRect);
        }
    }
#endif

#if 0 
	//保存当前大屏界面帧数据
	//时间： 2019-10-16 15：44 解决插销重做异常问题！！！！ by songwentao
	//保存当前镜像通道中所有大屏界面帧数据
	std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
	QVector<ScreenLayoutItem*> vecScreenLayout = _pScreenLayoutWidget->getVecScreenLayoutItems();
	qInfo() << "vecScreenLayoutSize:" << vecScreenLayout.size();
	for (int i = 0; i < vecScreenLayout.size(); i++)
	{
		int left = vecScreenLayout[i]->getRealityRect().left();
		int top = vecScreenLayout[i]->getRealityRect().top();
		int right = vecScreenLayout[i]->getRealityRect().right();
		int bottom = vecScreenLayout[i]->getRealityRect().bottom();
		int sequence = i;
		int encoderId = vecScreenLayout[i]->getEncoderID();
		qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
		std::tuple<int, int, int, int, int, int> layoutInfo;
		layoutInfo = std::make_tuple(left, top, right, bottom, sequence, encoderId);
		vecLayoutInfo.push_back(layoutInfo);
	}


	int tempIndex = 0;
	otsServer::DataSession::instance()->saveMouldFrame2DB(tempIndex, -1, 0, _screenID, vecLayoutInfo, _layoutModelType);
#endif 

    OTS_LOG->debug("end apply .....................");
}

void ScreenWidget::slotApplyScreenNoSave()
{
#ifdef OLD
    if (_isOnlyPreview)
    {
        return;
    }
    if ((_screen_row * _screen_col) != _mapDecoder.size())
    {
        OTS_LOG->error("[{}] screen row:{}, {col:}, decoder size:{}", __FUNCTION__, _screen_row, _screen_col, _mapDecoder.size());
        return;
    }

    QList<ScreenLayoutItem*> listItem;
    listItem = this->getScreenLayoutItems();
    qInfo() << "size:" << listItem.size();
    /* 没有排布的时候是否可以投屏 */
    /*if (listItem.size() == 0)
    {
    QMessageBox::warning(this, tr("Invalid Model"), tr("The big screen is not arranged"));
    return;
    }*/
    OTS_LOG->debug("start apply .....................");
    std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;
    vecDecodeToIRect.clear();
    /*<保存本大屏帧数据>*/
    slotSaveFrameToDB();
    otsServer::DataSession::instance()->setIndexByScreenID(_screenID, _saveTemporaryFrameIndex);

    QVector<QRect> temp_vectItem;

    for (int i = 0; i < _screen_row; ++i)
    {
        for (int j = 0; j < _screen_col; ++j)
        {
            ots::PairDecodeToIRect pairInfo;
            int num = i * _screen_col + j;
            pairInfo.first = _mapDecoder[num].id; //页面布局index对应的解码器ID
            int layout = 0;/* 层级从0层开始 */

            std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int > > vecIRect;
            QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
            //Todo: //解码器上的Rect，编码器ID，信号源Rect，lay,码流类型（用实际矩形大小layRect判断）
            temp_vectItem.clear();
            for (int itemLayout = 0; itemLayout < listItem.size(); itemLayout++)
            {
                if (!_pScreenLayoutWidget->existItem(listItem[itemLayout]))
                {
                    listItem[itemLayout]->deleteLater();
                    continue;
                }
                QRect layRect = listItem[itemLayout]->getRealityRect(); //布局Rect

                QRect rectInter = layRect.intersected(devRect);//重叠区域

                if (rectInter.width() > 1 && rectInter.height() > 1)
                {
                    //计算解码器画面在拼接大屏上的位置信息
                    QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
                    ots::IRect destRt;
                    destRt.left = getNumforT(rtDest.left(), 4);
                    destRt.top = getNumforT(rtDest.top(), 4);
                    destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
                    destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

                    QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                    /*
                    如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
                    */
                    for (int temp = 0; temp < temp_vectItem.size(); temp++)
                    {
                        if (tempDesRect.intersected(temp_vectItem[temp]) != QRect())
                        {
                            layout++;
                            temp_vectItem.clear();
                            break;
                        }
                    }

                    //计算编码器画面在拼接大屏上的位置
                    QRect rtran = rectInter.translated(layRect.topLeft() * -1);
                    float fx = (float(layRect.width())) / 1920.0;
                    float fy = (float(layRect.height())) / 1080.0;
                    ots::IRect srcRt;
                    /* 是否需要对视频源进行4的倍数计算 */

#ifdef SRC_4
                    srcRt.left = getNumforT((long)(rtran.left() / fx), 4);
                    srcRt.top = getNumforT((long)(rtran.top() / fy), 4);
                    srcRt.right = getNumforT((long)((rtran.left() + rtran.width()) / fx), 4);
                    srcRt.bottom = getNumforT((long)((rtran.top() + rtran.height()) / fy), 4); #else
#else
                    srcRt.left = (long)(rtran.left() / fx);
                    srcRt.top = (long)(rtran.top() / fy);
                    srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                    srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);
#endif

                    vecIRect.push_back(std::make_tuple(destRt, listItem[itemLayout]->getEncoderID(), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

                    /*Todo:保存矩形大小*/
                    temp_vectItem.push_back(tempDesRect);
                }
            }
            pairInfo.second = vecIRect;
            vecDecodeToIRect.push_back(pairInfo);
        }
    }

    OTS_LOG->debug("111111 apply .....................");
    // 发送应用命令
    otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

    //若开启了镜像通道，则在切换预案时，发送投屏指令
    std::vector<std::tuple<int, std::string, int> > allMirror;
    int retCode = otsServer::DataSession::instance()->getMirrorByScreenID(_screenID, allMirror);
    for (int i = 0; i < allMirror.size(); i++)
    {
        if (std::get<2>(allMirror[i]) == 1)
        {
            /* 通过镜像通道ID拉取该方案下的大屏ID */
            QVector<QRect> temp_vectRectItem;
            std::vector<int> vecAllScreenID;
            otsServer::DataSession::instance()->getScreenByMirrorID(std::get<0>(allMirror[i]), vecAllScreenID);

            /* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
            std::tuple<int, int, int> rowColPro;
            std::vector<std::tuple<int, int, int> > vecDecoderID;

            qInfo() << "当前大屏ID：" << _screenID;
            bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(_screenID, rowColPro, vecDecoderID);
            if (!retCode)
            {
                qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                return;
            }

            int row = std::get<0>(rowColPro);
            int col = std::get<1>(rowColPro);
            int totalWidth = col * 1920;
            int totalHeight = row * 1080;
            //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
            std::vector<std::tuple<int, std::string, int, QRect> > vecTupleLayout;
            std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

            //从当前界面获取排布信息
            QList<ScreenLayoutItem*> listScreenLayout = this->getScreenLayoutItems();
            qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
            for (int i = 0; i < listScreenLayout.size(); i++)
            {
                if (!_pScreenLayoutWidget->existItem(listScreenLayout[i]))
                {
                    listScreenLayout[i]->deleteLater();
                    continue;
                }

                int left = listScreenLayout[i]->getRealityRect().left();
                int top = listScreenLayout[i]->getRealityRect().top();
                int right = listScreenLayout[i]->getRealityRect().right();
                int bottom = listScreenLayout[i]->getRealityRect().bottom();
                int sequence = i;
                int encoderId = listScreenLayout[i]->getEncoderID();
                qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
                std::tuple<int, std::string, int, QRect> layoutInfo;
                layoutInfo = std::make_tuple(encoderId, "", sequence, QRect(left, top, right - left, bottom - top));
                vecTupleLayout.push_back(layoutInfo);
            }

            for (int i = 0; i < vecTupleLayout.size(); i++)
            {
                //计算当前编码器信号在当前大屏上的屏幕占比
                int encoderId = std::get<0>(vecTupleLayout[i]);
                std::string layoutName = std::get<1>(vecTupleLayout[i]);
                int sequence = std::get<2>(vecTupleLayout[i]);
                QRect curRect = std::get<3>(vecTupleLayout[i]);
                QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

                vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
                qInfo() << std::get<3>(vecNewTupleLayout[i]);
            }

            /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
            std::vector<ots::PairDecodeToIRect> vecDecoderToIRect;

            //通过大屏ID获取大屏信息
            for (int k = 0; k < vecAllScreenID.size(); k++)
            {
                /*<自身大屏不重复投屏>*/
                if (vecAllScreenID[k] == _screenID)
                {
                    continue;
                }

                std::map<int, int> mapIndex2Decoder;
                mapIndex2Decoder.clear();

                std::tuple<int, int, int> rowCol;
                std::vector<std::tuple<int, int, int> > vecDecoderInfo;
                bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
                if (!retCode)
                {
                    qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                    return;
                }
                //大屏的行列，宽高信息
                int newRow = std::get<0>(rowCol);
                int newCol = std::get<1>(rowCol);
                int newTotalWidth = newCol * 1920;
                int newTotalHeight = newRow * 1080;

                /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
                for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
                {
                    int decoderId = std::get<2>(vecDecoderInfo[i]);
                    int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
                    mapIndex2Decoder[num] = decoderId;
                }

                //保存帧数据
                bool tempState = true;
                std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
                //<left, top, right, botto, seq, encoderid>

                for (int i = 0; i < newRow; ++i)
                {
                    for (int j = 0; j < newCol; ++j)
                    {
                        temp_vectRectItem.clear();
                        int lay = 0; //默认在第0层

                        ots::PairDecodeToIRect pairInfo;
                        int num = i * newCol + j;
                        pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

                        std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                        QRect devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
                        for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                        {
                            QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                            layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                            QRect newLayRect = layRectF.toRect();

                            if (tempState)
                            {
                                std::tuple<int, int, int, int, int, int > layoutInfo;
                                layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                                vecLayoutInfo.push_back(layoutInfo);
                            }

                            QRect rectInter = newLayRect.intersected(devRect);//重叠区域

                            if (rectInter.width() > 1 && rectInter.height() > 1)
                            {
                                //计算解码器画面在拼接大屏上的位置信息
                                QRect rtDest = rectInter.translated(devRect.topLeft() * -1);
                                ots::IRect destRt;
                                destRt.left = getNumforT(rtDest.left(), 4);
                                destRt.top = getNumforT(rtDest.top(), 4);
                                destRt.right = getNumforT(rtDest.left() + rtDest.width(), 4);
                                destRt.bottom = getNumforT(rtDest.top() + rtDest.height(), 4);

                                //2019年10月14日11:05:49
                                QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                                /*
                                如果listItem[itemLayout]的矩形跟temp_vectRectItem  每个item比较是否有交集，有则将layout++，temp_vectRectItem清除，如果没有交集，
                                */
                                for (int temp = 0; temp < temp_vectRectItem.size(); temp++)
                                {
                                    if (tempDesRect.intersected(temp_vectRectItem[temp]) != QRect())
                                    {
                                        lay++;
                                        temp_vectRectItem.clear();
                                        break;
                                    }
                                }

                                //计算编码器画面在拼接大屏上的位置
                                QRect rtran = rectInter.translated(newLayRect.topLeft() * -1);
                                float fx = (float(newLayRect.width())) / 1920.0;
                                float fy = (float(newLayRect.height())) / 1080.0;
                                ots::IRect srcRt;
                                srcRt.left = (long)(rtran.left() / fx);
                                srcRt.top = (long)(rtran.top() / fy);
                                srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                                srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                                temp_vectRectItem.push_back(tempDesRect);
                                vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, lay, 0));//目的矩形、编码器ID、源矩形
                            }
                        }
                        tempState = false;
                        pairInfo.second = vecIRect;
                        vecDecoderToIRect.push_back(pairInfo);
                    }
                }

                //镜像开启后,保存每块大屏的帧数据
                int tempIndex = -1;
                otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
                qInfo() << "index:" << tempIndex;
                int backIndexOfOtherScreen = -1;
                retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, _layoutModelType);/*<开启镜像，排布类型相同>*/
                retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
            }

            // 发送应用命令
            otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecoderToIRect);
        }
    }
#else
    if (_isOnlyPreview)
    {
        return;
    }
    if ((_screen_row * _screen_col) != _mapDecoder.size())
    {
        OTS_LOG->error("[{}] screen row:{}, {col:}, decoder size:{}", __FUNCTION__, _screen_row, _screen_col, _mapDecoder.size());
        return;
    }

    QList<ScreenLayoutItem*> listItem;
    listItem = this->getScreenLayoutItems();
    qInfo() << "size:" << listItem.size();
    /* 没有排布的时候是否可以投屏 */
    /*if (listItem.size() == 0)
    {
    QMessageBox::warning(this, tr("Invalid Model"), tr("The big screen is not arranged"));
    return;
    }*/
    OTS_LOG->debug("start apply .....................");
    std::vector<ots::PairDecodeToIRect> vecDecodeToIRect;
    vecDecodeToIRect.clear();

    QVector<QRect> temp_vectItem;

    for (int i = 0; i < _screen_row; ++i)
    {
        for (int j = 0; j < _screen_col; ++j)
        {
            ots::PairDecodeToIRect pairInfo;
            int num = i * _screen_col + j;
            pairInfo.first = _mapDecoder[num].id; //页面布局index对应的解码器ID
            int layout = 0;/* 层级从0层开始 */

            std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int > > vecIRect;
            QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
            //Todo: //解码器上的Rect，编码器ID，信号源Rect，lay,码流类型（用实际矩形大小layRect判断）
            temp_vectItem.clear();
            for (int itemLayout = 0; itemLayout < listItem.size(); itemLayout++)
            {
                if (!_pScreenLayoutWidget->existItem(listItem[itemLayout]))
                {
                    listItem[itemLayout]->deleteLater();
                    continue;
                }
                QRectF layRect = listItem[itemLayout]->getRealityRectF(); //布局Rect

                QRectF rectInter = layRect.intersected(devRect);//重叠区域

                if (rectInter.width() > 1 && rectInter.height() > 1)
                {
                    //计算解码器画面在拼接大屏上的位置信息
                    QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                    ots::IRect destRt;
                    destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                    destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                    destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                    destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

                    QRect tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                    /*
                    如果listItem[itemLayout]的矩形跟temp_vectItem  每个item比较是否有交集，有则将layout++，temp_vectItem清除，如果没有交集，
                    */
                    for (int temp = 0; temp < temp_vectItem.size(); temp++)
                    {
                        if (tempDesRect.intersected(temp_vectItem[temp]) != QRect())
                        {
                            layout++;
                            temp_vectItem.clear();
                            break;
                        }
                    }

                    //计算编码器画面在拼接大屏上的位置
                    QRectF rtran = rectInter.translated(layRect.topLeft() * -1);
                    float fx = (float(layRect.width())) / 1920.0;
                    float fy = (float(layRect.height())) / 1080.0;
                    ots::IRect srcRt;
                    /* 是否需要对视频源进行4的倍数计算 */

#ifdef SRC_4
                    srcRt.left = getNumforT((long)(rtran.left() / fx), 4);
                    srcRt.top = getNumforT((long)(rtran.top() / fy), 4);
                    srcRt.right = getNumforT((long)((rtran.left() + rtran.width()) / fx), 4);
                    srcRt.bottom = getNumforT((long)((rtran.top() + rtran.height()) / fy), 4); #else
#else
                    srcRt.left = (long)(rtran.left() / fx);
                    srcRt.top = (long)(rtran.top() / fy);
                    srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                    srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);
#endif

                    vecIRect.push_back(std::make_tuple(destRt, listItem[itemLayout]->getEncoderID(), srcRt, layout, 0));//目的矩形、编码器ID、源矩形

                    /*Todo:保存矩形大小*/
                    temp_vectItem.push_back(tempDesRect);
                }
            }
            pairInfo.second = vecIRect;
            vecDecodeToIRect.push_back(pairInfo);
        }
    }

    OTS_LOG->debug("111111 apply .....................");
    // 发送应用命令
    otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecodeToIRect);

    //若开启了镜像通道，则在切换预案时，发送投屏指令
    std::vector<std::tuple<int, std::string, int> > allMirror;
    int retCode = otsServer::DataSession::instance()->getMirrorByScreenID(_screenID, allMirror);
    for (int i = 0; i < allMirror.size(); i++)
    {
        if (std::get<2>(allMirror[i]) == 1)
        {
            /* 通过镜像通道ID拉取该方案下的大屏ID */
            QVector<QRectF> temp_vectRectItem;
            std::vector<int> vecAllScreenID;
            otsServer::DataSession::instance()->getScreenByMirrorID(std::get<0>(allMirror[i]), vecAllScreenID);

            /* 获取当前大屏信息(大屏行、列、对应行数、列数的解码板ID) */
            std::tuple<int, int, int> rowColPro;
            std::vector<std::tuple<int, int, int> > vecDecoderID;

            qInfo() << "当前大屏ID：" << _screenID;
            bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(_screenID, rowColPro, vecDecoderID);
            if (!retCode)
            {
                qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                return;
            }

            int row = std::get<0>(rowColPro);
            int col = std::get<1>(rowColPro);
            int totalWidth = col * 1920;
            int totalHeight = row * 1080;
            //param vecTupleLayout  导出的结果 <编码板ID, 布局名称, 布局RECT, 次序>
            std::vector<std::tuple<int, std::string, int, QRectF> > vecTupleLayout;
            std::vector<std::tuple<int, std::string, int, QRectF> > vecNewTupleLayout;

            //从当前界面获取排布信息
            QList<ScreenLayoutItem*> listScreenLayout = this->getScreenLayoutItems();
            qInfo() << "vecScreenLayoutSize:" << listScreenLayout.size();
            for (int i = 0; i < listScreenLayout.size(); i++)
            {
                if (!_pScreenLayoutWidget->existItem(listScreenLayout[i]))
                {
                    listScreenLayout[i]->deleteLater();
                    continue;
                }

                qreal left = listScreenLayout[i]->getRealityRectF().left();
                qreal top = listScreenLayout[i]->getRealityRectF().top();
                qreal right = listScreenLayout[i]->getRealityRectF().right();
                qreal bottom = listScreenLayout[i]->getRealityRectF().bottom();
                int sequence = i;
                int encoderId = listScreenLayout[i]->getEncoderID();
                qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
                std::tuple<int, std::string, int, QRectF> layoutInfo;
                layoutInfo = std::make_tuple(encoderId, "", sequence, QRectF(left, top, right - left, bottom - top));
                vecTupleLayout.push_back(layoutInfo);
            }

            for (int i = 0; i < vecTupleLayout.size(); i++)
            {
                //计算当前编码器信号在当前大屏上的屏幕占比
                int encoderId = std::get<0>(vecTupleLayout[i]);
                std::string layoutName = std::get<1>(vecTupleLayout[i]);
                int sequence = std::get<2>(vecTupleLayout[i]);
                QRectF curRect = std::get<3>(vecTupleLayout[i]);
                QRectF ratioRectF = QRectF(1.0 * curRect.x() / totalWidth, 1.0 * curRect.y() / totalHeight, 1.0 * curRect.width() / totalWidth, 1.0 * curRect.height() / totalHeight);

                vecNewTupleLayout.push_back(std::make_tuple(encoderId, layoutName, sequence, ratioRectF));
                qInfo() << std::get<3>(vecNewTupleLayout[i]);
            }

            /* <解码器ID、目的矩形、编码器ID、源矩形>vector */
            std::vector<ots::PairDecodeToIRect> vecDecoderToIRect;

            //通过大屏ID获取大屏信息
            for (int k = 0; k < vecAllScreenID.size(); k++)
            {
                /*<自身大屏不重复投屏>*/
                if (vecAllScreenID[k] == _screenID)
                {
                    continue;
                }

                std::map<int, int> mapIndex2Decoder;
                mapIndex2Decoder.clear();

                std::tuple<int, int, int> rowCol;
                std::vector<std::tuple<int, int, int> > vecDecoderInfo;
                bool retCode = otsServer::DataSession::instance()->getScreenDecoderInfo(vecAllScreenID[k], rowCol, vecDecoderInfo);
                if (!retCode)
                {
                    qInfo() << QStringLiteral("获取当前大屏信息失败！！！！");
                    return;
                }
                //大屏的行列，宽高信息
                int newRow = std::get<0>(rowCol);
                int newCol = std::get<1>(rowCol);
                int newTotalWidth = newCol * 1920;
                int newTotalHeight = newRow * 1080;

                /*解析vecDecoderInfo,根据解码板所在行列数，计算后将位置Index与decoderID对应起来*/
                for (std::size_t i = 0; i < vecDecoderInfo.size(); ++i)
                {
                    int decoderId = std::get<2>(vecDecoderInfo[i]);
                    int num = std::get<0>(vecDecoderInfo[i]) * newCol + std::get<1>(vecDecoderInfo[i]);
                    mapIndex2Decoder[num] = decoderId;
                }

                //保存帧数据
                bool tempState = true;
                std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
                //<left, top, right, botto, seq, encoderid>

                for (int i = 0; i < newRow; ++i)
                {
                    for (int j = 0; j < newCol; ++j)
                    {
                        temp_vectRectItem.clear();
                        int lay = 0; //默认在第0层

                        ots::PairDecodeToIRect pairInfo;
                        int num = i * newCol + j;
                        pairInfo.first = mapIndex2Decoder[num]; //页面布局index对应的解码器ID

                        std::vector<std::tuple<ots::IRect, int, ots::IRect, int, int> > vecIRect;
                        QRectF devRect(j * 1920, i * 1080, 1920, 1080);//(x,y,width,height)//解码器在大屏上的Rect
                        for (int itemLayout = 0; itemLayout < vecNewTupleLayout.size(); itemLayout++)
                        {
                            QRectF layRectF = std::get<3>(vecNewTupleLayout[itemLayout]); //布局Rect--占比Rect
                            layRectF = QRectF(layRectF.x() * newTotalWidth, layRectF.y() * newTotalHeight, layRectF.width() * newTotalWidth, layRectF.height() * newTotalHeight);
                            QRectF newLayRect = layRectF;

                            if (tempState)
                            {
                                std::tuple<int, int, int, int, int, int > layoutInfo;
                                layoutInfo = std::make_tuple(newLayRect.left(), newLayRect.top(), newLayRect.right(), newLayRect.bottom(), itemLayout, std::get<0>(vecNewTupleLayout[itemLayout]));
                                vecLayoutInfo.push_back(layoutInfo);
                            }

                            QRectF rectInter = newLayRect.intersected(devRect);//重叠区域

                            if (rectInter.width() > 1 && rectInter.height() > 1)
                            {
                                //计算解码器画面在拼接大屏上的位置信息
                                QRectF rtDest = rectInter.translated(devRect.topLeft() * -1);
                                ots::IRect destRt;
                                destRt.left = getNumforT(rtDest.left() + 0.5, 4);
                                destRt.top = getNumforT(rtDest.top() + 0.5, 4);
                                destRt.right = getNumforT(rtDest.left() + rtDest.width() + 0.5, 4);
                                destRt.bottom = getNumforT(rtDest.top() + rtDest.height() + 0.5, 4);

                                //2019年10月14日11:05:49
                                QRectF tempDesRect(destRt.left, destRt.top, destRt.right - destRt.left, destRt.bottom - destRt.top);

                                /*
                                如果listItem[itemLayout]的矩形跟temp_vectRectItem  每个item比较是否有交集，有则将layout++，temp_vectRectItem清除，如果没有交集，
                                */
                                for (int temp = 0; temp < temp_vectRectItem.size(); temp++)
                                {
                                    if (tempDesRect.intersected(temp_vectRectItem[temp]) != QRect())
                                    {
                                        lay++;
                                        temp_vectRectItem.clear();
                                        break;
                                    }
                                }

                                //计算编码器画面在拼接大屏上的位置
                                QRectF rtran = rectInter.translated(newLayRect.topLeft() * -1);
                                float fx = (float(newLayRect.width())) / 1920.0;
                                float fy = (float(newLayRect.height())) / 1080.0;
                                ots::IRect srcRt;
                                srcRt.left = (long)(rtran.left() / fx);
                                srcRt.top = (long)(rtran.top() / fy);
                                srcRt.right = (long)((rtran.left() + rtran.width()) / fx);
                                srcRt.bottom = (long)((rtran.top() + rtran.height()) / fy);

                                temp_vectRectItem.push_back(tempDesRect);
                                vecIRect.push_back(std::make_tuple(destRt, std::get<0>(vecTupleLayout[itemLayout]), srcRt, lay, 0));//目的矩形、编码器ID、源矩形
                            }
                        }
                        tempState = false;
                        pairInfo.second = vecIRect;
                        vecDecoderToIRect.push_back(pairInfo);
                    }
                }

                //镜像开启后,保存每块大屏的帧数据
                int tempIndex = -1;
                otsServer::DataSession::instance()->getIndexByScreenID(vecAllScreenID[k], tempIndex);
                qInfo() << "index:" << tempIndex;
                int backIndexOfOtherScreen = -1;
                retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(backIndexOfOtherScreen, tempIndex, 0, vecAllScreenID[k], vecLayoutInfo, _layoutModelType);/*<开启镜像，排布类型相同>*/
                retCode = otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[k], backIndexOfOtherScreen);
            }

            // 发送应用命令
            otsControler::ControlerData::instance()->sendPlayStreamConfig(vecDecoderToIRect);
        }
    }
#endif

#if 0 
    //保存当前大屏界面帧数据
    //时间： 2019-10-16 15：44 解决插销重做异常问题！！！！ by songwentao
    //保存当前镜像通道中所有大屏界面帧数据
    std::vector<std::tuple<int, int, int, int, int, int > > vecLayoutInfo;
    QVector<ScreenLayoutItem*> vecScreenLayout = _pScreenLayoutWidget->getVecScreenLayoutItems();
    qInfo() << "vecScreenLayoutSize:" << vecScreenLayout.size();
    for (int i = 0; i < vecScreenLayout.size(); i++)
    {
        int left = vecScreenLayout[i]->getRealityRect().left();
        int top = vecScreenLayout[i]->getRealityRect().top();
        int right = vecScreenLayout[i]->getRealityRect().right();
        int bottom = vecScreenLayout[i]->getRealityRect().bottom();
        int sequence = i;
        int encoderId = vecScreenLayout[i]->getEncoderID();
        qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
        std::tuple<int, int, int, int, int, int> layoutInfo;
        layoutInfo = std::make_tuple(left, top, right, bottom, sequence, encoderId);
        vecLayoutInfo.push_back(layoutInfo);
    }


    int tempIndex = 0;
    otsServer::DataSession::instance()->saveMouldFrame2DB(tempIndex, -1, 0, _screenID, vecLayoutInfo, _layoutModelType);
#endif 

    OTS_LOG->debug("end apply .....................");
}

void ScreenWidget::slotItemRectChanged(QObject* pItem, QRect newRect)
{
    //OTS_LOG->debug("start time...");
    ScreenLayoutItem* pSendItem = qobject_cast<ScreenLayoutItem*>(pItem);
    if (!pSendItem)
    {
        return;
    }
    QRect videoRect = pSendItem->getRealityRect();

    /* 判断是否贴合边界，通过视频布局判断 */
    int n2PixConvert = (int)(2.0 / _pointRatio.x() + 0.9);
    const int min_MAGNETIC_SPACE = 64;
    n2PixConvert = n2PixConvert < min_MAGNETIC_SPACE ? min_MAGNETIC_SPACE : n2PixConvert;

    videoRect.setLeft(checkRectPos(videoRect.left(), 1920 -1, n2PixConvert));
    videoRect.setTop(checkRectPos(videoRect.top(), 1080-1, n2PixConvert));
    videoRect.setRight(checkRectPos(videoRect.right(), 1920-1, n2PixConvert));
    videoRect.setBottom(checkRectPos(videoRect.bottom(), 1080-1, n2PixConvert));

    if (videoRect.right() % 1920 == 0)
    {
        videoRect.setRight(videoRect.right() - 1);
    }

    if (videoRect.bottom() % 1080 == 0)
    {
        videoRect.setBottom(videoRect.bottom() - 1);
    }

    /* 设置最小大小 */
    if (videoRect.width() < 200)
    {
        videoRect.setRight(videoRect.left() + 199);
    }
    if (videoRect.height() < 200)
    {
        videoRect.setBottom(videoRect.top() + 199);
    }

    /* 判断是否越界 */
    if (videoRect.right() >= _screen_col * 1920)
    {
        videoRect.setRight(_screen_col * 1920 - 1);
    }
    if (videoRect.bottom() >= _screen_row * 1080)
    {
        videoRect.setBottom(_screen_row * 1080 - 1);
    }

    /* 判断与其它边是否贴合 */
    QList<ScreenLayoutItem*> listItems = getScreenLayoutItems();
    for (auto iter = listItems.begin(); iter != listItems.end(); ++iter)
    {
        if (*iter == pSendItem)
        {
            continue;
        }
        /* 判断水平方向 */
        QRect vRect = (*iter)->getRealityRect();
        if ((vRect.top() <= videoRect.top() && videoRect.top() <= vRect.bottom())				/* rt1 的上边界在 rt2之间 */
            || (vRect.top() <= videoRect.bottom() && videoRect.bottom() <= vRect.bottom())	/* rt1 的下边界在 rt2之间 */
            || (videoRect.top() <= vRect.top() && vRect.top() <= videoRect.bottom())			/* rt2 的上边界在 rt1之间 */
            || (videoRect.top() <= vRect.bottom() && vRect.bottom() <= videoRect.bottom())	/* rt2 的下边界在 rt1之间 */
            )
        {
            /* 新位置的左边界 */
            if (std::abs(vRect.right() - videoRect.left()) <= n2PixConvert)
                videoRect.setLeft(vRect.right()+1);
            /* 右边界 */
            if (std::abs(vRect.left() - videoRect.right()) <= n2PixConvert)
                videoRect.setRight(vRect.left()-1);
        }
        /* 判断垂直方向 */
        if ((vRect.left() <= videoRect.left() && videoRect.left() <= vRect.right())		/* rt1 的左边界在 rt2之间 */
            || (vRect.left() <= videoRect.right() && videoRect.right() <= vRect.right())	/* rt1 的右边界在 rt2之间 */
            || (videoRect.left() <= vRect.left() && vRect.left() <= videoRect.right())	/* rt2 的左边界在 rt1之间 */
            || (videoRect.left() <= vRect.right() && vRect.right() <= videoRect.right())	/* rt2 的右边界在 rt1之间 */
            )
        {
            /* 新位置的上边界 */
            if (abs(videoRect.top() - vRect.bottom()) <= n2PixConvert)
                videoRect.setTop(vRect.bottom()+1);
            /* 下边界 */
            if (abs(videoRect.bottom() - vRect.top()) <= n2PixConvert)
                videoRect.setBottom(vRect.top()-1);
        }
    }

    /* 首先移动到新的位置 */
    pSendItem->setRealityRect(videoRect);
    //OTS_LOG->debug("end time...");
	//emit signalSaveFrameToDB();
    slotApplyScreen();
}

void ScreenWidget::slotItemRectFChanged(QObject* pItem, QRectF newRect)
{
    //OTS_LOG->debug("start time...");
    ScreenLayoutItem* pSendItem = qobject_cast<ScreenLayoutItem*>(pItem);
    if (!pSendItem)
    {
        return;
    }
    QRectF videoRect = pSendItem->getRealityRectF();

    /* 判断是否贴合边界，通过视频布局判断 */
    int n2PixConvert = (int)(2.0 / _pointRatio.x() + 0.9);
    const int min_MAGNETIC_SPACE = 64;
    n2PixConvert = n2PixConvert < min_MAGNETIC_SPACE ? min_MAGNETIC_SPACE : n2PixConvert;

    videoRect.setLeft(checkRectPos(videoRect.left() + 0.5, 1920, n2PixConvert));
    videoRect.setTop(checkRectPos(videoRect.top() + 0.5, 1080, n2PixConvert));
    videoRect.setRight(checkRectPos(videoRect.right() + 0.5, 1920, n2PixConvert));
    videoRect.setBottom(checkRectPos(videoRect.bottom() + 0.5, 1080, n2PixConvert));

     /*if (videoRect.right() == 1920)
     {
     videoRect.setRight(videoRect.right() - 1);
     }

     if (videoRect.bottom() == 1080)
     {
     videoRect.setBottom(videoRect.bottom() - 1);
     }*/

    /* 设置最小大小 */
    if (videoRect.width() < 200)
    {
        videoRect.setRight(videoRect.left() + 200);
    }
    if (videoRect.height() < 200)
    {
        videoRect.setBottom(videoRect.top() + 200);
    }

    /* 判断是否越界 */
    if (videoRect.right() >= _screen_col * 1920)
    {
        videoRect.setRight(_screen_col * 1920);
    }
    if (videoRect.bottom() >= _screen_row * 1080)
    {
        videoRect.setBottom(_screen_row * 1080);
    }

    /* 判断与其它边是否贴合 */
    QList<ScreenLayoutItem*> listItems = getScreenLayoutItems();
    for (auto iter = listItems.begin(); iter != listItems.end(); ++iter)
    {
        if (*iter == pSendItem)
        {
            continue;
        }
        /* 判断水平方向 */
        QRectF vRect = (*iter)->getRealityRectF();
        if ((vRect.top() <= videoRect.top() && videoRect.top() <= vRect.bottom())				/* rt1 的上边界在 rt2之间 */
            || (vRect.top() <= videoRect.bottom() && videoRect.bottom() <= vRect.bottom())	/* rt1 的下边界在 rt2之间 */
            || (videoRect.top() <= vRect.top() && vRect.top() <= videoRect.bottom())			/* rt2 的上边界在 rt1之间 */
            || (videoRect.top() <= vRect.bottom() && vRect.bottom() <= videoRect.bottom())	/* rt2 的下边界在 rt1之间 */
            )
        {
            /* 新位置的左边界 */
            if (std::abs(vRect.right() - videoRect.left()) <= n2PixConvert)
                videoRect.setLeft(vRect.right());
            /* 右边界 */
            if (std::abs(vRect.left() - videoRect.right()) <= n2PixConvert)
                videoRect.setRight(vRect.left());
        }
        /* 判断垂直方向 */
        if ((vRect.left() <= videoRect.left() && videoRect.left() <= vRect.right())		/* rt1 的左边界在 rt2之间 */
            || (vRect.left() <= videoRect.right() && videoRect.right() <= vRect.right())	/* rt1 的右边界在 rt2之间 */
            || (videoRect.left() <= vRect.left() && vRect.left() <= videoRect.right())	/* rt2 的左边界在 rt1之间 */
            || (videoRect.left() <= vRect.right() && vRect.right() <= videoRect.right())	/* rt2 的右边界在 rt1之间 */
            )
        {
            /* 新位置的上边界 */
            if (abs(videoRect.top() - vRect.bottom()) <= n2PixConvert)
                videoRect.setTop(vRect.bottom());
            /* 下边界 */
            if (abs(videoRect.bottom() - vRect.top()) <= n2PixConvert)
                videoRect.setBottom(vRect.top());
        }
    }

    /* 首先移动到新的位置 */
    pSendItem->setRealityRectF(videoRect);
    slotApplyScreen();
}

void ScreenWidget::slotClearScreen()
{
	/*<清屏之前保存当前页面的界面排布>*/
    _pScreenLayoutWidget->cleanScreenLayoutItems();
	//emit signalSaveFrameToDB();
    slotApplyScreen();
}

void ScreenWidget::slotSaveFrameToDB()
{
	int screenId = _screenID;
	qInfo() << QStringLiteral("当前大屏ID：") << screenId;
	std::vector<std::tuple<std::string, int, int, int, int, int > > vecLayoutInfo;
	//QVector<ScreenLayoutItem*> vecScreenLayout = _pScreenLayoutWidget->getVecScreenLayoutItems();
    QList<ScreenLayoutItem*> vecScreenLayout = this->getScreenLayoutItems();
	qInfo() << "vecScreenLayoutSize:" << vecScreenLayout.size();
	for (int i = 0; i < vecScreenLayout.size(); i++)
	{
		if (!_pScreenLayoutWidget->existItem(vecScreenLayout[i]))
		{
			continue;
		}
#ifdef OLD
        int left = vecScreenLayout[i]->getRealityRect().left();
        int top = vecScreenLayout[i]->getRealityRect().top();
        int right = vecScreenLayout[i]->getRealityRect().right();
        int bottom = vecScreenLayout[i]->getRealityRect().bottom();
#else
        int left = vecScreenLayout[i]->getRealityRectF().left();
        int top = vecScreenLayout[i]->getRealityRectF().top();
        int right = vecScreenLayout[i]->getRealityRectF().right();
        int bottom = vecScreenLayout[i]->getRealityRectF().bottom();
#endif

		int sequence = i;
		int encoderId = vecScreenLayout[i]->getEncoderID();
		qInfo() << __FUNCTION__ << "," << "left,top,right,bottom,seq,encoderId:" << left << "," << top << "," << right << "," << bottom << "," << sequence << "," << encoderId;
		std::tuple<std::string, int, int, int, int, int> layoutInfo;
		layoutInfo = std::make_tuple("",left,top,right,bottom,encoderId);
		vecLayoutInfo.push_back(layoutInfo);
	}
	int mouldType = _layoutModelType;
	qInfo() << QStringLiteral("预案类型：") << mouldType;

    std::tuple<int, std::string> mouldInfo = std::make_tuple(mouldType, "");

    if (g_masterID == g_self_ID)
    {
        OTS_LOG->debug("send to master...");
        send_mould_change_by_master(g_self_ID, _screenID, mouldInfo, vecLayoutInfo);
    }
    else
    {
        OTS_LOG->debug("send to slave...");
        send_mould_change_by_slave(g_self_ID, _screenID, mouldInfo, vecLayoutInfo);
    }

	/*bool retCode = false;
	if (_saveTemporaryFrameIndex == -1)
	{
		retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(_saveTemporaryFrameIndex, -1, 0, screenId, vecLayoutInfo, mouldType);
	}
	else
	{
		int tempIndex = _saveTemporaryFrameIndex;
		retCode = otsServer::DataSession::instance()->saveMouldFrame2DB(_saveTemporaryFrameIndex, tempIndex, 0, screenId, vecLayoutInfo, mouldType);
	}*/


	//替换新的DB文件后，程序异常崩溃
	//功能实现目的:大屏切换时，保存最后一帧数据
	//保存当前大屏最后一帧数据
	//镜像通道开始，当开启大屏时，也会用到该方法。
#if 0
	std::vector<std::tuple<int, std::string, int> > allMirror;
	retCode = otsServer::DataSession::instance()->getMirrorByScreenID(_screenID, allMirror);
	for (int i = 0; i < allMirror.size(); i++)
	{
		if (std::get<2>(allMirror[i]) == 1)
		{
			/* 通过镜像通道ID拉取该方案下的大屏ID */
			std::vector<int> vecAllScreenID;
			int mirrorId = std::get<0>(allMirror[i]);
			otsServer::DataSession::instance()->getScreenByMirrorID(mirrorId, vecAllScreenID);
			for (int i = 0; i < vecAllScreenID.size(); i++)
			{
				//为当前镜像通道下的所有大屏保存帧数据，因为你不知道切换大屏后，当前是哪一块大屏
				otsServer::DataSession::instance()->setIndexByScreenID(vecAllScreenID[i], _saveTemporaryFrameIndex);
			}
		}
		else
		{
			//如果没有开启镜像，则保存当前大屏帧数据
			otsServer::DataSession::instance()->setIndexByScreenID(_screenID, _saveTemporaryFrameIndex);
		}
	}
	if (allMirror.size() == 0)
	{
		//如果没有镜像，则保存当前大屏帧数据
		otsServer::DataSession::instance()->setIndexByScreenID(_screenID, _saveTemporaryFrameIndex);
	}
	
#endif 

	//if (retCode)
	//{
	//	qInfo() << QStringLiteral("帧数据保存成功！当前返回帧索引为：") << _saveTemporaryFrameIndex;
	//}
	//else
	//{
	//	qInfo() << QStringLiteral("帧数据保存失败！！！");
	//}
}

void ScreenWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&styleOpt,&painter,this);
    //int temp_width = this->size().width() / _screen_col;
    //int temp_height = this->size().height() / _screen_row;
    //for (int i = 0; i < _screen_row; i++)
    //{
    //    for (int j = 0; j < _screen_col; j++)
    //    {
            //painter.drawPixmap(temp_width*j, temp_height*i, _bgPixmap.scaled(QSize(temp_width, temp_height), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    //    }
    //}
    painter.drawPixmap(0, 0, _bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QWidget::paintEvent(event);
}

void ScreenWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //ScreenLayoutItem* pItem = qobject_cast<ScreenLayoutItem*>(childAt(event->pos()));
    //if (pItem)
    //{
    //        QMap<int, QVector<QRect>> mapDecoderRect;
    //        calculationRect2DecoderRect(pItem->geometry(), mapDecoderRect);
    //        pItem->setMapDeocerRect(mapDecoderRect);
    //}
    QWidget::mouseReleaseEvent(event);
}

void ScreenWidget::dragEnterEvent(QDragEnterEvent *event)
{
    _dragEnterRect = QRect();
    QRect rect_ = _pScreenTopSplit->calculateRect(event->pos());
     _dragEnterRect = rect_;
        QMap<int, QVector<QRect>> mapDecoderRect;
        calculationRect2DecoderRect(rect_, mapDecoderRect);
    LinesJson linsJson;
    for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
    {
        int decoderID = item.key();
        linsJson.vecLine.clear();
        QVector<QRect> vecRect = item.value();
        if (vecRect.isEmpty())
        {
            continue;
        }
        Line_ST line;
        line.x0 = vecRect[0].left();
        line.y0 = vecRect[0].top();
        line.x1 = vecRect[0].right();
        line.y1 = vecRect[0].top();
        linsJson.vecLine.push_back(line);
        line.x0 = vecRect[0].left();
        line.y0 = vecRect[0].top();
        line.x1 = vecRect[0].left();
        line.y1 = vecRect[0].bottom();
        linsJson.vecLine.push_back(line);
        line.x0 = vecRect[0].right();
        line.y0 = vecRect[0].top();
        line.x1 = vecRect[0].right();
        line.y1 = vecRect[0].bottom();
        linsJson.vecLine.push_back(line);
        line.x0 = vecRect[0].left();
        line.y0 = vecRect[0].bottom();
        line.x1 = vecRect[0].right();
        line.y1 = vecRect[0].bottom();
        linsJson.vecLine.push_back(line);
        OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
        otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
    }
    event->accept();
    QWidget::dragEnterEvent(event);
}

void ScreenWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect rect_ = _pScreenTopSplit->calculateRect(event->pos());
    if (rect_ != _dragEnterRect)
    {
        QMap<int, QVector<QRect>> mapDecoderRect;
        calculationRect2DecoderRect(_dragEnterRect, mapDecoderRect);
        LinesJson linsJson;
        for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
        {
            int decoderID = item.key();
            OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
            otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
        }
        mapDecoderRect.clear();
        _dragEnterRect = rect_;
        calculationRect2DecoderRect(rect_, mapDecoderRect);
        for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
        {
            int decoderID = item.key();
            linsJson.vecLine.clear();
            QVector<QRect> vecRect = item.value();
            if (vecRect.isEmpty())
            {
                continue;
            }
            Line_ST line;
            line.x0 = vecRect[0].left();
            line.y0 = vecRect[0].top();
            line.x1 = vecRect[0].right();
            line.y1 = vecRect[0].top();
            linsJson.vecLine.push_back(line);
            line.x0 = vecRect[0].left();
            line.y0 = vecRect[0].top();
            line.x1 = vecRect[0].left();
            line.y1 = vecRect[0].bottom();
            linsJson.vecLine.push_back(line);
            line.x0 = vecRect[0].right();
            line.y0 = vecRect[0].top();
            line.x1 = vecRect[0].right();
            line.y1 = vecRect[0].bottom();
            linsJson.vecLine.push_back(line);
            line.x0 = vecRect[0].left();
            line.y0 = vecRect[0].bottom();
            line.x1 = vecRect[0].right();
            line.y1 = vecRect[0].bottom();
            linsJson.vecLine.push_back(line);
            OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
            otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
        }
    }

    event->accept();
    QWidget::dragMoveEvent(event);
}

void ScreenWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QMap<int, QVector<QRect>> mapDecoderRect;
    calculationRect2DecoderRect(_dragEnterRect, mapDecoderRect);
    LinesJson linsJson;
    for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
    {
        int decoderID = item.key();
        OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
        otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
    }
    _dragEnterRect = QRect();
    event->ignore();
    QWidget::dragLeaveEvent(event);
}

void ScreenWidget::dropEvent(QDropEvent *event)
{
#ifdef OLD
    QMap<int, QVector<QRect>> mapDecoderRect;
    calculationRect2DecoderRect(_dragEnterRect, mapDecoderRect);
    LinesJson linsJson;
    for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
    {
        int decoderID = item.key();
        OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
        otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
    }
    _dragEnterRect = QRect();
    if (event->mimeData() && event->mimeData()->hasFormat("TreeModel"))
    {
        QRect rect_ = _pScreenTopSplit->calculateRect(event->pos());

        /** 这块处理有点耗时界面响应慢 */
        OTS_LOG->debug("get items start....");
        QList<ScreenLayoutItem*> listItem;
        listItem = this->getScreenLayoutItems();
        OTS_LOG->debug("get items end....size:{}", listItem.size());
        if (listItem.size() > 15)
        {
            QRect temp;
            for (int i = 0; i < listItem.size(); i++)
            {
                temp = rect_ | listItem[i]->geometry();
                if ((temp.width() - 4) < rect_.width() && (temp.height() - 4) < rect_.height())
                {
                    _pScreenLayoutWidget->remvoeItemFromVector(listItem.at(i));
                    delete listItem.at(i);
                    listItem.removeAt(i);
                    break;
                }
            }
            /* 如果不存在底层的目前就删除堆底的item */
            if (listItem.size() > 15)
            {
                _pScreenLayoutWidget->remvoeItemFromVector(listItem.first());
                delete listItem.first();
                listItem.removeOne(listItem.first());
            }
        }
        OTS_LOG->debug("delet items....");

        QByteArray exData = event->mimeData()->data("TreeModel");
        QDataStream dataStream(&exData, QIODevice::ReadOnly);
        QString strName = "";
        qint32 id = 0;
        dataStream >> strName >> id;
        _pScreenLayoutWidget->addScreenLayoutItem(rect_, id);
#else
    QMap<int, QVector<QRect>> mapDecoderRect;
    calculationRect2DecoderRect(_dragEnterRect, mapDecoderRect);
    LinesJson linsJson;
    for (QMap<int, QVector<QRect>>::Iterator item = mapDecoderRect.begin(); item != mapDecoderRect.end(); item++)
    {
        int decoderID = item.key();
        OTS_LOG->error("{}：{}", decoderID, linsJson.createLineJson2Decoder());
        otsControler::ControlerData::instance()->sendMsgToDecoder(decoderID, linsJson.createLineJson2Decoder());
    }
    _dragEnterRect = QRect();
    if (event->mimeData() && event->mimeData()->hasFormat("TreeModel"))
    {
        QRectF rect_ = _pScreenTopSplit->calculateRectF(event->pos());

        /** 这块处理有点耗时界面响应慢 */
        OTS_LOG->debug("get items start....");
        QList<ScreenLayoutItem*> listItem;
        listItem = this->getScreenLayoutItems();
        OTS_LOG->debug("get items end....size:{}", listItem.size());
        if (listItem.size() > 15)
        {
            QRectF temp;
            for (int i = 0; i < listItem.size(); i++)
            {
                temp = rect_ | listItem[i]->getGeometry();
                if ((temp.width() - 4) < rect_.width() && (temp.height() - 4) < rect_.height())
                {
                    _pScreenLayoutWidget->remvoeItemFromVector(listItem.at(i));
                    delete listItem.at(i);
                    listItem.removeAt(i);
                    break;
                }
            }
            /* 如果不存在底层的目前就删除堆底的item */
            if (listItem.size() > 15)
            {
                _pScreenLayoutWidget->remvoeItemFromVector(listItem.first());
                delete listItem.first();
                listItem.removeOne(listItem.first());
            }
        }
        OTS_LOG->debug("delet items....");

        QByteArray exData = event->mimeData()->data("TreeModel");
        QDataStream dataStream(&exData, QIODevice::ReadOnly);
        QString strName = "";
        qint32 id = 0;
        dataStream >> strName >> id;
        _pScreenLayoutWidget->addScreenLayoutItem(rect_, id);
#endif

    }
	//emit signalSaveFrameToDB();
    event->accept();
    slotApplyScreen();
}

void ScreenWidget::resizeEvent(QResizeEvent *event)
{
    calculationRatio();
    _pScreenLayoutWidget->setRatio(_pointRatio);
    QWidget::resizeEvent(event);
}

void ScreenWidget::showEvent(QShowEvent *event)
{
    slotScreenChanged(_screenID);
    QWidget::showEvent(event);
}

void ScreenWidget::calculationRatio()
{
    const QSizeF videoSize{ 1920.0, 1080.0 };
    _pointRatio = QPointF(this->geometry().width() / videoSize.width() / _screen_col,
        this->geometry().height() / videoSize.height() / _screen_row);
}

int ScreenWidget::checkRectPos(int nPos, int nUnitSize, int nMaxOffset)
{
    if (nPos < 0)
    {
        nPos = 0;
    }

    int nOff = nPos % nUnitSize;
    if (nOff <= nMaxOffset)
    {
        return nPos / nUnitSize * nUnitSize;
    }

    if (std::abs(nUnitSize - nOff) <= nMaxOffset)
    {
        return (nPos / nUnitSize + 1) * nUnitSize;
    }

    return nPos;
}

void ScreenWidget::setScreenLayoutType(int modelType)
{
    _layoutModelType = modelType;
    _pScreenTopSplit->setLayoutModelType(_layoutModelType);
    _pScreenLayoutWidget->cleanScreenLayoutItems();
}

/******************************************/
LinesJson::LinesJson()
{
    strLineColor = "red";
    iLineWidth = 2;
    iLineType=1;
    vecLine.clear();
}

std::string LinesJson::createLineJson2Decoder()
{
    _strLineJson = "{\"method\":1,\"body\":{";
    /* color */
    _strLineJson += "\"color\":\"" + strLineColor+"\",";
    /* line-width */
    _strLineJson += "\"line-width\":" + std::to_string(iLineWidth) + ",";
    /* line-type */
    _strLineJson += "\"line-type\":" + std::to_string(iLineType) + ",";
    /* lines */
    _strLineJson += "\"lines\":[";
    for (int i = 0; i < vecLine.size(); i++)
    {
        if (i > 0)
        {
            _strLineJson += ",";
        }
        _strLineJson += "{";
        _strLineJson += "\"x0\":" + std::to_string(vecLine[i].x0) + ",";
        _strLineJson += "\"y0\":" + std::to_string(vecLine[i].y0) + ",";
        _strLineJson += "\"x1\":" + std::to_string(vecLine[i].x1) + ",";
        _strLineJson += "\"y1\":" + std::to_string(vecLine[i].y1);
        _strLineJson += "}";
    }
    _strLineJson += "]}}";

    return _strLineJson;
}
