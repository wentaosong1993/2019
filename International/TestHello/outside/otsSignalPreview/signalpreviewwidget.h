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
    //�����ź�Դ��ID�����ź�Ԥ������
    void slotCreatePreviewWidgetFromGroupID(int ID, int type, QString name);

public:
    ///** ID,Name,is4K */
    //void createListData(std::vector<std::tuple<int, int, std::string>>& vecInfo);

private:
    /** ID,Name,is4K */
    //���ݽӿڷ������ݣ�����ˮƽ�ź�Ԥ���б�
    void createListData(std::vector<std::tuple<int, std::string, int>>& vecInfo);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void slotForward();
    void slotBackward();
    void slotRangeChanged(int min, int max);

private:
    /** ����������ť�Ƿ���ʾ */
    void setShowButton();
    /** ���ý������������� */
    void resetValue();

private:
    Ui::SignalPreviewWidget* ui;
    QSpacerItem* _pSpacerItem;

    int _currentValue;/**< ��������ǰ����λ�� */
    int _maxValue;/**< ���������ֵ */
    int _pageValue;/**< ������ÿҳ��С */
};

#endif // SIGNALPREVIEWWIDGET_H
