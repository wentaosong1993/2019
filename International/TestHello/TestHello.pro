QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestHello
TEMPLATE = app
TRANSLATIONS = TestHello.ts
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
SOURCES += \
        main.cpp \
        testhello.cpp
HEADERS += \
        testhello.h
FORMS += \
        testhello.ui



