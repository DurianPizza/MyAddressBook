#ifndef MODIFYITEMDIALOG_H
#define MODIFYITEMDIALOG_H

#include <QDialog>

namespace Ui {
class ModifyItemDialog;
}

class ModifyItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyItemDialog(QWidget *parent = 0);
    ~ModifyItemDialog();

private slots:
    void on_comboBoxItemID_currentIndexChanged(const QString &arg1);

    void on_buttonModifyAvatar_clicked();

    void on_buttonBox_accepted();

private:
    Ui::ModifyItemDialog *ui;
    QString avatarDir;
    QPixmap avatar;
};

#endif // MODIFYITEMDIALOG_H
