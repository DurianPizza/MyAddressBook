#-------------------------------------------------
#
# Project created by QtCreator 2017-05-29T19:25:26
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AddressBook
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    RegisterDialog.cpp \
    AddressBookWindow.cpp \
    AddAddressDialog.cpp \
    PresentSearchResult.cpp \
    stdafx.cpp \
    AddTypeDialog.cpp \
    ModifyTypeDialog.cpp \
    DeleteTypeDialog.cpp \
    ModifyItemDialog.cpp \
    DeleteItemDialog.cpp \
    AboutDialog.cpp \
    threadcontroller.cpp \
    ItemDetail.cpp \
    TypeDetail.cpp \
    MyMenuItem.cpp \
    SelectAvatarDialog.cpp \
    customlabel.cpp

HEADERS  += mainwindow.h \
    stdafx.h \
    RegisterDialog.h \
    AddressBookWindow.h \
    AddAddressDialog.h \
    PresentSearchResult.h \
    AddTypeDialog.h \
    ModifyTypeDialog.h \
    DeleteTypeDialog.h \
    ModifyItemDialog.h \
    DeleteItemDialog.h \
    AboutDialog.h \
    threadcontroller.h \
    ItemDetail.h \
    TypeDetail.h \
    MyMenuItem.h \
    SelectAvatarDialog.h \
    customlabel.h

FORMS    += mainwindow.ui \
    RegisterDialog.ui \
    AddressBookWindow.ui \
    AddAddressDialog.ui \
    PresentSearchResult.ui \
    AddTypeDialog.ui \
    ModifyTypeDialog.ui \
    DeleteTypeDialog.ui \
    ModifyItemDialog.ui \
    DeleteItemDialog.ui \
    AboutDialog.ui \
    ItemDetail.ui \
    TypeDetail.ui \
    SelectAvatarDialog.ui

RESOURCES += \
    resource.qrc
