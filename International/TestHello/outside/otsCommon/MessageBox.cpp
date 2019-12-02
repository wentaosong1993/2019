#include "MessageBox.h"
#include "ui_MessageBox.h"

using namespace otsCommon;
SCMessageBox::SCMessageBox(QString strTitle, QString strInfo, MessageBoxType type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint|Qt::FramelessWindowHint);
	//this->setWindowModality(Qt::WindowModal);
    ui->setupUi(this);
    ui->titleLabel->setText(strTitle);
    ui->centerLabel->setText(strInfo);

    switch (type) {
    case Warning:
    {
		ui->icoLabel->setObjectName("icoLabel-warning");
        /*ui->cancelPushButton->hide();*/
		break;
    }
	case Information:
	{
		ui->icoLabel->setObjectName("icoLabel-infor");
		ui->cancelPushButton->hide();
	}
        break;
    default:
        break;
    }
    //this->setProperty("type",type);
    this->style()->unpolish(this);
    this->style()->polish(this);
}

SCMessageBox::~SCMessageBox()
{
    delete ui;
}

void SCMessageBox::on_okPushButton_clicked()
{
    this->accept();
}

void SCMessageBox::on_cancelPushButton_clicked()
{
    this->reject();
}
