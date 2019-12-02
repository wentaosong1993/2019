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
    /* <���������Ų� List >*/
    void createCommonModelListData();
    /** ���ĵ�ǰѡ��Item */
    void slotChangedItem(int type);
private slots :
    void slotButtonClicked();

private:
    void createUI();
private:
    SetCommonLayoutDialog * _pCommonLayoutDlg; /* <���ó��ý����Ų��Ի���> */

    Ui::LayoutModelWidget* ui;
    int _oldButtonID;/**< ��¼���°�ť */
    QMap<int, LayoutButton*> _mapID2Button;/**< ��¼��ť��ID��ӳ�� */
    QSpacerItem* _pSpacerItem;
};

#endif // LAYOUTMODELWIDGET_H
