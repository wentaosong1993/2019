#include "previewwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include <QDataStream>
#include <QByteArray>
#include <QLabel>
#include <QVBoxLayout>
#include "ots/Common.h"
#include "otsDecoderVideo/ffmpeg_h264.h"

PreviewWidget::PreviewWidget(QWidget *parent) : QWidget(parent), _enableDrag(false)
{
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setMargin(0);

    pLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    QString strQSS = "max-height:30px;\
                      min-height:30px;\
                      background-color: #0093FE;\
                      font-size:14px;\
                      font-family:Microsoft YaHei;\
                      color:rgba(255, 255, 255, 255);\
                      qproperty-alignment:AlignCenter; ";

    _pLabelName = new QLabel(this);
    _pLabelName->setObjectName("NameLabel");
    _pLabelName->setText(tr("Unknown Signal"));
    _pLabelName->setStyleSheet(strQSS);
    pLayout->addWidget(_pLabelName);

    this->setLayout(pLayout);
    _bgImage = QImage(244, 137, QImage::Format_RGB32);
    _bgImage.fill(Qt::black);

    _lastIndex = 0;
    _intervalTime = 0;
    update();
    this->startTimer(34);/* 15֡�����������66.6666ms 30֡���33.3333������ˢ�°�33�� */
}

void PreviewWidget::setName(QString name)
{
    _encoderName = name;
    _pLabelName->setText(_encoderName);
}

void PreviewWidget::setEnableDrag(bool enableDrag)
{
    _enableDrag = enableDrag;
}

void PreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption styleOpt;
    styleOpt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget,&styleOpt,&painter,this);

    ots::g_DecoderLock.lockForRead();

    if ((ots::g_mapDeviceID2H264Decoder.find(_encoderID) != ots::g_mapDeviceID2H264Decoder.end()) && ots::g_mapDeviceID2H264Decoder.find(_encoderID)->second->isStart())
    {
        H264Decoder* pTemp = ots::g_mapDeviceID2H264Decoder.find(_encoderID)->second;
        _intervalTime++;
        if (_intervalTime == 4)
        {
            _intervalTime = 0;
            int currentIndex = pTemp->getIndex();
            if (currentIndex == _lastIndex)
            {
                /* �ź�Դû�й������� */
                if (_enableDrag)
                {
                    _enableDrag = false;
                }
            }
            else
            {
                _lastIndex = currentIndex;
                if (!_enableDrag)
                {
                    _enableDrag = true;
                }
            }
        }
        if (_enableDrag)
        {
            _bgImage = pTemp->getImage();
        }
        else
        {
            _bgImage.fill(Qt::black);
        }
    }
    ots::g_DecoderLock.unlock();

    painter.drawImage(0, 0, _bgImage.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    QWidget::paintEvent(event);
}

void PreviewWidget::timerEvent(QTimerEvent *event)
{
    update();
}

void PreviewWidget::mousePressEvent(QMouseEvent *event)
{
    /* ��ק���ݷ���
    Name  ID   Resolution
    */
    if (!_enableDrag)
    {
        return;
    }
    QDrag* pDrag = new QDrag(this);
    QMimeData* data = new QMimeData;
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << (_strName) << (qint32)(_encoderID) << (qint32)(18);
    data->setData("TreeModel", itemData);//keyֵ��Ҫ��Drop��һ��
    pDrag->setMimeData(data);
    pDrag->setPixmap(QPixmap::fromImage(_bgImage).scaled(this->size()));/* ������ק��ʾͼƬ */
    pDrag->setHotSpot(this->rect().center());/* ������ק�����ͼƬ�е�λ�� */
    pDrag->exec();
}
