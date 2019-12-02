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
		QPushButton* _pMiniBT;/**< 最小化按钮 */
		QPushButton* _pCloseBT;/**< 关闭按钮 */

		QLabel* _pTimeLabel;/**< 时间 */
		QLabel* _pLogoLabel;/**< Logo */
        SignalGroupManageWidget* _pSignalGroupTree = nullptr; /**< 信号源组树 */
		AddAccountWidget * _pAddAccount = nullptr; /**< 创建编辑人员控件 */
		ScreenManageWidget * _pScreenManageWidget = nullptr; /*<录入大屏数据>*/
	};
}
