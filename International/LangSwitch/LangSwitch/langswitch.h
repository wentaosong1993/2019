#ifndef LANGSWITCH_H
#define LANGSWITCH_H

#include <QWidget>
class QComboBox;
class QLabel;

class LangSwitch : public QWidget
{
    Q_OBJECT

public:
    LangSwitch(QWidget *parent = 0);
    ~LangSwitch();

private slots:
    void changeLang(int index);

private:
    void createScreen();
    void changeTr(const QString& langCode);
    void refreshLabel();
    QComboBox * combo;
    QLabel * label;
};

#endif // LANGSWITCH_H
