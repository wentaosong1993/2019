#include "MainEntrance.h"
#include <QApplication>
#include <QDebug>
#include "ots/Logger.h"
#include "ots/PathRegistry.h"
#include "otsServer/DataSession.h"
#include "LayoutModelWidget.h"
#include "otsCommon/MessageBox.h"

namespace otsLayoutModel
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
        qssString += "QListWidget#CommonModelList::item{width:120px;margin:3px;}";
        qssString += "otsCommon--SCMessageBox QLabel#titleLabel{max-height:30px;background-color: rgba(13,34,100,1);font-size:14px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}";
        qssString += "otsCommon--SCMessageBox QFrame#line{background-color: rgba(0,84,162,1);}";
        qssString += "otsCommon--SCMessageBox{background:rgba(13,34,61,1);border:2px solid rgba(0,84,162,1);}";
        qssString += "otsCommon--SCMessageBox QPushButton{width:108px;\
                                                height:40px;\
                                                                                          font-size:16px;\
                                                                                                                                                font-family:Microsoft YaHei;\
                                                                                                                                                                                                                  font-weight:400;\
                                                                                                                                                                                                                                                                                                color:rgba(255,255,255,1);border-image:url(\":/round-button-normal.png\")}";
        qssString += "otsCommon--SCMessageBox QPushButton:hover{border-image:url(\":/round-button-pressed.png\")}";
        qssString += "otsCommon--SCMessageBox QLabel#centerLabel{font-size:14px;font-family:Microsoft YaHei;color:rgba(255,255,255,1);}";
        qssString += "otsCommon--SCMessageBox QLabel#icoLabel{min-width:50px;min-height:50px;max-width:50px;max-height:50px;background-color:red;}";
        qApp->setStyleSheet(qssString);

        otsCommon::SCMessageBox e("warring", "arararar", otsCommon::SCMessageBox::Information);
        e.exec();

        LayoutModelWidget w;
        w.show();


		a.exec();

		return 0;
	}
}
