#include "MyMenuItem.h"

// usage: MyMenuItem inherit QAction
//        to emit a signal which parameter is the name it present on
//        ui

// function: bind signal and slot
MyMenuItem::MyMenuItem(QObject *parent)
    :QAction(parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(onTriggeredSlot()));
    menuName = "";
}

// reloaded constructor
MyMenuItem::MyMenuItem(const QString &text, QObject *parent)
    :QAction(text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(onTriggeredSlot()));
    menuName = text;
}

// reloaded constructor
MyMenuItem::MyMenuItem(const QIcon &icon, const QString &text, QObject *parent)
    :QAction(icon, text, parent)
{
    connect(this, SIGNAL(triggered()), this, SLOT(onTriggeredSlot()));
    menuName = text;
}

MyMenuItem::~MyMenuItem()
{

}

// function: emit the self defined signal
void MyMenuItem::onTriggeredSlot()
{
    emit onTriggered(menuName);
}
