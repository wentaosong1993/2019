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
    * 1.��ʼ������
    * 1.1.��ȡ����IP
    * 1.2.��ȡ����������Ϣ
    * 2.��������IP
    * 3.����ǰ��������ΪĬ��ֵ
    * 4.����ID
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
    // ��ȡ��һ����������IPv4��ַ
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
    * 1.���ȷ���ж������Ƿ���Ч
    * 2.��Ч�򴴽�������ʼ��
    * 3.�����Ҫ���汾���ļ�
    * 4.��ת���浽�ȴ�����
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

    /* ��ʼ����������  w00001 �������ip��ַ�ӿ� */
    QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_WAITE, QObject::tr("Initialise SDK...")));
    otsControler::ControlerData::instance()->initialize();
    QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_WAITE, QObject::tr("Initialise SDK  complete...")));
}

void ConfigIPIDWidget::slotCancelClicked()
{
    /**
    * 1.ȡ��֮��رս���
    */

    QCoreApplication::exit();
}
