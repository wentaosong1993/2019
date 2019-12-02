#pragma once

#include <QWidget>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QTimer>
#include <QPushButton>

class SignalPreviewWidget;
class SignalGroupTreeWidget;
class ScreenSwitchWidget;
class ScreenWidget;
class LayoutModelWidget;
class ScreenModelWidget;
class AddModelWidget;
class PollWidget;
class MirrorWidget;
class AddPollWidget;
class AddMirrorWidget;
class QLabel;

extern int _operationFrameIndex; //全局帧索引

namespace otsDisplayControl
{
	class MainWidget :public QWidget
	{
		Q_OBJECT
	public:
		explicit MainWidget(QWidget *parent = 0);
		virtual ~MainWidget();

	public:
		int getNumforT(int num, int T);

        void initUIData();

        /** 设置界面为网络传输的数据 */
        void setScreenLayoutWidget_net(int moduleType, std::vector<std::tuple<std::string, int, int, int, int, int>> &vecLayoutInfo);
        /** 获取当前操作大屏ID */
        int getCurrentScreenID();
        /** 更新预案模块数据 */
        void updateMouldItem(const int mouldid);

protected:
		void changeEvent(QEvent * event);


	signals:
		//投屏后保证帧数据
		void saveFrameIndex();
	private:
		void createUI();

		private slots:
		void slotSaveModel();
        void slotDelModel(int modelID);
		void slotPollStartStop(int groupID, bool start);
		void slotAddPoll();
		void slotDelPoll();
		void slotTimerOut();
		void slotUndo();
		void slotRedo();
		void slotUpLoadImage();
		void slotAddMirror();
		void slotDelMirror();
		//void slotMirrorStartStop();
		void slotApplyMirror(bool state, int mirrorID);

        void slotModelChanged(int modelID);
		//void slotLoggout();
	protected:
		void paintEvent(QPaintEvent *event);
		void timerEvent(QTimerEvent *event);
        void showEvent(QShowEvent *event);

		/* 轮巡数据存储 */
	private:
		class PollData
		{
		public:
			PollData() : _screenID(0), _time(0), _currnetIndex(0), _timer(NULL){}
			int _screenID;
			int _time;
			int _currnetIndex;
			QVector<int> _vecModels;
			QTimer* _timer;
		};

        /** 处理轮巡数据 */
        void processPollData(PollData& pollData);

	private:
		SignalGroupTreeWidget* _pSignalGroupTreeWidget;/**< 信号源组树 */
		SignalPreviewWidget* _pSignalPreviewWidget;/**< 信号源预览窗口 */
		ScreenSwitchWidget* _pScreenSwitchWidget;/**< 所有大屏数据菜单 */
		ScreenWidget* _pScreenWidget;/**< 大屏操作界面 */
		LayoutModelWidget* _pLayoutModelWidget;/**< 模板样式菜单 */
		ScreenModelWidget* _pScreenModelWidget;/**< 大屏预案菜单 */
		AddModelWidget* _pAddModelWidget;/**< 添加预案消息框 */
		PollWidget* _pPollWidget;/**< 轮巡通道窗口 */
		AddMirrorWidget *_pAddMirrorWidget; /* <添加镜像通道弹框>*/
		MirrorWidget* _pMirrorWidget;/**< 创建镜像通道 */
		AddPollWidget* _pAddPollWidget;/**< 添加轮巡方案的消息框 */
		QVector<PollData> _vecPollDatas;/**< 保存轮巡方案数据 */
		QMap<int, PollData> _mapPollID2PollData;/**< 保存轮巡方案数据：方案ID跟大屏ID */
		QMap<QTimer*, PollData> _mapTimer2PollData;/**< 保存轮巡方案数据：方案ID跟大屏ID */
		//int _operationFrameIndex = 0;
		int _undoButtonClickCnt = 0;  //撤销按钮点击次数
		int _redoButtonClickCnt = 0; //重做按钮点击次数 (如果切换了大屏就需要重置该值)

		QPushButton* _pClearBT = nullptr; //清屏按钮
		QPushButton* _pUndoBT = nullptr; //撤销按钮
		QPushButton* _pRedoBT = nullptr; //重做按钮

		QPushButton* _pUploadBT; /**< 上传图片 */

		QPushButton* _pMiniBT;/**< 最小化按钮 */
		QPushButton* _pCloseBT;/**< 关闭按钮 */

		QLabel* _pTimeLabel;/**< 时间 */
		QLabel* _pLogoLabel;/**< Logo */
	};
}