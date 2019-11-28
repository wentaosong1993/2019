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
}

LangSwitch::~LangSwitch()
{

}

void LangSwitch::changeLang(int index)
{
    qInfo() << "curIndex:" << index;
    QString langCode = combo->itemData(index).toString();
    changeTr(langCode);
    refreshLabel();
}

void LangSwitch::createScreen()
{
    combo = new QComboBox;
    combo->addItem("English", "en.qm");
    combo->addItem("Chinese", "zh.qm");
    combo->addItem("Latin", "la.qm");
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
    label->setText(tr("TXT_HELLO_WORLD", "Hello World"));
}
