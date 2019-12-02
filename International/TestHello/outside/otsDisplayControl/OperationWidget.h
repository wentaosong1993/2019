#pragma once

#include <QDialog>
#include <QListWidgetItem>
#include <QListWidget>

namespace Ui {
    class AddModelWidget;
    class AddPollWidget;
	class AddMirrorWidget;
    class PreviewDialog;
}

class AddModelWidget :public QDialog
{
    Q_OBJECT
public:
    explicit AddModelWidget(QWidget *parent = 0);
    ~AddModelWidget();

    QString getModelName();
	void clearContent();

protected:
	void changeEvent(QEvent *event);

private slots:
	void slotOk();
	void slotCancel();
private:
    Ui::AddModelWidget* _ui;
};

class AddPollWidget :public QDialog
{
    Q_OBJECT
public:
    explicit AddPollWidget(QWidget *parent = 0);
    ~AddPollWidget();

    QString getPollName();
    std::vector<int> getPollModels();
    int getTimes();
    void creatSrcList(int screenID);

protected:
	void changeEvent(QEvent *event);

private slots:
    void slotDelItem();
    void slotOK();
    void slotCancel();

private:
    Ui::AddPollWidget* _ui;
};

class AddMirrorWidget :public QDialog
{
    Q_OBJECT
public:
    explicit AddMirrorWidget(QWidget *parent = 0);
    ~AddMirrorWidget();

    QString getMirrorName();
    void creatSrcList(int screenID);
	std::vector<int> getMirrorData();

protected:
	void changeEvent(QEvent *event);

signals:
	void signalsMirrorChannelID(int screenId);

private slots:
	void slotOk();
	void slotCancel();

private:
    Ui::AddMirrorWidget* _ui;
	QString _mirrorName; /* <¾µÏñÍ¨µÀÃû³Æ>*/
};

class PreviewDialog :public QDialog
{
    Q_OBJECT
public:
    explicit PreviewDialog(QWidget *parent = 0);
    ~PreviewDialog();

    void setModel(int modelID);
    void setScreen(int screenID);

protected:
	void changeEvent(QEvent *event);
private:
    Ui::PreviewDialog* ui;
};