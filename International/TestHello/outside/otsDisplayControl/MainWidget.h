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

extern int _operationFrameIndex; //ȫ��֡����

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

        /** ���ý���Ϊ���紫������� */
        void setScreenLayoutWidget_net(int moduleType, std::vector<std::tuple<std::string, int, int, int, int, int>> &vecLayoutInfo);
        /** ��ȡ��ǰ��������ID */
        int getCurrentScreenID();
        /** ����Ԥ��ģ������ */
        void updateMouldItem(const int mouldid);

protected:
		void changeEvent(QEvent * event);


	signals:
		//Ͷ����֤֡����
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

		/* ��Ѳ���ݴ洢 */
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

        /** ������Ѳ���� */
        void processPollData(PollData& pollData);

	private:
		SignalGroupTreeWidget* _pSignalGroupTreeWidget;/**< �ź�Դ���� */
		SignalPreviewWidget* _pSignalPreviewWidget;/**< �ź�ԴԤ������ */
		ScreenSwitchWidget* _pScreenSwitchWidget;/**< ���д������ݲ˵� */
		ScreenWidget* _pScreenWidget;/**< ������������ */
		LayoutModelWidget* _pLayoutModelWidget;/**< ģ����ʽ�˵� */
		ScreenModelWidget* _pScreenModelWidget;/**< ����Ԥ���˵� */
		AddModelWidget* _pAddModelWidget;/**< ���Ԥ����Ϣ�� */
		PollWidget* _pPollWidget;/**< ��Ѳͨ������ */
		AddMirrorWidget *_pAddMirrorWidget; /* <��Ӿ���ͨ������>*/
		MirrorWidget* _pMirrorWidget;/**< ��������ͨ�� */
		AddPollWidget* _pAddPollWidget;/**< �����Ѳ��������Ϣ�� */
		QVector<PollData> _vecPollDatas;/**< ������Ѳ�������� */
		QMap<int, PollData> _mapPollID2PollData;/**< ������Ѳ�������ݣ�����ID������ID */
		QMap<QTimer*, PollData> _mapTimer2PollData;/**< ������Ѳ�������ݣ�����ID������ID */
		//int _operationFrameIndex = 0;
		int _undoButtonClickCnt = 0;  //������ť�������
		int _redoButtonClickCnt = 0; //������ť������� (����л��˴�������Ҫ���ø�ֵ)

		QPushButton* _pClearBT = nullptr; //������ť
		QPushButton* _pUndoBT = nullptr; //������ť
		QPushButton* _pRedoBT = nullptr; //������ť

		QPushButton* _pUploadBT; /**< �ϴ�ͼƬ */

		QPushButton* _pMiniBT;/**< ��С����ť */
		QPushButton* _pCloseBT;/**< �رհ�ť */

		QLabel* _pTimeLabel;/**< ʱ�� */
		QLabel* _pLogoLabel;/**< Logo */
	};
}