#pragma once

#include <QWidget>

class SignalGroupManageWidget;
class AddAccountWidget;
class ScreenManageWidget;

class QLabel;
class QPushButton;

namespace otsManagerWidget
{
	class MainWidget :public QWidget
	{
		Q_OBJECT
	public:
		explicit MainWidget(QWidget *parent = 0);

        void createUIData();

	private:
		void createUI();

	protected:
		void paintEvent(QPaintEvent *event);
		void timerEvent(QTimerEvent *event);
		void changeEvent(QEvent* event);

	private:
		QPushButton* _pMiniBT;/**< ��С����ť */
		QPushButton* _pCloseBT;/**< �رհ�ť */

		QLabel* _pTimeLabel;/**< ʱ�� */
		QLabel* _pLogoLabel;/**< Logo */
        SignalGroupManageWidget* _pSignalGroupTree = nullptr; /**< �ź�Դ���� */
		AddAccountWidget * _pAddAccount = nullptr; /**< �����༭��Ա�ؼ� */
		ScreenManageWidget * _pScreenManageWidget = nullptr; /*<¼���������>*/
	};
}
