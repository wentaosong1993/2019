#-------------------------------------------------
#
# Project created by QtCreator 2019-11-28T11:54:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test
TEMPLATE = app

CONFIG += c++11

TRANSLATIONS += lang_en.ts \
                lang_zh.ts \
                lang_la.ts \

SOURCES += \
        MainEntrance.cpp \
        ScreenLoginWidget.cpp \
        main.cpp

HEADERS += \
    MainEntrance.h \
    ScreenLoginWidget.h \
    globalApi.h

FORMS += \
    ScreenLoginWidget.ui
