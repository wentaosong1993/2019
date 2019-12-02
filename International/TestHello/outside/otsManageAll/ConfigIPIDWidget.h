#ifndef CONFIGIPIDWIDGET_H
#define CONFIGIPIDWIDGET_H

#include <QWidget>

namespace Ui {
class ConfigIPIDWidget;
}

class ConfigIPIDWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigIPIDWidget(QWidget *parent = 0);
    ~ConfigIPIDWidget();

protected:
	void changeEvent(QEvent *event);

private:
    void getLocalIP(QStringList& strIPList);

private slots:
void slotOKClicked();
void slotCancelClicked();

private:
    Ui::ConfigIPIDWidget *ui;
    QString _strConfigIP;
    int _iConfigID;
};

#endif // CONFIGIPIDWIDGET_H
