#ifndef START_INITDATA_H
#define START_INITDATA_H

#include <QWidget>
#include <QString>

namespace Ui {
class Start_InitData;
}

class Start_InitData : public QWidget
{
    Q_OBJECT

public:
    explicit Start_InitData(QWidget *parent = 0);
    ~Start_InitData();

    /*
    * Date: 2019-11-4
    * Author: John_Yang
    * Note: 设置等待界面提示信息
    */
    void setDiaplayText(const QString& strDisplayText);

protected:
	void changeEvent(QEvent *event);

private:
    Ui::Start_InitData *ui;
};

#endif // START_INITDATA_H
