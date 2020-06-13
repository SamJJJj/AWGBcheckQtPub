#-------------------------------------------------
#
# Project created by QtCreator 2020-06-04T17:27:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
LIBS += -lpthread libwsock32 libws2_32
TARGET = AWGBCheckTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    AWQueue.c \
    beanstalk.c \
        main.cpp \
        AWGBCheckTool.cpp \
    ConfigureGuide.cpp \
    showcheckres.cpp \
    showthread.cpp \
    showtree.cpp \
    tinyxml2.cpp

HEADERS += \
        AWGBCheckTool.h \
    AWQueue.h \
    ConfigureGuide.h \
    beanstalk.h \
    main.h \
    tinyxml2.h

FORMS += \
        AWGBCheckTool.ui \
        ConfigureGuide.ui \
_ConfigureGuide.ui
        

INCLUDEPATH += D:\Qt\myprojects\AWGBCheckTool
