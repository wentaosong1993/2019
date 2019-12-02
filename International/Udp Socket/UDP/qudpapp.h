#ifndef QUDPAPP_H
#define QUDPAPP_H

#include <QWidget>
#include <QUdpSocket>

namespace Ui {
class QUdpApp;
}

class QUdpApp : public QWidget
{
    Q_OBJECT

public:
    explicit QUdpApp(QWidget *parent = 0);
    ~QUdpApp();

private slots:
    void on_sendSigRb_clicked(); //单播旋转轴
    void on_sendMulRb_clicked();  //组播选择
    void on_sendBroadRb_clicked(); //广播选择
    void on_sendBt_clicked(); //发送按钮

//===========================================
    void on_recvCb_clicked(bool checked);  //选择接收
    void on_recvJoinMulBt_clicked();  //加入组播
    void on_recvLeaveMulBt_clicked(); //退出组播

    void on_sendMesEdit_cursorPositionChanged();//检测消息框是否有数据
    void read_data();

private:
    Ui::QUdpApp *ui;
    QUdpSocket *mSocket;
    QHostAddress sendaddrees;
    QString sendPort;
};

#endif // QUDPAPP_H
