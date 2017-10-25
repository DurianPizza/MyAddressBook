#include "mainwindow.h"
#include "stdafx.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try
    {
        MainWindow w;
        a.setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Common.qss"));
        w.show();
        auto ret = a.exec();
        global::CloseDatabase();
        return ret;
    }
    catch(std::exception e)
    {
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
        global::CloseDatabase();
    }
    return -1;
}
