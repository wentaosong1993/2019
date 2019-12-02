#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include "ots/Logger.h"
#include "SignalGroupTreeWidget.h"
#include "ots/Logger.h"
#include "ots/PathRegistry.h"
#include "otsServer/DataSession.h"
#include "otsControler/ControlerData.h"

namespace otsSignalTree
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
		QApplication a(argc, argv);

        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

        /* 设置资源路径 */
        std::string strCurPath = QApplication::applicationDirPath().toStdString();
        FILE_PATH->setDataPath(strCurPath + "/resource");

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

        SignalGroupTreeWidget signalGroupTree;
        signalGroupTree.show();

		a.exec();

		return 0;
	}
}
