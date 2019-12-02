#pragma once

#include <QListWidget>
#include <QWidget>

//����Ԥ���б�
namespace Ui
{
    class ScreenModelWidget;
}
class ScreenModelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenModelWidget(QWidget *parent = 0);
    ~ScreenModelWidget();

    int getCurrentScreenID(){ return _screenID; }

    void setCurrentItem(QListWidgetItem *item);

    void delSelectedItem();

    bool isAutoPreview(){ return _isAutoPreview; }

public slots:
    void slotScreenChanged(int screenID);
    void slotAddModelItem(int modelID, QString modelName);

signals:
    void signalScreenModelID(int screenModelID);
    void signalSaveModel();
    void signalDelModel(int modelID);

    /*Ӧ��Ԥ��*/
    void signalApplyScreenModule();

private slots:
    void slotListItemChange(QListWidgetItem *current, QListWidgetItem *previous);
    void slotItemClicked(QListWidgetItem *current);
    void slotButtonClicked();

private:
    void createData(int screenID);

protected:
    void paintEvent(QPaintEvent *event);
	void changeEvent(QEvent *event);

private:
    int _screenID;/**< ����ID */
    Ui::ScreenModelWidget* ui;
    bool _isAutoPreview;/**< �Ƿ��Զ�Ԥ�� */
};
