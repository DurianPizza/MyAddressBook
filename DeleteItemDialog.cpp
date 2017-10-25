// created by Brian at 2017.6.13
// modified from ModifiedItemDialog.cpp

#include "DeleteItemDialog.h"
#include "ui_DeleteItemDialog.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>

// function: intialize the interface
DeleteItemDialog::DeleteItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteItemDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
    QSqlQuery query(global::db);

    // 将AddressGroupItem改成外部类就不需要从数据库中获取数据了
    // 这是当初设计考虑不周所致
    query.prepare(QString("SELECT typeName FROM %1").arg(global::tableAddressTypeName));
    query.exec();
    // 不选择任何类别
    ui->type->addItem("");
    while(query.next()){
        ui->type->addItem(query.value(0).toString());
    }
    query.prepare(QString("SELECT itemID FROM %1").arg(global::tableAddressItemName));
    query.exec();
    // 不选择任何联系人ID
    ui->comboBoxItemID->addItem("");
    while(query.next())
        ui->comboBoxItemID->addItem(query.value(0).toString());
}

DeleteItemDialog::~DeleteItemDialog()
{
    delete ui;
}

// function: called when user changed the current item
//           to update the interface
void DeleteItemDialog::on_comboBoxItemID_currentIndexChanged(const QString &arg1)
{
    if(arg1.isEmpty()){
        ui->name->setText("");
        ui->type->setCurrentText("");
        ui->sex->setCurrentText("");
        ui->phone->setText("");
        ui->email->setText("");
        ui->note->setText("");
        ui->labelShowAvatar->setPixmap(QPixmap()); // 设置空的头像
    }
    else{
        try{

            // 将AddressItem改为外部类就不需要这么麻烦了
            // 这是设计时考虑不周所引起的问题

            int itemID = arg1.toInt();
            QSqlQuery query(global::db);
            query.prepare(QString("SELECT itemName, typeName, sex, phone, email, note, avatar FROM %1, %2"
                                  " WHERE %3.type = %4.typeID AND itemID = ?")
                          .arg(global::tableAddressItemName)
                          .arg(global::tableAddressTypeName)
                          .arg(global::tableAddressItemName)
                          .arg(global::tableAddressTypeName));
            query.addBindValue(itemID);
            query.exec();query.next();

            // 保存原来的类型名
           global::deletedItemTypeName = query.value(1).toString();

            ui->name->setText(query.value(0).toString());
            ui->type->setCurrentText(query.value(1).toString());
            ui->sex->setCurrentText(query.value(2).toString());
            ui->phone->setText(query.value(3).toString());
            ui->email->setText(query.value(4).toString());
            ui->note->setText(query.value(5).toString());

            QByteArray imgByteArray= query.value(6).toByteArray();
            QPixmap avatar;
            if(!imgByteArray.isEmpty()){
                QDataStream ds(imgByteArray);
                ds >> avatar;
            }
            ui->labelShowAvatar->setPixmap(avatar);

        }catch(std::exception e){
            QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}

// function: called when user pressed the "ok" button
//           to remove a data entry from database
void DeleteItemDialog::on_buttonBox_accepted()
{
    try{
        QString sID = ui->comboBoxItemID->currentText();
        int itemID;
        if(sID.isEmpty())
            throw QString(STRING("没有选择任何联系人"));
        else
            itemID = sID.toInt();

        // 这里使用信号和槽的机制更好
        QSqlQuery query(global::db);
        query.prepare(QString("DELETE FROM %1 WHERE itemID = ?").arg(global::tableAddressItemName));
        query.addBindValue(itemID);
        if(!query.exec())
            throw QString(STRING("执行失败") + query.lastError().text());
        global::hasDeleteAddressItem = true;
        global::deletedItemID = itemID;

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}
