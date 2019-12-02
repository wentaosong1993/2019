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
    /** ����������ť�Ƿ���ʾ */
    void setShowButton();
    /** ���ý������������� */
    void resetValue();

private:
    Ui::ScreenSwitchWidget* ui;
    QPushButton* _oldButton;/**< ��¼���°�ť */
    QMap<QPushButton*, int> _mapButton2ID;/**< ��¼��ť��ID��ӳ�� */
    QSpacerItem* _pSpacerItem;

    int _currentValue;/**< ��������ǰ����λ�� */
    int _maxValue;/**< ���������ֵ */
    int _pageValue;/**< ������ÿҳ��С */
};

#endif // SCREENSWITCHWIDGET_H
