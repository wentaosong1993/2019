#pragma comment(lib, "legacy_stdio_definitions.lib") 

#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include <otsServer/DataSession.h>
#include <otsControler/ControlerData.h>
#include "ots/Logger.h"
#include "ots/PathRegistry.h"
#include "MainWidget.h"
#include <QFile>
#include "otsXML/XML.h"
#include "otsXML/globalVar.h"

namespace otsManagerMode
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
		QApplication a(argc, argv);

		QFile file("./resource/qss/QSS.css");
		file.open(QIODevice::ReadOnly);

		QString strQSS = file.readAll();
		file.close();
		qApp->setStyleSheet(strQSS);
		qInfo() << "QSS：" << strQSS;

        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

        /* 设置资源路径 */
        std::string strCurPath = QApplication::applicationDirPath().toStdString();
        FILE_PATH->setDataPath(strCurPath + "/resource");

		qInfo() << "strCurPath:" << QString::fromStdString(strCurPath);

		/** 读取配置文件 */
		//ParseXML xml;
		//std::string filePath = FILE_PATH->getDataPath();
		//filePath += "/../xml/config-manager.xml";
		//xml.openXMLFile(filePath.c_str());
		//ots::g_strLocalHostIP = g_localHostIP.toStdString();

		//qInfo() << "IP addr:" << QString::fromStdString(ots::g_strLocalHostIP);

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

		otsManagerWidget::MainWidget g;
        //g.show();
		g.showFullScreen();
		a.exec();

		return 0;
	}
}
