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
    /* ����Ԥ�������Ƿ������ק */
    void setEnableDrag(bool enableDrag);

protected:
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    int _lastIndex;/**< ��һ֡ */
    int _intervalTime;/**< ����������������֮����ж�Ϊ���ź� */
    int _encoderID;/**< ��Ӧ�����ID */
	QString _encoderName; /*< ��Ӧ����������> */
    QString _strName;/**< ���� */
    QImage _bgImage;        /**< �źŻ��� */
    QLabel* _pLabelName;/**< �ź����� */
    bool _enableDrag;/**< �Ƿ�������ק */
};

#endif // PREVIEWWIDGET_H
