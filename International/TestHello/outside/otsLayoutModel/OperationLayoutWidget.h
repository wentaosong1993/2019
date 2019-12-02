#pragma once

#include <QDialog>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QListWidget>
#include <QMouseEvent>
#include <QList>

namespace Ui {
    class SetCommonLayoutDialog;
}

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

class DragListWidget : public QListWidget
{
	Q_OBJECT
public:
	DragListWidget(QWidget *parent = Q_NULLPTR);
	~DragListWidget();

	/*获取目标列表中的排布类型*/
	//QList<int> getModuleTypeList() { return m_moduleTypeList; }
	QVector<int>& getModuleTypeVector() { return m_moduleTypeVector; }

protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void dropEvent(QDropEvent *event) override;
private:
	//记录拖拽的起点.
	QPoint m_dragPoint;
	//记录被拖拽的项.
	QListWidgetItem *m_dragItem;
	//QList<int> m_moduleTypeList;
	QVector<int> m_moduleTypeVector;
};


class SetCommonLayoutDialog :public QDialog
{
    Q_OBJECT
public:
    explicit SetCommonLayoutDialog(QWidget *parent = 0);

	/*保存常用排布数据 */
	void saveCommonModelListData();

protected:
	void changeEvent(QEvent *event);
signals:
	/*发射设置常用排布确认信号*/
	void saveModuleSignal();

private slots:
    void slotDelItem();

	/**<-----所有界面排布---->*/
	void createSrcList();

	void slotOkButton();
	void slotCancelButton();

private:
    Ui::SetCommonLayoutDialog* _ui;
};

//中间控件，绘制分割矩形
class LayoutWidget : public QWidget
{
	Q_OBJECT
public:
	explicit LayoutWidget(QWidget *parent = 0);

	void setType(int type);

private:
	void calculatePaintRect();

protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	int _modelType;/**< 模板类型 */
	std::vector<std::pair<QPointF, QPointF> > _vecLayoutRangeRatio;/**< 排布占比 */
};


