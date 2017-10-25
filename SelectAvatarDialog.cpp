#include "SelectAvatarDialog.h"
#include "ui_SelectAvatarDialog.h"
#include "stdafx.h"

#include <QFileDialog>
#include <QDebug>

extern QByteArray avatarByteArr;
extern QByteArray smallAvatarByteArr;

// function: just set up the interface
SelectAvatarDialog::SelectAvatarDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectAvatarDialog),
    imgLabel(new CustomLabel(this))
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
    ui->horizontalSlider->setDisabled(true);
    connect(imgLabel, SIGNAL(onMouseMove(QPoint)), this, SLOT(onMouseMove(QPoint)));

    imgLabel->setGeometry(60, 60, 350, 350);
    //imgLabel->setText("SelectAvatarImage");
    //imgLabel->show();

    global::hasSelectedNewAvatar = false;
}

SelectAvatarDialog::~SelectAvatarDialog()
{
    delete ui;
}

// function: called when user load image
//           to show the new add image
//           and initialize some variables
void SelectAvatarDialog::on_pushButton_clicked()
{
    QString avatarDir = QFileDialog::getOpenFileName(this, STRING("选择头像"), ".",
                                                     tr(global::selectFilter));

    if(avatarDir == ""){
        global::hasSelectedNewAvatar = false;
        return;
    }
    global::hasSelectedNewAvatar = true;

    originalPixmap = QPixmap(avatarDir);
    maxPixelInAxis = originalPixmap.width() > originalPixmap.height() ?
                originalPixmap.height() : originalPixmap.width();

    if(maxPixelInAxis < imgLabel->width()){
        ui->horizontalSlider->setDisabled(true);
    }else{
        ui->horizontalSlider->setDisabled(false);
    }

    ui->horizontalSlider->setSliderPosition(0);
    minShrinkRatio = 1.0;
    maxShrinkRatio = (double)imgLabel->width() / maxPixelInAxis;
    currentShrinkRatio = 1.0;
    centralPoint.setX(originalPixmap.width() / 2);
    centralPoint.setY(originalPixmap.height() / 2);

    QPixmap showImg = originalPixmap.copy(centralPoint.x() - maxPixelInAxis / 2,
                                          centralPoint.y() - maxPixelInAxis / 2,
                                          maxPixelInAxis, maxPixelInAxis);

    imgLabel->setPixmap(showImg.scaled(imgLabel->width(), imgLabel->height()));
}

// function: called when user drag the image and try to move it
//           ,this function change the area which will be show
//           and used as an avatar
void SelectAvatarDialog::onMouseMove(QPoint p)
{
    if(originalPixmap.isNull())return;

    //qDebug() << QString("x %1 y %2").arg(p.x()).arg(p.y());
    // 中心点做相对运动
    int x = centralPoint.x() + p.x();
    int y = centralPoint.y() + p.y();

    // 检测中心点形成的正方形是否超出图像边界
    int currentPixelInAxis = (int)(maxPixelInAxis * currentShrinkRatio);
    int newX = x - currentPixelInAxis / 2;
    int newY = y - currentPixelInAxis / 2;
    if(newX < 0) newX = 0;
    if(newX + currentPixelInAxis > originalPixmap.width())
        newX = originalPixmap.width() - currentPixelInAxis;
    if(newY < 0) newY = 0;
    if(newY + currentPixelInAxis > originalPixmap.height())
        newY = originalPixmap.height() - currentPixelInAxis;

    centralPoint.setX(newX + currentPixelInAxis / 2);
    centralPoint.setY(newY + currentPixelInAxis / 2);

    QPixmap showImg = originalPixmap.copy(newX,
                                          newY,
                                          currentPixelInAxis, currentPixelInAxis);

    imgLabel->setPixmap(showImg.scaled(imgLabel->width(), imgLabel->height()));

}

// function: called when user drag the slide bar
//           to scale the area which will be shown
//           and used as an avatar
void SelectAvatarDialog::on_horizontalSlider_sliderMoved(int position)
{
    if(originalPixmap.isNull())return;

    int maximum = ui->horizontalSlider->maximum();
    int minimum = ui->horizontalSlider->minimum();

    currentShrinkRatio = (double)(position - minimum) / (maximum - minimum) *
            (maxShrinkRatio - minShrinkRatio) + minShrinkRatio;

    int currentPixelInAxis = (int)(maxPixelInAxis * currentShrinkRatio);
    int newX = centralPoint.x() - currentPixelInAxis / 2;
    int newY = centralPoint.y() - currentPixelInAxis / 2;
    if(newX < 0) newX = 0;
    if(newX + currentPixelInAxis > originalPixmap.width())
        newX = originalPixmap.width() - currentPixelInAxis;
    if(newY < 0) newY = 0;
    if(newY + currentPixelInAxis > originalPixmap.height())
        newY = originalPixmap.height() - currentPixelInAxis;

    centralPoint.setX(newX + currentPixelInAxis / 2);
    centralPoint.setY(newY + currentPixelInAxis / 2);

    QPixmap showImg = originalPixmap.copy(newX,
                                          newY,
                                          currentPixelInAxis, currentPixelInAxis);

    imgLabel->setPixmap(showImg.scaled(imgLabel->width(), imgLabel->height()));
}

// function: called when user press the "ok" button
void SelectAvatarDialog::on_buttonBox_accepted()
{
    if(global::hasSelectedNewAvatar){
        int currentPixelInAxis = (int)(maxPixelInAxis * currentShrinkRatio);

        QPixmap showImg = originalPixmap.copy(centralPoint.x() - currentPixelInAxis / 2,
                                              centralPoint.y() - currentPixelInAxis / 2,
                                              currentPixelInAxis, currentPixelInAxis);

        // 使用信号和槽

        QDataStream ds(&avatarByteArr, QIODevice::WriteOnly); // 80 * 80
        ds << showImg.scaled(global::addItemDialogAvatarWidth,
                            global::addItemDialogAvatarHeight);

        QDataStream ds1(&smallAvatarByteArr, QIODevice::WriteOnly); // 40 * 40
        ds1 << showImg.scaled(global::avatarWidth,
                              global::avatarHeight);
    }

    // 不能自动关闭？
    this->close();
}

void SelectAvatarDialog::on_buttonBox_rejected()
{
    global::hasSelectedNewAvatar = false;
    this->close();
}
