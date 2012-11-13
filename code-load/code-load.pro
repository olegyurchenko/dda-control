TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    ../dda_ctrl/pc_serial.c \
    ../dda_ctrl/get_opts.cpp \
    boot_protocol.cpp

HEADERS += \
    ../dda_ctrl/pc_serial.h \
    ../dda_ctrl/get_opts.h \
    boot_protocol.h

INCLUDEPATH += \
 ../dda_ctrl/

win32 {
  DEFINES += WIN32
}

!win32 {
  DEFINES += UNIX LINUX
}
