#-------------------------------------------------
#
# Project created by QtCreator 2012-11-08T19:29:21
#
#-------------------------------------------------

QT       += core gui xml

TARGET = message-edit
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../message_file.cpp

HEADERS  += mainwindow.h \
    ../message_file.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc

INCLUDEPATH += \
 ..
