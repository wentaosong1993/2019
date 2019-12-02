#include <QApplication>
#include "MessageBox.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString strQSS;
    strQSS += "MessageBox QLabel#titleLabel{max-height:30px;background-color: rgba(13,34,100,1);font-size:14px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}";
    strQSS += "MessageBox QFrame#line{background-color: rgba(0,84,162,1);}";
    strQSS += "MessageBox{background:rgba(13,34,61,1);border:2px solid rgba(0,84,162,1);}";
    strQSS += "MessageBox QPushButton{width:108px;\
            height:40px;\
            font-size:16px;\
            font-family:Microsoft YaHei;\
            font-weight:400;\
            color:rgba(255,255,255,1);border-image:url(\":/round-button-normal.png\")}";
    strQSS += "MessageBox QPushButton:hover{border-image:url(\":/round-button-pressed.png\")}";
    strQSS += "MessageBox QLabel#centerLabel{font-size:14px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}";
    strQSS += "MessageBox QLabel#icoLabel{min-width:50px;min-height:50px;max-width:50px;max-height:50px;background-color:red;}";
    a.setStyleSheet(strQSS);


    otsCommon::SCMessageBox e("warring", "arararar", otsCommon::SCMessageBox::Information);
    e.exec();
//    Widget w;
//    w.show();

    return a.exec();
}
