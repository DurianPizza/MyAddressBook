// modified by Brian at 2017.6.16
// * 在注册事件处理函数中尝试连接服务器

#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = 0);
    ~RegisterDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    // 自定义槽
    void onRegister();
private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
