#ifndef DELETETYPEDIALOG_H
#define DELETETYPEDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteTypeDialog;
}

class DeleteTypeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteTypeDialog(QWidget *parent = 0);
    ~DeleteTypeDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DeleteTypeDialog *ui;
};

#endif // DELETETYPEDIALOG_H
