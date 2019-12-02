#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include <otsServer/DataSession.h>
#include <otsControler/ControlerData.h>
#include "ots/Logger.h"
#include "ots/PathRegistry.h"

#include "ManageWidget.h"
#include "ots/GUIEvent.h"
#include "otsXML/XML.h"
#include <QFile>

void initOtsManageAllResource() { Q_INIT_RESOURCE(otsManageAll); }

namespace otsManageAll
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
		//std::string ids; //多开唯一性
		//FILE *fp = fopen("ip.txt", "rb");
		//if (fp)
		//{
		//	char buff[32] = { 0 };
		//	fread(buff, 1, 32, fp);
		//	fclose(fp);
		//	ids = std::string(buff);
		//}

		////ots::g_strLocalHostIP = "192.168.45.221";
		//ots::g_strLocalHostIP = ids;
		QApplication a(argc, argv);
        initOtsManageAllResource();

        /**
        * 1.初始化日志库
        * 2.设置资源路径
        * 3.创建界面，启动界面，初始界面为配置界面
        * 4.当确认配置之后，启动网动SDK，界面跳转到等待处理界面
        * 5.当选举成功之后，通知等待界面更新消息
        * 6.如果需要更新数据库，通知等待界面更新消息，开始同步数据库
        * 7.当数据库同步完成后，切换界面到登录界面
        * 8.登录之后跳转到操作界面（管理界面）
        * 9.
        */

        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

        /* 设置资源路径 */
        std::string strCurPath = QApplication::applicationDirPath().toStdString();

        FILE_PATH->setDataPath(strCurPath + "/resource");
		qInfo() << "strCurPath:" << QString::fromStdString(strCurPath);

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

        /* 初始化数据库  初始化 */
        //if (!otsServer::DataSession::instance()->initialize())
        //{
        //    return 0;
        //}

        ManageWidget w;
        g_ManageWidget = &w;
        w.show();

		/* 初始化大屏控制  w00001 添加设置ip地址接口 */
        //QCoreApplication::postEvent(g_ManageWidget, new GUIEvent(GUIEvent::SHOW_WAITE, "初始SDK..."));
		//if (!otsControler::ControlerData::instance()->initialize())
		//{
		//	return 0;
		//}
	


        //OTS_LOG->debug("DB SDK Resource initialize .......");

		a.exec();

		return 0;
	}
}
