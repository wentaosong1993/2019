#include "ConfigIPIDWidget.h"
#include "ui_ConfigIPIDWidget.h"
#include <QNetworkInterface>
#include <QHostAddress>
#include <QSettings>
#include <QDebug>
#include "ots/Common.h"
#include "otsControler/ControlerData.h"
#include "ots/GUIEvent.h"

ConfigIPIDWidget::ConfigIPIDWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigIPIDWidget)
{
    ui->setupUi(this);
    /**
    * 1.初始化数据
    * 1.1.获取所有IP
    * 1.2.读取本地配置信息
    * 2.设置所有IP
    * 3.将当前配置设置为默认值
    * 4.设置ID
    */
    QStringList strIPList;
    getLocalIP(strIPList);

    QSettings settings("Config.ini", QSettings::IniFormat);
    _strConfigIP = settings.value("IP", "0.0.0.0").toString();
    _iConfigID = settings.value("ID", 0).toInt();

    ui->comboBox->addItems(strIPList);
    if (strIPList.contains(_strConfigIP))
    {
        ui->comboBox->setCurrentText(_strConfigIP);
    }

    ui->lineEdit->setText(QString::number(_iConfigID));
}

ConfigIPIDWidget::~ConfigIPIDWidget()
{
    delete ui;
}

void ConfigIPIDWidget::changeEvent(QEvent *event)
{
	switch (event->type())
	{
		case QEvent::LanguageChange:
		{
			ui->retranslateUi(this);
		}
		break;
	default:
		break;
	}
}

void ConfigIPIDWidget::getLocalIP(QStringList& strIPList)
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            strIpAddress = ipAddressesList.at(i).toString();
            //qDebug() << "strIP:" << strIpAddress;
            strIPList << strIpAddress;
        }
    }
}

void ConfigIPIDWidget::slotOKClicked()
{
    /**
    * 1.点击确定判断数据是否无效
    * 2.有效则创建网动初始化
    * 3.如果需要保存本地文件
    * 4.跳转界面到等待界面
    */

    QString strIP = ui->comboBox->currentText();
    int ID = ui->lineEdit->text().toInt();
    if (strIP.isEmpty())
    {
        return;
    }
    if (ID == 0)
    {
        return;
    }
    if (_strConfigIP != strIP)
    {
        _strConfigIP = strIP;
        QSettings settings("Config.ini", QSettings::IniFormat);
        settings.setValue("IP", _strConfigIP);
    }
    if (_iConfigID != ID)
    {
        _iConfigID = ID;
        QSettings settings("Config.ini", QSettings::IniFormat);
        settings.setValue("ID", _iConfigID);
    }

    ots::g_strLocalHostIP = _strConfigIP.toStdString();
    ots::g_iLocalHostID = _iConfigID;

    /* 初始化大屏控制  w00001 添加设置ip地址接口 */
    QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_WAITE, QObject::tr("Initialise SDK...")));
    otsControler::ControlerData::instance()->initialize();
    QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_WAITE, QObject::tr("Initialise SDK  complete...")));
}

void ConfigIPIDWidget::slotCancelClicked()
{
    /**
    * 1.取消之后关闭进程
    */

    QCoreApplication::exit();
}
