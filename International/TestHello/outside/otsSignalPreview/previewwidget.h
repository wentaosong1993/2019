#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>

class QLabel;
class PreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewWidget(QWidget *parent = 0);

    void setID(int id){ _encoderID = id; }
    void setName(QString name);
    /* 设置预览窗口是否可以拖拽 */
    void setEnableDrag(bool enableDrag);

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    int _lastIndex;/**< 上一帧 */
    int _intervalTime;/**< 间隔次数，相隔几次之后才判断为无信号 */
    int _encoderID;/**< 对应编码板ID */
	QString _encoderName; /*< 对应编码器名称> */
    QString _strName;/**< 名称 */
    QImage _bgImage;        /**< 信号画面 */
    QLabel* _pLabelName;/**< 信号名称 */
    bool _enableDrag;/**< 是否允许拖拽 */
};

#endif // PREVIEWWIDGET_H
