#-------------------------------------------------
#
# Project created by QtCreator 2012-09-17T10:40:40
#
#-------------------------------------------------

QT       += core gui

TARGET = histogramm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../dda_ctrl/plotwidget.cpp \
    ../dda_ctrl/plotter.cpp \
    ../dda_ctrl/axisplotter.cpp \
    ../dda_ctrl/histogrammplotter.cpp \
    ../dda_ctrl/graphplotter.cpp \
    ../dda_ctrl/curveplotter.cpp

HEADERS  += mainwindow.h \
    ../dda_ctrl/plotwidget.h \
    ../dda_ctrl/plotter.h \
    ../dda_ctrl/axisplotter.h \
    ../dda_ctrl/histogrammplotter.h \
    ../dda_ctrl/graphplotter.h \
    ../dda_ctrl/curveplotter.h

FORMS    += mainwindow.ui

INCLUDEPATH += \
../dda_ctrl
