// modified by Brian at 2017.6.14
// * 取消了在构造函数中查询所有类型名，加快启动
// * 将删除类型名后联系人中原有的类型ID设置为空修改为引用默认分组的ID
// * comboxBox的内容不再显示默认分组，同时也删除了对默认分组检测的代码

#include "DeleteTypeDialog.h"
#include "ui_DeleteTypeDialog.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

// function: setup the interface
DeleteTypeDialog::DeleteTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteTypeDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));

    for(auto key : global::typeNameTotypeID.keys()){
        if(key == STRING("默认"))
            continue;
        ui->comboBox->addItem(key);
    }
}

DeleteTypeDialog::~DeleteTypeDialog()
{
    delete ui;
}

// called when user press the "ok" button
// to remove a data entry from database
void DeleteTypeDialog::on_buttonBox_accepted()
{
    try{
        QString selectedTypeName = ui->comboBox->currentText();

        //将引用了该typeName的ID的联系人类型字段设置为默认分组的ID
        int referencedTypeID = global::typeNameTotypeID.value(selectedTypeName);
        int defaultTypeID = global::typeNameTotypeID.value(STRING("默认"));
        QSqlQuery query(global::db);

        query.prepare(QString("UPDATE %1 SET type = ? WHERE type = ?").arg(global::tableAddressItemName));
        query.addBindValue(defaultTypeID);
        query.addBindValue(referencedTypeID);

        if(!query.exec())
            throw QString(STRING("删除联系人类别失败：") + query.lastError().text());

        query.prepare(QString("DELETE FROM %1 WHERE typeName = ?").arg(global::tableAddressTypeName));
        query.addBindValue(selectedTypeName);
        if(!query.exec())
            throw QString(STRING("删除联系人类别失败：") + query.lastError().text());

        // 改进：使用信号和槽
        global::deletedTypeName = selectedTypeName;
        global::hasDeleteTypeName = true;

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}
