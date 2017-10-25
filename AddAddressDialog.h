#ifndef ADDADDRESSDIALOG_H
#define ADDADDRESSDIALOG_H

#include <QDialog>

namespace Ui {
class AddAddressDialog;
}

class AddAddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAddressDialog(QWidget *parent, const QString& userName);
    ~AddAddressDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonAddAvatar_clicked();

private:
    Ui::AddAddressDialog *ui;
    QString userName;
    QString avatarDir;
};

#endif // ADDADDRESSDIALOG_H
