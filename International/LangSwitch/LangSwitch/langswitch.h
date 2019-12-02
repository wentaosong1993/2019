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

    void setText2label(const char* text);

private slots:
    void changeLang(int index);

protected:
    void changeEvent(QEvent* event);

private:
    void createScreen();
    void changeTr(const QString& langCode);
    void refreshLabel();
    QComboBox * combo;
    QLabel * label;
    QString _strText;
};

#endif // LANGSWITCH_H
