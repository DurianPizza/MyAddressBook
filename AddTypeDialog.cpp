#include "AddTypeDialog.h"
#include "ui_AddTypeDialog.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>

// function: set the style sheet
AddTypeDialog::AddTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTypeDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
}

AddTypeDialog::~AddTypeDialog()
{
    delete ui;
}

// function: called when user pressed "ok" button,
//           this function just insert new data entry to database
void AddTypeDialog::on_buttonBox_accepted()
{
    try{
        QString newTypeName = ui->newTypeName->text();
        if(newTypeName == "")
            throw QString(STRING("类型名不能为空"));
        QString detail = ui->detail->text();
        QSqlQuery query(global::db);
        query.prepare(QString("INSERT INTO %1(typeName, detail)"
                      "VALUES(?, ?)").arg(global::tableAddressTypeName));
        query.addBindValue(newTypeName);
        query.addBindValue(detail == "" ? QVariant(QVariant::String) : detail);
        if(!query.exec())
            throw QString(STRING("插入失败") + query.lastError().text());
        global::hasInsertNewType = true;
        global::newTypeName = newTypeName;
        global::newTypeDescription = detail;

        // 添加类型名到类型ID的映射
        query.prepare(QString("SELECT typeID FROM %1 WHERE typeName = ?").arg(global::tableAddressTypeName));
        query.addBindValue(newTypeName);
        query.exec();query.next();
        global::typeNameTotypeID.insert(newTypeName, query.value(0).toInt());
    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}
