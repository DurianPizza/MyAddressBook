// modified By Brian at 2017.6.15
// * 添加了自定义槽logIn

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AddressBookWindow.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_login_clicked();

    void on_regist_clicked();

    void on_forget_clicked();

    // 不要使用onCurrentTextChanged信号，因为可编辑的comboBox也会发出这个信号
    void on_userNameComboBox_currentIndexChanged(const QString &arg1);

    void on_savePasswordCheckBox_stateChanged(int arg1);

    // 自定义槽
    void logIn();
    void onUserLogout();
    void on_deleteRecord_triggered();

    void on_clearAllRecord_triggered();

    void on_userNameComboBox_currentTextChanged(const QString &arg1);

private:

    // 辅助结构
    struct PasswordHashCode
    {
        QByteArray hashCode;
        int passwordLen;
        int recordID;
    };

    Ui::MainWindow *ui;

    QSqlDatabase sqliteDatabase;
    QMap<QString, PasswordHashCode*> passwordHashCodes;

    AddressBookWindow* addressBookWnd;
};

#endif // MAINWINDOW_H
