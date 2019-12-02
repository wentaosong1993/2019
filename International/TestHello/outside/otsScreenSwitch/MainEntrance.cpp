#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include "ots/Logger.h"
#include "ots/PathRegistry.h"
#include "otsServer/DataSession.h"
#include "screenswitchwidget.h"

namespace otsScreenSwitch
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

        QString qssString;
        qssString += "ScreenSwitchWidget::item{width:120px;margin:3px;}";
        qApp->setStyleSheet(qssString);

        ScreenSwitchWidget w;
        w.createTestData();
        w.show();
        w.createData();

		a.exec();

		return 0;
	}
}
