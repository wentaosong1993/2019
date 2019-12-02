#ifndef SIGNALPREVIEWWIDGET_H
#define SIGNALPREVIEWWIDGET_H

#include <QWidget>

namespace Ui
{
    class SignalPreviewWidget;
}
class QSpacerItem;
class SignalPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalPreviewWidget(QWidget *parent = 0);
    ~SignalPreviewWidget();

    void createTestData();

protected:
	void changeEvent(QEvent *event);

public slots:
    //根据信号源组ID创建信号预览画面
    void slotCreatePreviewWidgetFromGroupID(int ID, int type, QString name);

public:
    ///** ID,Name,is4K */
    //void createListData(std::vector<std::tuple<int, int, std::string>>& vecInfo);

private:
    /** ID,Name,is4K */
    //根据接口返回数据，创建水平信号预览列表
    void createListData(std::vector<std::tuple<int, std::string, int>>& vecInfo);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotForward();
    void slotBackward();
    void slotRangeChanged(int min, int max);

private:
    /** 设置两个按钮是否显示 */
    void setShowButton();
    /** 重置进度条三个数据 */
    void resetValue();

private:
    Ui::SignalPreviewWidget* ui;
    QSpacerItem* _pSpacerItem;

    int _currentValue;/**< 进度条当前所在位置 */
    int _maxValue;/**< 进度条最大值 */
    int _pageValue;/**< 进度条每页大小 */
};

#endif // SIGNALPREVIEWWIDGET_H
