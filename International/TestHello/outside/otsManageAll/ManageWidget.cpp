#include "ManageWidget.h"
#include "ui_ManageWidget.h"
#include "ots/GUIEvent.h"
#include "ots/Logger.h"
#include "otsServer/DataSession.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "otsCommon/MessageBox.h"
#include "otsControler/net_globle_data.h"

using namespace rapidjson;
/**
 * @brief ManageWidget::ManageWidget
 * @param parent
 * 1.UI包括 等待界面 登录界面 操作界面 管理界面
 */

ManageWidget::ManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManageWidget)
{
    ui->setupUi(this);

    /* 初始化配置界面 */
    ui->stackedWidget->setCurrentWidget(ui->Config_page);

    /* 测试 设置等待界面显示 'Hello World!' */
    ui->Start_InitData_page->setDiaplayText(tr("Hello \r\n World!"));
}

ManageWidget::~ManageWidget()
{
    delete ui;
}

void ManageWidget::changeEvent(QEvent *event)
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

bool ManageWidget::event(QEvent *event)
{
    if(event->type() == GUIEvent::GUIType)
    {
        GUIChanged((GUIEvent*)event);
        return true;
    }
    return QWidget::event(event);
}

void ManageWidget::GUIChanged(GUIEvent *event)
{
    if (!this->isVisible())
    {
        this->setVisible(true);
    }

    switch (event->ProcessType) {
    case GUIEvent::SHOW_WAITE:
    case GUIEvent::SHOW_SYN_DB:
    {
        if (ui->stackedWidget->currentWidget() != ui->Start_InitData_page)
        {
            ui->stackedWidget->setCurrentWidget(ui->Start_InitData_page);
        }
        ui->Start_InitData_page->setDiaplayText(event->processInfo);
    }
        break;
    case GUIEvent::SHOW_LOGIN:
    case GUIEvent::SHOW_SYN_DB_FINISH:
    case GUIEvent::SHOW_SELF_MASTER:
    {
        /* 初始化数据库  初始化 */
        otsServer::DataSession::instance()->initialize();
        g_get_db_flag.set_value(1);
        /* 初始化操作界面数据 */
        ui->MainWidget_page->initUIData();
        /* 显示当前界面为登录界面 */
        if (ui->stackedWidget->currentWidget() != ui->Login_page)
        {
            ui->stackedWidget->setCurrentWidget(ui->Login_page);
        }
    }
        break;
    case GUIEvent::SHOW_MOULD_CHANGE:
    {
        OTS_LOG->debug("mould change info:{}", event->processInfo.toStdString());
        Document document;    //生成一个dom元素Document

        int screenid;
        std::tuple<int, std::string> mouldInfo;
        std::vector<std::tuple<std::string, int, int, int, int, int>> layoutInfo;

        if (!document.Parse((const char*)event->processInfo.toStdString().c_str()).HasParseError())
        {
            screenid = document["screenid"].GetInt();
            int type = document["type"].GetInt();
            const char*  name = document["name"].GetString();

            if (ui->MainWidget_page->getCurrentScreenID() == screenid)
            {
                mouldInfo = std::make_tuple(type, std::string(name));

                Value& infoArray = document["layout_arry"];
                if (infoArray.IsArray())
                {
                    for (int i = 0; i < infoArray.Size(); i++)
                    {
                        const Value& object = infoArray[i];

                        std::string name = object["name"].GetString();
                        int left = object["left"].GetInt();
                        int top = object["top"].GetInt();
                        int right = object["right"].GetInt();
                        int bottom = object["bottom"].GetInt();
                        int encoderID = object["encoderID"].GetInt();

                        layoutInfo.push_back(std::make_tuple(name, left, top, right, bottom, encoderID));
                    }
                }
                ui->MainWidget_page->setScreenLayoutWidget_net(std::get<0>(mouldInfo), layoutInfo);
            }
        }
    }
        break;
    case GUIEvent::SHOW_MOULD_DEL:
    case GUIEvent::SHOW_MOULD_SAVE:
    {
        OTS_LOG->debug("mould del info:{}", event->processInfo.toStdString());
        Document document;    //生成一个dom元素Document
        if (!document.Parse((const char*)event->processInfo.toStdString().c_str()).HasParseError())
        {
            int screenid = document["screenid"].GetInt();
            int mouldid = document["mouldid"].GetInt();

            if (ui->MainWidget_page->getCurrentScreenID() == screenid)
            {
                ui->MainWidget_page->updateMouldItem(mouldid);
            }
        }
    }
        break;
    case GUIEvent::SHOW_MOULD_SAVE_FAILED:
    {
        /* 如果当前界面不是操作界面，则不进行弹窗 */
        if (ui->stackedWidget->currentWidget() != ui->MainWidget_page)
        {
            return;
        }
        OTS_LOG->debug("save mould failed info:{}", event->processInfo.toStdString());
        Document document;    //生成一个dom元素Document
        if (!document.Parse((const char*)event->processInfo.toStdString().c_str()).HasParseError())
        {
            int screenid = document["screenid"].GetInt();
            int mouldid = document["mouldid"].GetInt();

            if (ui->MainWidget_page->getCurrentScreenID() == screenid)
            {
                std::string mouldname = document["mouldname"].GetString();
                std::string ret_msg = document["ret_msg"].GetString();
                QString str = tr("Module:") + QString::fromStdString(mouldname) + "\r\n" + QString::fromStdString(ret_msg);
                otsCommon::SCMessageBox* activeModuleDlg = new otsCommon::SCMessageBox(QString::fromStdString("Add Module Failed"), str, otsCommon::SCMessageBox::Information, this);
                activeModuleDlg->exec();
                activeModuleDlg->deleteLater();
                activeModuleDlg = nullptr;
            }
        }
    }
        break;
    case GUIEvent::SHOW_LOGIN_RES:
    {
        OTS_LOG->debug("login info:{}", event->processInfo.toStdString());

        Document document;    //生成一个dom元素Document
        if (!document.Parse((const char*)event->processInfo.toStdString().c_str()).HasParseError())
        {
            int ret_code = document["ret_code"].GetInt();
            std::string ret_msg = document["ret_msg"].GetString();

            if (ret_code == 1)
            {
                int user_id = document["userid"].GetInt();
                int user_type = document["type"].GetInt();
                if (user_type == 1)/* 管理员 */
                {
                    ui->ManageWidget_page->createUIData();
                    if (ui->stackedWidget->currentWidget() != ui->ManageWidget_page)
                    {
                        ui->stackedWidget->setCurrentWidget(ui->ManageWidget_page);
                    }
                }
                else/* 普通用户 */
                {
                    if (ui->stackedWidget->currentWidget() != ui->MainWidget_page)
                    {
                        ui->stackedWidget->setCurrentWidget(ui->MainWidget_page);
                    }
                }
            }
            else
            {
                /* 登录界面提示登录失败原因 */
                ui->Login_page->setErrorTip(QString::fromStdString(ret_msg));
            }
        }
    }
        break;
    default:
        OTS_LOG->debug("type:{}", event->ProcessType);
        break;
    }
}

