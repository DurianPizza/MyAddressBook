#ifndef MODIFYTYPEDIALOG_H
#define MODIFYTYPEDIALOG_H

#include <QDialog>

namespace Ui {
class ModifyTypeDialog;
}

class ModifyTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyTypeDialog(QWidget *parent = 0);
    ~ModifyTypeDialog();

    void UpdateUI();

private slots:
    void on_buttonBox_accepted();

    void on_originTypeName_currentIndexChanged(const QString &arg1);

private:
    Ui::ModifyTypeDialog *ui;
};

#endif // MODIFYTYPEDIALOG_H
