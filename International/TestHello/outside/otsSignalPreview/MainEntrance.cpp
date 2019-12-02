#include "MainEntrance.h"
#include <QApplication>
#include "signalpreviewwidget.h"
#include "otsControler/ControlerData.h"
#include "ots/Logger.h"

namespace otsSignalPreview
{
	int mainEntrance(int argc, char * argv[], bool isLocalResource)
	{
        ots::g_strLocalHostIP = "192.168.40.135";
        /** 初始化日志 */
        ots::Logger::instance()->initLog("logControl.log");

		QApplication a(argc, argv);

        // 初始化大屏控制  w00001 添加设置ip地址接口
        if (!otsControler::ControlerData::instance()->initialize())
        {
            return 0;
        }

        QString qssString;
        qssString += "SignalPreviewWidget::item{width:320px;height:90px;margin:3px;}";
        qssString += "PreviewWidget{background-color:red;}";
        qApp->setStyleSheet(qssString);

        SignalPreviewWidget signalGroupTree;
        signalGroupTree.createTestData();
        signalGroupTree.show();
        std::vector<std::tuple<int, int, std::string>> vecInfo;

        //vecInfo.push_back(std::make_tuple(37, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(37);

        //vecInfo.push_back(std::make_tuple(39, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(39);

        vecInfo.push_back(std::make_tuple(40, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(40);

        vecInfo.push_back(std::make_tuple(41, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(41);

        vecInfo.push_back(std::make_tuple(42, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(42);

        vecInfo.push_back(std::make_tuple(43, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(43);

        vecInfo.push_back(std::make_tuple(44, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(44);

        vecInfo.push_back(std::make_tuple(45, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(45);

        vecInfo.push_back(std::make_tuple(46, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(46);

        vecInfo.push_back(std::make_tuple(47, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(47);

        vecInfo.push_back(std::make_tuple(48, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(48);

        vecInfo.push_back(std::make_tuple(49, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(49);

        vecInfo.push_back(std::make_tuple(50, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(50);

        vecInfo.push_back(std::make_tuple(51, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(51);

        vecInfo.push_back(std::make_tuple(52, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(52);

        vecInfo.push_back(std::make_tuple(53, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(53);

        vecInfo.push_back(std::make_tuple(54, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(54);

        vecInfo.push_back(std::make_tuple(55, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(55);

        vecInfo.push_back(std::make_tuple(56, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(56);

        vecInfo.push_back(std::make_tuple(57, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(57);

        vecInfo.push_back(std::make_tuple(58, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(58);

        vecInfo.push_back(std::make_tuple(59, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(59);

        vecInfo.push_back(std::make_tuple(60, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(60);

        //
        //vecInfo.push_back(std::make_tuple(64, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(64);
        //


        vecInfo.push_back(std::make_tuple(66, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(66);

        vecInfo.push_back(std::make_tuple(69, 0, ""));
        otsControler::ControlerData::instance()->startPlayVideo(69);

        //vecInfo.push_back(std::make_tuple(70, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(70);

        //vecInfo.push_back(std::make_tuple(71, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(71);

        //vecInfo.push_back(std::make_tuple(72, 0, ""));
        //otsControler::ControlerData::instance()->startPlayVideo(72);

        //signalGroupTree.createListData(vecInfo);

		a.exec();

		return 0;
	}
}
