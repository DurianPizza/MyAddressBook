#include "TypeDetail.h"
#include "ui_TypeDetail.h"

// usage: show the floating panel of address type

// function: set up the interface
TypeDetail::TypeDetail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TypeDetail)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
}

TypeDetail::~TypeDetail()
{
    delete ui;
}

// setters
void TypeDetail::setPresentTypeName(const QString& str){ui->labelPresentTypeName->setText(str);}
void TypeDetail::setPresentDescription(const QString& str){ui->labelPresentDescription->setText(str);}
