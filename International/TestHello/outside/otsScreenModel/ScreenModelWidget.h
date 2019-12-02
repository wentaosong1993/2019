#pragma once

#include <QListWidget>
#include <QWidget>

//大屏预案列表
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

    /*应用预案*/
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
    int _screenID;/**< 大屏ID */
    Ui::ScreenModelWidget* ui;
    bool _isAutoPreview;/**< 是否自动预览 */
};
