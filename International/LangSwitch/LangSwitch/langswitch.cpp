#include "langswitch.h"
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QTranslator>
#include <QApplication>
#include <QString>
#include <QDebug>

LangSwitch::LangSwitch(QWidget *parent)
    : QWidget(parent)
{
    createScreen();
    QStringList language_list;
    //language_list<< tr("English")<<tr("Chinese")<<tr("Latin");
}

LangSwitch::~LangSwitch()
{

}

void LangSwitch::setText2label(const char *text)
{
    _strText = QString(text);
    label->setText(QCoreApplication::translate("QLabel",_strText.toStdString().c_str()));
}

void LangSwitch::changeLang(int index)
{
    qInfo() << "curIndex:" << index;
    QString langCode = combo->itemData(index).toString();
    changeTr(langCode);
    //    refreshLabel();
}

void LangSwitch::changeEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::LanguageChange:
        refreshLabel();
        break;
    default:
        break;
    }
}

void LangSwitch::createScreen()
{
    combo = new QComboBox;
    combo->addItem(tr("English"), "en.qm");
    combo->addItem(tr("Chinese"), "zh.qm");
    combo->addItem(tr("Latin"), "la.qm");//اللغة العربية
    label = new QLabel;
    refreshLabel();
    QVBoxLayout * layout = new QVBoxLayout;
    layout->addWidget(combo,1);
    layout->addWidget(label,5);
    setLayout(layout);
    connect(combo, SIGNAL(currentIndexChanged(int)),this,SLOT(changeLang(int)));
}

void LangSwitch::changeTr(const QString &langCode)
{
    static QTranslator * translator;
    if(translator != NULL)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator = NULL;
    }

    translator = new QTranslator;
    QString qmFileName = "lang_" + langCode;
    qInfo() << "qmFileName:" << qmFileName;
    if(translator->load(QString("D:/2019/International/LangSwitch/LangSwitch/") + qmFileName))
    {
        qApp->installTranslator(translator);
    }
}

void LangSwitch::refreshLabel()
{
    label->setText(QCoreApplication::translate("QLabel",_strText.toStdString().c_str()));
}
