#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>

namespace Ui {
class MessageBox;
}

namespace otsCommon
{
    class SCMessageBox : public QDialog
    {
        Q_OBJECT

    public:
        enum MessageBoxType
        {
            Warning = 0,
            Information
        };

        explicit SCMessageBox(QString strTitle, QString strInfo, MessageBoxType type = Information, QWidget *parent = 0);
        ~SCMessageBox();

        private slots:
        void on_okPushButton_clicked();

        void on_cancelPushButton_clicked();

    private:
        Ui::MessageBox *ui;
    };
}

#endif // MESSAGEBOX_H
