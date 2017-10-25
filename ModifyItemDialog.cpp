// created by Brian at 2017.6.13

// modified by Brian at 2017.6.15
// * 添加了对右键菜单-修改联系人的支持

#include "ModifyItemDialog.h"
#include "ui_ModifyItemDialog.h"
#include "SelectAvatarDialog.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFileDialog>

extern QByteArray avatarByteArr;
extern QByteArray smallAvatarByteArr;

// function: setup the interface
ModifyItemDialog::ModifyItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyItemDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));
    QSqlQuery query(global::db);

    // 将AddressGroupItem 改为外部类就不需要从数据库再次获取数据了
    query.prepare(QString("SELECT typeName FROM %1").arg(global::tableAddressTypeName));
    query.exec();

    while(query.next()){
        ui->type->addItem(query.value(0).toString());
    }

    // 改进：将AddressItem 改为外部类
    query.prepare(QString("SELECT itemID FROM %1").arg(global::tableAddressItemName));
    query.exec();

    // 不选择任何联系人ID
    ui->comboBoxItemID->addItem("");
    while(query.next())
        ui->comboBoxItemID->addItem(query.value(0).toString());

    // 这里使用信号和槽的机制更好
    if(global::specificItemID != -1){
        query.prepare(QString("SELECT itemName, typeName, sex, phone, email, note, avatar FROM %1, %2"
                              " WHERE %3.type = %4.typeID AND itemID = ?")
                      .arg(global::tableAddressItemName)
                      .arg(global::tableAddressTypeName)
                      .arg(global::tableAddressItemName)
                      .arg(global::tableAddressTypeName));
        query.addBindValue(global::specificItemID);
        query.exec();query.next();

        ui->name->setText(query.value(0).toString());
        ui->type->setCurrentText(query.value(1).toString());
        ui->sex->setCurrentText(query.value(2).toString());
        ui->phone->setText(query.value(3).toString());
        ui->email->setText(query.value(4).toString());
        ui->note->setText(query.value(5).toString());
        ui->comboBoxItemID->setCurrentText(QString::number(global::specificItemID));

        QByteArray imgByteArray= query.value(6).toByteArray();
        QPixmap avatar;
        if(!imgByteArray.isEmpty()){
            QDataStream ds(imgByteArray);
            ds >> avatar;
        }
        ui->labelShowAvatar->setPixmap(avatar);
        global::specificItemID = -1;
    }
}

ModifyItemDialog::~ModifyItemDialog()
{
    delete ui;
}

// function: called when user changed the current item of combobox
//           to update the interface
void ModifyItemDialog::on_comboBoxItemID_currentIndexChanged(const QString &arg1)
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
            int itemID = arg1.toInt();
            QSqlQuery query(global::db);

            // 改进：将AddressItem 改为外部类
            query.prepare(QString("SELECT itemName, typeName, sex, phone, email, note, avatar FROM %1, %2"
                                  " WHERE %3.type = %4.typeID AND itemID = ?")
                          .arg(global::tableAddressItemName)
                          .arg(global::tableAddressTypeName)
                          .arg(global::tableAddressItemName)
                          .arg(global::tableAddressTypeName));
            query.addBindValue(itemID);
            query.exec();query.next();

            // 保存原来的类型名
           global::originalType = query.value(1).toString();

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

// function: pop up the modify image window
void ModifyItemDialog::on_buttonModifyAvatar_clicked()
{
    SelectAvatarDialog* dialog = new SelectAvatarDialog();
    dialog->exec();
    delete dialog;

    if(global::hasSelectedNewAvatar)
    {
        // 选择了新头像的情况
        QPixmap modifiedAvatar;
        // 40 * 40 show in modify item dialog
        QDataStream ds(&smallAvatarByteArr, QIODevice::ReadOnly);
        ds >> modifiedAvatar;

        ui->labelShowAvatar->setPixmap(modifiedAvatar);
    }
}

// function: called when user pressed "ok" button
//           to update data from database
void ModifyItemDialog::on_buttonBox_accepted()
{
    try{
        QString sID = ui->comboBoxItemID->currentText();
        int itemID;
        if(sID.isEmpty())
            throw("没有选择任何联系人");
        else
            itemID = sID.toInt();

        QString name = ui->name->text();
        QString type = ui->type->currentText();
        QString sex = ui->sex->currentText();
        QString phone = ui->phone->text();
        QString email = ui->email->text();
        QString note = ui->note->text();

        if(name.isEmpty())
            throw QString(STRING("姓名不能为空"));
        if(type.isEmpty())
            throw QString(STRING("类别不能为空"));
        if(sex.isEmpty())
            throw QString(STRING("性别不能为空"));
        QSqlQuery query(global::db);

        // 获取类型ID
        int typeID = global::typeNameTotypeID.value(type);
        if(!global::hasSelectedNewAvatar){
            query.prepare(QString("UPDATE %1 SET itemName = ?, type = ?, sex = ?, "
                                  "phone = ?, email = ?, note = ?"
                                  " WHERE itemID = ?").arg(global::tableAddressItemName));
            query.addBindValue(name);
            query.addBindValue(typeID);
            query.addBindValue(sex);
            query.addBindValue(phone);
            query.addBindValue(email);
            query.addBindValue(note);
            query.addBindValue(itemID);
        }else{
            query.prepare(QString("UPDATE %1 SET itemName = ?, type = ?, sex = ?, "
                                  "phone = ?, email = ?, note = ?, avatar = ?"
                                  " WHERE itemID = ?").arg(global::tableAddressItemName));
            query.addBindValue(name);
            query.addBindValue(typeID);
            query.addBindValue(sex);
            query.addBindValue(phone);
            query.addBindValue(email);
            query.addBindValue(note);

            // 40 * 40 used to update database's data
            query.addBindValue(smallAvatarByteArr);
            query.addBindValue(itemID);
        }
        if(!query.exec())
            throw QString(STRING("修改联系人失败") + query.lastError().text());

        // 使用信号的槽的机制更好
        global::hasModifyAddressItem = true;
        global::modifiedName = name;
        global::modifiedType = type;
        global::modifiedSex = sex;
        global::modifiedPhone = phone;
        global::modifiedEmail = email;
        global::modifiedNote = note;
        global::modifiedItemID = itemID;

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}
