// modified by Brian at 2017.6.14
// * 取消了在构造函数中查询所有类型名，加快启动
// * 考虑了修改后的类型名和已有的类型名相同的情况
// * 添加了UpdateUI函数，每次启动这个窗口时调用这个函数
// * 解决了改变类型名后comboBox的内容显示不正确的问题
// * comboxBox的内容不再显示默认分组，同时也删除了对默认分组检测的代码

#include "ModifyTypeDialog.h"
#include "ui_ModifyTypeDialog.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

// function: update the interface
ModifyTypeDialog::ModifyTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyTypeDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
}

ModifyTypeDialog::~ModifyTypeDialog()
{
    delete ui;
}

// function: called when user press "ok" button
//           to update data from database
void ModifyTypeDialog::on_buttonBox_accepted()
{
    try{
        QString originTypeName = ui->originTypeName->currentText();
        QString newTypeName = ui->newTypeName->text();
        QString newDetail = ui->newDetail->toPlainText();

        if(originTypeName == STRING("默认"))
            throw QString(STRING("不能修改默认类型名"));

        if(newTypeName != originTypeName && global::typeNameTotypeID.keys().contains(newTypeName))
            throw QString(STRING("类型名") + newTypeName + STRING("已经存在"));

        if(newTypeName == "")
        //    throw QString(STRING("类型名不能为空"));
            newTypeName = originTypeName;

        //if(newTypeName == originTypeName)
        //    throw QString(STRING("原类型名和新类型名相同"));

        QSqlQuery query(global::db);
        query.prepare(QString("UPDATE %1 SET typeName = ?, detail = ? WHERE typeName = ?").arg(global::tableAddressTypeName));
        query.addBindValue(newTypeName);
        query.addBindValue(newDetail);
        query.addBindValue(originTypeName);

        if(!query.exec())
            throw QString(STRING("修改失败") + query.lastError().text());


        // 使用信号和槽机制更好
        global::hasModifyTypeName = true;
        global::originalTypeName = originTypeName;
        global::modifiedTypeName = newTypeName;
        global::modifiedDetail = newDetail;

    }
    catch(QString& e){
            QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

void ModifyTypeDialog::UpdateUI()
{
    for(auto key : global::typeNameTotypeID.keys()){
        if(key == STRING("默认"))
            continue;
        ui->originTypeName->addItem(key);
    }
}

void ModifyTypeDialog::on_originTypeName_currentIndexChanged(const QString &arg1)
{
    try{
        QSqlQuery query(global::db);
        query.prepare(QString("SELECT detail FROM %1 WHERE typeName = ?").arg(global::tableAddressTypeName));
        query.addBindValue(arg1);
        if(!query.exec())
            throw QString(query.lastError().text());
        query.next();
        ui->newDetail->setText(query.value(0).toString());
        ui->newTypeName->setText(arg1);

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}
