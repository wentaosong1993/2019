QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LangSwitch
TEMPLATE = app

TRANSLATIONS += lang_en.ts \
                lang_zh.ts \
                lang_la.ts \


CONFIG += c++11

SOURCES += \
        main.cpp \
        langswitch.cpp

HEADERS += \
        langswitch.h
