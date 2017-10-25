#ifndef ADDTYPEDIALOG_H
#define ADDTYPEDIALOG_H

#include <QDialog>

namespace Ui {
class AddTypeDialog;
}

class AddTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTypeDialog(QWidget *parent = 0);
    ~AddTypeDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddTypeDialog *ui;
};

#endif // ADDTYPEDIALOG_H
