#ifndef SCREENTOPSPLIT_H
#define SCREENTOPSPLIT_H

#include <QWidget>
#include <QVector>
#include <QPoint>

/** 大屏不同类型的分割方式 */
class ScreenTopSplit : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenTopSplit(QWidget *parent = 0);

    void setRowCol(int row,int col);
    void setLayoutModelType(int type);

    QRect calculateRect(const QPoint& currentPointf);
    QRectF calculateRectF(const QPointF& currentPointf);
    /*
    功能:计算矩形在当前所占位置的最大矩形
    */
    QRect calculateRectMax(const QRect& rect);
    QRectF calculateRectFMax(const QRectF& rectf);

protected:
	/*
	功能：根据所选模板--排布占比，绘制分割后的矩形，即大屏布局
	*/
    void paintEvent(QPaintEvent *event);
    //void resizeEvent(QResizeEvent *event);

private:
	/*
	功能:简单绘制分割矩形可用，复杂绘制（譬如，6L、6L2形状的分割矩形）无法满足，弃用。
	*/
    void calculationPoints();

	/*
	功能:根据选择的_modelType，保存矩形分割的比例
	*/
    void calculatePaintRect();

private:
    int _row;/**< 排布行 */
    int _col;/**< 排布列 */
    int _modelType;/**< 模板类型 */
    QVector<QPoint> _vecPoint;/**< 排布坐标 */
    std::vector<std::pair<QPointF, QPointF> > _vecLayoutRangeRatio;/**< 排布占比 */
};

#endif // SCREENTOPSPLIT_H
