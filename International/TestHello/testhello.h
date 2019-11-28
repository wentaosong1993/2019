#ifndef TESTHELLO_H
#define TESTHELLO_H

#include <QWidget>

namespace Ui {
class TestHello;
}

class TestHello : public QWidget
{
    Q_OBJECT

public:
    explicit TestHello(QWidget *parent = nullptr);
    ~TestHello();

private:
    Ui::TestHello *ui;
};

#endif // TESTHELLO_H
