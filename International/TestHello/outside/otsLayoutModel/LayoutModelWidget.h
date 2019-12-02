#ifndef LAYOUTMODELWIDGET_H
#define LAYOUTMODELWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QMap>

namespace Ui
{
    class LayoutModelWidget;
}

class LayoutButton;
class QSpacerItem;
class SetCommonLayoutDialog;
class LayoutModelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutModelWidget(QWidget *parent = 0);
    ~LayoutModelWidget();

protected:
	void changeEvent(QEvent * event);

signals:
    void signalScreenLayoutID(int modelID);

public slots:
    void slotChangeItem();
    void createAllModelListData();
    /* <创建常用排布 List >*/
    void createCommonModelListData();
    /** 更改当前选中Item */
    void slotChangedItem(int type);
private slots :
    void slotButtonClicked();

private:
    void createUI();
private:
    SetCommonLayoutDialog * _pCommonLayoutDlg; /* <设置常用界面排布对话框> */

    Ui::LayoutModelWidget* ui;
    int _oldButtonID;/**< 记录按下按钮 */
    QMap<int, LayoutButton*> _mapID2Button;/**< 记录按钮与ID的映射 */
    QSpacerItem* _pSpacerItem;
};

#endif // LAYOUTMODELWIDGET_H
