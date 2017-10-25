#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "stdafx.h"

// function: just set the style sheet
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_buttonOk_clicked()
{
    this->close();
}
