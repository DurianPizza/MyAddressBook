#ifndef DELETEITEMDIALOG_H
#define DELETEITEMDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteItemDialog;
}

class DeleteItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteItemDialog(QWidget *parent = 0);
    ~DeleteItemDialog();

private slots:
    void on_comboBoxItemID_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::DeleteItemDialog *ui;
    QString avatarDir;
    QPixmap avatar;
};

#endif // DELETEITEMDIALOG_H
