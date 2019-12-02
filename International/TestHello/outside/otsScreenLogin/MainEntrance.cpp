﻿#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <otsServer/DataSession.h>
#include <otsControler/ControlerData.h>
#include "ots/Logger.h"
#include "otsXML/XML.h"
#include "ots/PathRegistry.h"
#include "MainEntrance.h"
#include "otsDisplayControl/MainWidget.h"
#include "otsManagerMode/MainWidget.h"
#include "ScreenLoginWidget.h"
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QDialog>
#include "globalApi.h"

void initOtsScreenLoginResource() { Q_INIT_RESOURCE(otsScreenLogin); }
namespace otsScreenLogin
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
		QApplication a(argc, argv);

		initOtsScreenLoginResource();

        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

		/* 设置资源路径 */
		std::string strCurPath = QApplication::applicationDirPath().toStdString();

		FILE_PATH->setDataPath(strCurPath + "/resource");
		//qInfo() << "strCurPath:" << QString::fromStdString(strCurPath);

		std::string filePath = FILE_PATH->getDataPath();
		QString qssPath = QString::fromStdString(filePath) + "/qss/QSS.css";
		qInfo() << "qssPath:" << qssPath;

		QFile file(qssPath);
		file.open(QIODevice::ReadOnly);
		QString strQSS = file.readAll();
		file.close();

		qApp->setStyleSheet(strQSS);

		/** 读取配置文件 */
		ParseXML xml;

		filePath += "/../xml/config.xml";
		xml.openXMLFile(filePath.c_str());

		qInfo() << "xml路径:" << QString::fromStdString(filePath);

        /* 初始化数据库  初始化 */
        if (!otsServer::DataSession::instance()->initialize())
        {
            return 0;
        }

		/* 初始化大屏控制  w00001 添加设置ip地址接口 */
		if (!otsControler::ControlerData::instance()->initialize())
		{
			return 0;
		}

        OTS_LOG->debug("DB SDK Resource initialize .......");

		QWidget *mainObj = nullptr;
		ScreenLoginWidget login;
		//int ret = login.exec();
		//qInfo() << QStringLiteral("状态：") << ret;
		//if (ret == QDialog::Accepted)
		//{
		//	if (2 == _userType)
		//	{
		//		//普通类型账户
		//		mainObj = new otsDisplayControl::MainWidget;
		//		mainObj->showFullScreen();
		//	}
		//	else if (1 == _userType)
		//	{
		//		//管理员类型账户
		//		mainObj = new otsManagerWidget::MainWidget;
		//		//mainObj->show();
		//		mainObj->showFullScreen();
		//	}
		//	if (!login.getUserInfo().isEmpty())
		//	{
		//		__userAccountName = login.getUserInfo();
		//		__userType = login.getUserType();
		//	}
		//	a.exec();
		//}
		//else
		//{
		//	qInfo() << "Rejected!!!!!!";
		//}

		if (mainObj)
		{
			mainObj->deleteLater();
			mainObj = nullptr;
		}

		return 0;
	}
}
