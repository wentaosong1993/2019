/********************************************************************************
** Form generated from reading UI file 'ScreenLoginWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCREENLOGINWIDGET_H
#define UI_SCREENLOGINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScreenLoginWidget
{
public:
    QGridLayout *gridLayout;
    QFrame *LoginFrame;
    QGridLayout *gridLayout_19;
    QFrame *menuFrame;
    QHBoxLayout *horizontalLayout_71;
    QHBoxLayout *horizontalLayout_72;
    QSpacerItem *horizontalSpacer_21;
    QPushButton *loginMiniButton;
    QPushButton *loginCloseButtton;
    QFrame *switchFrame;
    QVBoxLayout *verticalLayout;
    QLabel *labelError_1;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_3;
    QFrame *commonUserFrame;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *userNameFrame;
    QLabel *userNameLabel;
    QComboBox *commonCombobox;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_9;
    QFrame *commonKeyFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *userKeyFrame;
    QLabel *userKeyLabel;
    QLineEdit *userKeyLineEdit;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_63;
    QCheckBox *rememberKeyCheckBox;
    QCheckBox *autoLoginCheckBox;
    QSpacerItem *horizontalSpacer_20;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *commonUserLoginButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_57;
    QLabel *loginTitleLabel;

    void setupUi(QWidget *ScreenLoginWidget)
    {
        if (ScreenLoginWidget->objectName().isEmpty())
            ScreenLoginWidget->setObjectName(QString::fromUtf8("ScreenLoginWidget"));
        ScreenLoginWidget->resize(1920, 1080);
        ScreenLoginWidget->setMinimumSize(QSize(1920, 1080));
        ScreenLoginWidget->setMaximumSize(QSize(1920, 1080));
        ScreenLoginWidget->setWindowOpacity(1.000000000000000);
        ScreenLoginWidget->setStyleSheet(QString::fromUtf8(""));
        gridLayout = new QGridLayout(ScreenLoginWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        LoginFrame = new QFrame(ScreenLoginWidget);
        LoginFrame->setObjectName(QString::fromUtf8("LoginFrame"));
        LoginFrame->setMinimumSize(QSize(524, 535));
        LoginFrame->setMaximumSize(QSize(524, 535));
        LoginFrame->setFrameShape(QFrame::StyledPanel);
        LoginFrame->setFrameShadow(QFrame::Raised);
        gridLayout_19 = new QGridLayout(LoginFrame);
        gridLayout_19->setSpacing(0);
        gridLayout_19->setContentsMargins(11, 11, 11, 11);
        gridLayout_19->setObjectName(QString::fromUtf8("gridLayout_19"));
        gridLayout_19->setContentsMargins(0, 0, 0, 0);
        menuFrame = new QFrame(LoginFrame);
        menuFrame->setObjectName(QString::fromUtf8("menuFrame"));
        menuFrame->setMinimumSize(QSize(524, 35));
        menuFrame->setMaximumSize(QSize(524, 35));
        menuFrame->setFrameShape(QFrame::StyledPanel);
        menuFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_71 = new QHBoxLayout(menuFrame);
        horizontalLayout_71->setSpacing(4);
        horizontalLayout_71->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_71->setObjectName(QString::fromUtf8("horizontalLayout_71"));
        horizontalLayout_71->setContentsMargins(2, 5, 2, 0);
        horizontalLayout_72 = new QHBoxLayout();
        horizontalLayout_72->setSpacing(6);
        horizontalLayout_72->setObjectName(QString::fromUtf8("horizontalLayout_72"));
        horizontalSpacer_21 = new QSpacerItem(40, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_72->addItem(horizontalSpacer_21);

        loginMiniButton = new QPushButton(menuFrame);
        loginMiniButton->setObjectName(QString::fromUtf8("loginMiniButton"));
        loginMiniButton->setMinimumSize(QSize(26, 26));
        loginMiniButton->setMaximumSize(QSize(26, 26));

        horizontalLayout_72->addWidget(loginMiniButton);

        loginCloseButtton = new QPushButton(menuFrame);
        loginCloseButtton->setObjectName(QString::fromUtf8("loginCloseButtton"));
        loginCloseButtton->setMinimumSize(QSize(26, 26));
        loginCloseButtton->setMaximumSize(QSize(26, 26));

        horizontalLayout_72->addWidget(loginCloseButtton);


        horizontalLayout_71->addLayout(horizontalLayout_72);


        gridLayout_19->addWidget(menuFrame, 0, 0, 1, 1);

        switchFrame = new QFrame(LoginFrame);
        switchFrame->setObjectName(QString::fromUtf8("switchFrame"));
        switchFrame->setFrameShape(QFrame::StyledPanel);
        switchFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(switchFrame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelError_1 = new QLabel(switchFrame);
        labelError_1->setObjectName(QString::fromUtf8("labelError_1"));
        labelError_1->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        verticalLayout->addWidget(labelError_1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_3);

        commonUserFrame = new QFrame(switchFrame);
        commonUserFrame->setObjectName(QString::fromUtf8("commonUserFrame"));
        commonUserFrame->setMinimumSize(QSize(376, 50));
        commonUserFrame->setMaximumSize(QSize(376, 50));
        commonUserFrame->setFrameShape(QFrame::StyledPanel);
        commonUserFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(commonUserFrame);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        userNameFrame = new QPushButton(commonUserFrame);
        userNameFrame->setObjectName(QString::fromUtf8("userNameFrame"));
        userNameFrame->setMinimumSize(QSize(26, 28));
        userNameFrame->setMaximumSize(QSize(26, 28));
        userNameFrame->setFlat(true);

        horizontalLayout_8->addWidget(userNameFrame);

        userNameLabel = new QLabel(commonUserFrame);
        userNameLabel->setObjectName(QString::fromUtf8("userNameLabel"));
        userNameLabel->setMinimumSize(QSize(54, 15));
        userNameLabel->setMaximumSize(QSize(54, 15));

        horizontalLayout_8->addWidget(userNameLabel);

        commonCombobox = new QComboBox(commonUserFrame);
        commonCombobox->setObjectName(QString::fromUtf8("commonCombobox"));
        commonCombobox->setMinimumSize(QSize(256, 45));
        commonCombobox->setMaximumSize(QSize(256, 45));
        commonCombobox->setStyleSheet(QString::fromUtf8(""));
        commonCombobox->setEditable(true);

        horizontalLayout_8->addWidget(commonCombobox);


        horizontalLayout_9->addWidget(commonUserFrame);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(-1, 10, -1, -1);
        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_9);

        commonKeyFrame = new QFrame(switchFrame);
        commonKeyFrame->setObjectName(QString::fromUtf8("commonKeyFrame"));
        commonKeyFrame->setMinimumSize(QSize(376, 50));
        commonKeyFrame->setMaximumSize(QSize(376, 50));
        commonKeyFrame->setFrameShape(QFrame::StyledPanel);
        commonKeyFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(commonKeyFrame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        userKeyFrame = new QPushButton(commonKeyFrame);
        userKeyFrame->setObjectName(QString::fromUtf8("userKeyFrame"));
        userKeyFrame->setMinimumSize(QSize(26, 28));
        userKeyFrame->setMaximumSize(QSize(26, 28));
        userKeyFrame->setFlat(true);

        horizontalLayout->addWidget(userKeyFrame);

        userKeyLabel = new QLabel(commonKeyFrame);
        userKeyLabel->setObjectName(QString::fromUtf8("userKeyLabel"));
        userKeyLabel->setMinimumSize(QSize(54, 15));
        userKeyLabel->setMaximumSize(QSize(54, 15));

        horizontalLayout->addWidget(userKeyLabel);

        userKeyLineEdit = new QLineEdit(commonKeyFrame);
        userKeyLineEdit->setObjectName(QString::fromUtf8("userKeyLineEdit"));
        userKeyLineEdit->setMinimumSize(QSize(256, 45));
        userKeyLineEdit->setMaximumSize(QSize(256, 45));
        userKeyLineEdit->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(userKeyLineEdit);


        horizontalLayout_10->addWidget(commonKeyFrame);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);


        verticalLayout->addLayout(horizontalLayout_10);

        horizontalLayout_63 = new QHBoxLayout();
        horizontalLayout_63->setSpacing(6);
        horizontalLayout_63->setObjectName(QString::fromUtf8("horizontalLayout_63"));
        horizontalLayout_63->setContentsMargins(65, 10, 74, -1);
        rememberKeyCheckBox = new QCheckBox(switchFrame);
        rememberKeyCheckBox->setObjectName(QString::fromUtf8("rememberKeyCheckBox"));
        rememberKeyCheckBox->setMinimumSize(QSize(98, 24));
        rememberKeyCheckBox->setMaximumSize(QSize(98, 24));
        rememberKeyCheckBox->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_63->addWidget(rememberKeyCheckBox);

        autoLoginCheckBox = new QCheckBox(switchFrame);
        autoLoginCheckBox->setObjectName(QString::fromUtf8("autoLoginCheckBox"));
        autoLoginCheckBox->setMinimumSize(QSize(98, 24));
        autoLoginCheckBox->setMaximumSize(QSize(98, 24));

        horizontalLayout_63->addWidget(autoLoginCheckBox);

        horizontalSpacer_20 = new QSpacerItem(88, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_63->addItem(horizontalSpacer_20);


        verticalLayout->addLayout(horizontalLayout_63);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 15, -1, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        commonUserLoginButton = new QPushButton(switchFrame);
        commonUserLoginButton->setObjectName(QString::fromUtf8("commonUserLoginButton"));
        commonUserLoginButton->setMinimumSize(QSize(376, 50));
        commonUserLoginButton->setMaximumSize(QSize(376, 50));

        horizontalLayout_3->addWidget(commonUserLoginButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);


        gridLayout_19->addWidget(switchFrame, 4, 0, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_19->addItem(verticalSpacer_4, 3, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 38, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_19->addItem(verticalSpacer_3, 1, 0, 1, 1);

        horizontalLayout_57 = new QHBoxLayout();
        horizontalLayout_57->setSpacing(6);
        horizontalLayout_57->setObjectName(QString::fromUtf8("horizontalLayout_57"));
        horizontalLayout_57->setContentsMargins(80, -1, 79, -1);
        loginTitleLabel = new QLabel(LoginFrame);
        loginTitleLabel->setObjectName(QString::fromUtf8("loginTitleLabel"));
        loginTitleLabel->setMinimumSize(QSize(365, 29));
        loginTitleLabel->setMaximumSize(QSize(365, 29));
        loginTitleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_57->addWidget(loginTitleLabel);


        gridLayout_19->addLayout(horizontalLayout_57, 2, 0, 1, 1);


        gridLayout->addWidget(LoginFrame, 0, 0, 1, 1);

        QWidget::setTabOrder(loginMiniButton, loginCloseButtton);
        QWidget::setTabOrder(loginCloseButtton, autoLoginCheckBox);
        QWidget::setTabOrder(autoLoginCheckBox, commonUserLoginButton);
        QWidget::setTabOrder(commonUserLoginButton, userKeyFrame);
        QWidget::setTabOrder(userKeyFrame, userNameFrame);
        QWidget::setTabOrder(userNameFrame, userKeyLineEdit);
        QWidget::setTabOrder(userKeyLineEdit, rememberKeyCheckBox);

        retranslateUi(ScreenLoginWidget);

        QMetaObject::connectSlotsByName(ScreenLoginWidget);
    } // setupUi

    void retranslateUi(QWidget *ScreenLoginWidget)
    {
        ScreenLoginWidget->setWindowTitle(QString());
        loginMiniButton->setText(QString());
        loginCloseButtton->setText(QString());
        labelError_1->setText(QApplication::translate("ScreenLoginWidget", "\347\224\250\346\210\267\345\220\215\346\210\226\345\257\206\347\240\201\351\224\231\350\257\257\357\274\201", nullptr));
        userNameFrame->setText(QString());
        userNameLabel->setText(QApplication::translate("ScreenLoginWidget", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        userKeyFrame->setText(QString());
        userKeyLabel->setText(QApplication::translate("ScreenLoginWidget", "\345\257\206  \347\240\201\357\274\232", nullptr));
        rememberKeyCheckBox->setText(QApplication::translate("ScreenLoginWidget", "\350\256\260\344\275\217\345\257\206\347\240\201", nullptr));
        autoLoginCheckBox->setText(QApplication::translate("ScreenLoginWidget", "\350\207\252\345\212\250\347\231\273\345\275\225", nullptr));
        commonUserLoginButton->setText(QApplication::translate("ScreenLoginWidget", "\347\231\273\345\275\225", nullptr));
        loginTitleLabel->setText(QApplication::translate("ScreenLoginWidget", "AIDIS\345\210\206\345\270\203\345\274\217\345\217\257\350\247\206\345\214\226\347\273\274\345\220\210\345\272\224\347\224\250\347\263\273\347\273\237", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ScreenLoginWidget: public Ui_ScreenLoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCREENLOGINWIDGET_H
