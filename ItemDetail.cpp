#include "ItemDetail.h"
#include "ui_ItemDetail.h"

// function: set up the interface
ItemDetail::ItemDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemDetail)
{
    ui->setupUi(this);
    // 隐藏标题栏和任务栏图标
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
}

ItemDetail::~ItemDetail()
{
    delete ui;
}

// setters, to update the content of floating panel
void ItemDetail::setPresentName(const QString& s){ui->labelPresentName->setText(s);}
void ItemDetail::setPresentType(const QString& s){ui->labelPresentType->setText(s);}
void ItemDetail::setPresentSex(const QString& s){ui->labelPresentSex->setText(s);}
void ItemDetail::setPresentPhone(const QString& s){ui->labelPresentPhone->setText(s);}
void ItemDetail::setPresentEmail(const QString& s){ui->labelPresentEmail->setText(s);}
void ItemDetail::setPresentNote(const QString& s){ui->labelPresentNote->setText(s);}
void ItemDetail::setPresentAvatar(const QPixmap& map){ui->labelAvatar->setPixmap(map);}
