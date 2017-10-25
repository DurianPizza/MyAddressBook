// Created by Brian at 2017.6.19
// 继承QAction类，实现了当菜单项被点击时发送自己的名字

#ifndef MYMENUITEM_H
#define MYMENUITEM_H
#include <QAction>
#include <QString>


class MyMenuItem : public QAction
{
    Q_OBJECT // 要想自定义的槽和信号生效，必须添加这个宏

public:
    MyMenuItem(QObject *parent = nullptr);
    MyMenuItem(const QString &text, QObject *parent = nullptr);
    MyMenuItem(const QIcon &icon, const QString &text, QObject *parent = nullptr);
    ~MyMenuItem();

signals:
    void onTriggered(QString menuItemName);

private slots:
    void onTriggeredSlot(); // 将QActon的triggered信号和onTriggeredSlot绑定起来
    // onTriggeredSlot再发送onTriggered信号

private:
    QString menuName;
};

#endif // MYMENUITEM_H
