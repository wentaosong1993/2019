#include "qudpapp.h"
#include "ui_qudpapp.h"
#include <QMessageBox>
QUdpApp::QUdpApp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QUdpApp)
{
    ui->setupUi(this);
    mSocket = new QUdpSocket();//创建套接字
    ui->sendBt->setEnabled(false);
}

QUdpApp::~QUdpApp()
{
    delete ui;
}


//==========================发送端====================
//单播选择
void QUdpApp::on_sendSigRb_clicked()
{
    if(ui->sendPortEdit->text().isEmpty() || ui->sendSigAddrEdit->text().isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("请输入单播ip和端口号"));
        //ui->sendSigRb->setChecked(false);
        return;
    }
    qInfo() << QStringLiteral("单播:") << "IP:" << ui->sendSigAddrEdit->text() << \
               "Port:" << ui->sendPortEdit->text();
    sendaddrees.setAddress( ui->sendSigAddrEdit->text());
    sendPort  = ui->sendPortEdit->text();

}

//组播选择
void QUdpApp::on_sendMulRb_clicked()
{
    if(ui->sendPortEdit->text().isEmpty() || ui->sendMulAddrEdit->text().isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("请输入组播ip和端口号"));
        //ui->sendSigRb->setChecked(false);
        return;
    }
    qInfo() << QStringLiteral("组播:") << "IP:" << ui->sendMulAddrEdit->text() << \
               "Port:" << ui->sendPortEdit->text();
   sendaddrees.setAddress( ui->sendMulAddrEdit->text());
   sendPort  = ui->sendPortEdit->text();

}

//广播选择
void QUdpApp::on_sendBroadRb_clicked()
{
    if(ui->sendPortEdit->text().isEmpty() || ui->sendBroadAddrEdit->text().isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("请输入广播ip和端口号"));
        //ui->sendSigRb->setChecked(false);
        return;
    }
    qInfo() << QStringLiteral("广播:") << "IP:" << ui->sendBroadAddrEdit->text() << \
               "Port:" << ui->sendPortEdit->text();
    sendaddrees.setAddress( ui->sendBroadAddrEdit->text());
    sendPort  = ui->sendPortEdit->text();

}

//发送按钮
void QUdpApp::on_sendBt_clicked()
{
    mSocket->writeDatagram(ui->sendMesEdit->toPlainText().toUtf8(),sendaddrees,sendPort.toInt());
}

//检测发送消息对话框中是否有消息
void QUdpApp::on_sendMesEdit_cursorPositionChanged()
{
    if(ui->sendMesEdit->toPlainText().isEmpty())
    {
        ui->sendBt->setEnabled(false);
    }
    else
    {
        ui->sendBt->setEnabled(true);
    }

}

//==========================发送端====================


//==========================接收端=====================

//选择接收
void QUdpApp::on_recvCb_clicked(bool checked)
{
    if(ui->recvPortEdit->text().isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("请输入端口号"));
        ui->recvCb->setChecked(false);
        return;
    }
    if(checked)
    {
        mSocket->bind(QHostAddress::AnyIPv4,ui->recvPortEdit->text().toInt());
        //mSocket->bind(QHostAddress("192.168.1.2"),ui->recvPortEdit->text().toInt());
       // mSocket->bind(ui->recvPortEdit->text().toInt());
        connect(mSocket,SIGNAL(readyRead()),this,SLOT(read_data()));
        ui->recvPortEdit->setEnabled(false);
    }
    else
    {
        mSocket->close();
        ui->recvPortEdit->setEnabled(true);
    }
}

//加入组播
void QUdpApp::on_recvJoinMulBt_clicked()
{
    if(ui->recvMulAddrEdit->text().isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("请输入组播ip"));
        return;
    }
    if(mSocket->joinMulticastGroup(QHostAddress(ui->recvMulAddrEdit->text()))) //加入组播
    {
        ui->recvMulAddr->addItem(ui->recvMulAddrEdit->text());
    }
    else
    {
        QMessageBox::warning(this,QStringLiteral("提示"),QStringLiteral("加入组播失败,请修改ip后继续加入"));
        //return;
    }
}

//退出组播
void QUdpApp::on_recvLeaveMulBt_clicked()
{
    mSocket->leaveMulticastGroup(QHostAddress(ui->recvMulAddr->currentIndex()));//退出组播地址列表当前的组播
    ui->recvMulAddr->removeItem(ui->recvMulAddr->currentIndex()); //删除组播地址列表中当前的组播地址
}

void QUdpApp::read_data()
{
    qInfo() << "read data";
    QByteArray array;
    array.resize(mSocket->bytesAvailable()); //将接收数据的array设置成为要接收数据的大小
    QHostAddress recvaddress;
    quint16 port;
    mSocket->readDatagram(array.data(),array.size(),&recvaddress,&port); //读取数据
    ui->recvList->addItem(array);
}

//==========================接收端=====================
