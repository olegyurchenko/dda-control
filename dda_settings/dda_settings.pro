TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    dda_protocol.cpp \
    ../dda_ctrl/pc_serial.c \
    ../dda_ctrl/get_opts.cpp \
    ../dda_ctrl/logfile.cpp

HEADERS += \
    dda_protocol.h \
    ../dda_ctrl/pc_serial.h \
    ../dda_ctrl/get_opts.h \
    ../dda_ctrl/logfile.h



INCLUDEPATH += \
    ../dda_ctrl/


win32 {
  DEFINES += WIN32
}

!win32 {
  DEFINES += UNIX LINUX
}
