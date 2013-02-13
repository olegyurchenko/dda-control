TEMPLATE = app
CONFIG += console
CONFIG -= qt

HEADERS += \
    src/sys_timer.h \
    src/sys_sheduler.h \
    src/sys_adc.h \
    src/spin_edit.h \
    src/menu.h \
    src/loop_buff.h \
    src/interrupts.h \
    src/event.h \
    src/decimal.h \
    src/dda_work_mode.h \
    src/dda_uart.h \
    src/dda_text.h \
    src/dda_test_mode.h \
    src/dda_sensor.h \
    src/dda_protocol.h \
    src/dda_plunger.h \
    src/dda_motors.h \
    src/dda_mode.h \
    src/dda_message.h \
    src/dda_mesh.h \
    src/dda_lcd.h \
    src/dda_key.h \
    src/dda_db.h \
    src/dda_conv.h \
    src/dda_config.h \
    src/dda_clib.h \
    src/dda_cassette.h \
    src/console.h \
    src/view.h \
    src/dda_result_view.h \
    src/sys_flash.h \
    src/dda_settings.h \
    src/crc8.h \
    src/dda_flash.h \
    src/dda_text.c.utf8

SOURCES += \
    src/sys_timer.c \
    src/sys_sheduler.c \
    src/sys_adc.c \
    src/spin_edit.c \
    src/menu.c \
    src/main.c \
    src/loop_buff.c \
    src/interrupts.c \
    src/event.c \
    src/decimal.c \
    src/dda_work_mode.c \
    src/dda_uart.c \
    src/dda_test_mode.c \
    src/dda_sensor.c \
    src/dda_protocol.c \
    src/dda_plunger.c \
    src/dda_motors.c \
    src/dda_mode.c \
    src/dda_message.c \
    src/dda_mesh.c \
    src/dda_lcd.c \
    src/dda_key.c \
    src/dda_db.c \
    src/dda_conv.c \
    src/dda_clib.c \
    src/dda_cassette.c \
    src/console.c \
    src/view.c \
    src/dda_result_view.c \
    src/sys_flash.c \
    src/dda_settings.c \
    src/crc8.c

INCLUDEPATH += \
../lib/CMSIS_CM3 \
../lib/STM32F10x_StdPeriph_Driver/inc
