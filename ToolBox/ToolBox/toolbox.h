#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>
#include <QToolBox>

class ToolBox : public QToolBox
{
    Q_OBJECT

public:
    ToolBox(QWidget *parent = 0);
    ~ToolBox();
};

#endif // TOOLBOX_H
