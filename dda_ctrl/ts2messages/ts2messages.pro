#-------------------------------------------------
#
# Project created by QtCreator 2012-11-08T14:52:02
#
#-------------------------------------------------

QT       += core xml
QT       -= gui

TARGET = ts2messages
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    get_opts.cpp \
    ../message_file.cpp

HEADERS += \
    get_opts.h \
    ../message_file.h

INCLUDEPATH += \
 ..
