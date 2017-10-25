#ifndef SELECTAVATARDIALOG_H
#define SELECTAVATARDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QPoint>

#include "customlabel.h"

namespace Ui {
class SelectAvatarDialog;
}

class SelectAvatarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectAvatarDialog(QWidget *parent = 0);
    ~SelectAvatarDialog();

private slots:
    void on_pushButton_clicked();
    void onMouseMove(QPoint p);
    void on_horizontalSlider_sliderMoved(int position);


    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SelectAvatarDialog *ui;
    QPixmap originalPixmap;
    QPoint centralPoint;

    int maxPixelInAxis;
    double minShrinkRatio;
    double maxShrinkRatio;
    double currentShrinkRatio;

    CustomLabel* imgLabel;

};

#endif // SELECTAVATARDIALOG_H
