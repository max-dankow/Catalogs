TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=c99

SOURCES += main.c \
    Hash-Table.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    Hash-Table.h

