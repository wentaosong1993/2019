#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include "ots/Logger.h"
#include "ots/PathRegistry.h"
#include "PollWidget.h"

namespace otsPollWidget
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
		QApplication a(argc, argv);

        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

        /* 设置资源路径 */
        std::string strCurPath = QApplication::applicationDirPath().toStdString();
        FILE_PATH->setDataPath(strCurPath + "/resource");

        QString qssString;
        //qssString += "PollWidget::item{width:120px;margin:3px;}";
        qApp->setStyleSheet(qssString);

        PollWidget w;
        w.createTestData();
        w.show();
        w.addItem(100, "John_Yang");
        //w.createData();

		a.exec();

		return 0;
	}
}
