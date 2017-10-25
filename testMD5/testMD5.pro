#-------------------------------------------------
#
# Project created by QtCreator 2017-10-25T20:57:57
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_testmd5test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_testmd5test.cpp \
    md5.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    testmd5.h
