#-------------------------------------------------
#
# Project created by QtCreator 2012-10-11T08:12:58
#
#-------------------------------------------------

QT       += core gui xml sql declarative

TARGET = dda_analysis
TEMPLATE = app


SOURCES += main.cpp\
        analyswindow.cpp \
    ../usermanagedialog.cpp \
    ../sessiondialog.cpp \
    ../sessionbox.cpp \
    ../session.cpp \
    ../selsessionbox.cpp \
    ../plotwidget.cpp \
    ../plotter.cpp \
    ../measuremodel.cpp \
    ../histogrammplotter.cpp \
    ../graphplotter.cpp \
    ../database.cpp \
    ../curveplotter.cpp \
    ../configuration.cpp \
    ../axisplotter.cpp \
    ../newpassworddialog.cpp \
    ../qmlprocessing.cpp \
    ../editdatadialog.cpp

HEADERS  += analyswindow.h \
    ../usermanagedialog.h \
    ../sessiondialog.h \
    ../sessionbox.h \
    ../session.h \
    ../selsessionbox.h \
    ../plotwidget.h \
    ../plotter.h \
    ../measuremodel.h \
    ../histogrammplotter.h \
    ../graphplotter.h \
    ../database.h \
    ../curveplotter.h \
    ../configuration.h \
    ../axisplotter.h \
    ../newpassworddialog.h \
    ../qmlprocessing.h \
    ../editdatadialog.h

FORMS    += analyswindow.ui \
    ../usermanagedialog.ui \
    ../sessiondialog.ui \
    ../sessionbox.ui \
    ../selsessionbox.ui \
    ../newpassworddialog.ui \
    ../editdatadialog.ui

DEFINES += USE_QML

win32 {
  DEFINES += WIN32
}

!win32 {
  DEFINES += UNIX LINUX
}

win32 {
   RC_FILE = dda_analysis.rc
}

INCLUDEPATH += \
 ..

RESOURCES += \
    ../resuurces/resources.qrc

OTHER_FILES += \
    ../resuurces/processing.qml \
    processing.js
