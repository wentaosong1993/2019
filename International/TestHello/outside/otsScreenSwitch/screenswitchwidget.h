#ifndef SCREENSWITCHWIDGET_H
#define SCREENSWITCHWIDGET_H

#include <QListWidget>
#include <QWidget>
#include <QMap>

namespace Ui
{
    class ScreenSwitchWidget;
}
class QPushButton;
class QSpacerItem;
class ScreenSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenSwitchWidget(QWidget *parent = 0);
    ~ScreenSwitchWidget();

    void createTestData();
    void createData();

    void setDefaultScreen();

protected:
	void changeEvent(QEvent *event);

signals:
    void signalScreenID(int screenID);

private slots:
    void slotButtonClicked();
    void slotForward();
    void slotBackward();
    void slotRangeChanged(int min, int max);

private:
    /** 设置两个按钮是否显示 */
    void setShowButton();
    /** 重置进度条三个数据 */
    void resetValue();

private:
    Ui::ScreenSwitchWidget* ui;
    QPushButton* _oldButton;/**< 记录按下按钮 */
    QMap<QPushButton*, int> _mapButton2ID;/**< 记录按钮与ID的映射 */
    QSpacerItem* _pSpacerItem;

    int _currentValue;/**< 进度条当前所在位置 */
    int _maxValue;/**< 进度条最大值 */
    int _pageValue;/**< 进度条每页大小 */
};

#endif // SCREENSWITCHWIDGET_H
