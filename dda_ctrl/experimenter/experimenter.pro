#-------------------------------------------------
#
# Project created by QtCreator 2012-09-24T10:52:47
#
#-------------------------------------------------

QT       += core gui xml sql

TARGET = experimenter
TEMPLATE = app


SOURCES += main.cpp\
        measurewindow.cpp \
    ../configuration.cpp \
    ../profileselectdialog.cpp \
    ../editprofiledialog.cpp \
    ../database.cpp \
    ../plotwidget.cpp \
    ../plotter.cpp \
    ../histogrammplotter.cpp \
    ../graphplotter.cpp \
    ../curveplotter.cpp \
    ../axisplotter.cpp \
    ../sessiondialog.cpp \
    ../controller.cpp \
    ../pc_serial.c \
    ../get_opt.cpp \
    ../session.cpp \
    ../measuremodel.cpp \
    ../usermanagedialog.cpp \
    ../newpassworddialog.cpp

HEADERS  += measurewindow.h \
    ../configuration.h \
    ../profileselectdialog.h \
    ../editprofiledialog.h \
    ../database.h \
    ../plotwidget.h \
    ../plotter.h \
    ../histogrammplotter.h \
    ../graphplotter.h \
    ../curveplotter.h \
    ../axisplotter.h \
    ../sessiondialog.h \
    ../controller.h \
    ../pc_serial.h \
    ../get_opt.h \
    ../session.h \
    ../measuremodel.h \
    ../usermanagedialog.h \
    ../newpassworddialog.h

FORMS    += measurewindow.ui \
    ../profileselectdialog.ui \
    ../editprofiledialog.ui \
    ../sessiondialog.ui \
    ../usermanagedialog.ui \
    ../newpassworddialog.ui

INCLUDEPATH += \
 ..

RESOURCES += \
    ../resuurces/resources.qrc