// modified by Brian at 2017.6.12
// * modify the SQL script
// modified by Brian at 2017.6.13
// * 添加了具体的查询逻辑
// modified by Brian at 2017.6.14
// * 不再在构造函数中查找所有的类型名，而使用全局变量中储存的类型名，以加快启动时间
// * 将更新UI的部分放在UpdateUI函数里

#include "PresentSearchResult.h"
#include "ui_PresentSearchResult.h"
#include "stdafx.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

// function: setup the ui
PresentSearchResult::PresentSearchResult(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PresentSearchResult)
{
    ui->setupUi(this);
}

PresentSearchResult::~PresentSearchResult()
{
    delete ui;
}

// function: called when user press the search button
//           this function implement present data
//           according to random combination of search condition
void PresentSearchResult::on_search_clicked()
{
    try{
        QString name = ui->lineEditName->text();
        QString sex = ui->comboBoxSex->currentText();
        QString type = ui->comboBoxGroup->currentText();

        QString SQL = QString(global::baseQuerySQL)
                .arg(STRING("编号")).arg(STRING("姓名")).arg(STRING("性别"))
                .arg(STRING("电话")).arg(STRING("邮箱")).arg(STRING("联系人类别"))
                .arg(global::tableAddressItemName).arg(global::tableAddressTypeName)
                .arg(global::tableAddressItemName).arg(global::tableAddressTypeName);

        if(!name.isEmpty())
            SQL += global::nameConditionSQL;
        if(!sex.isEmpty())
            SQL += global::sexConditionSQL;
        if(!type.isEmpty())
            SQL += global::typeConditionSQL;

        QSqlQuery query(global::db);
        query.prepare(SQL);
        if(!name.isEmpty())
            query.addBindValue(name);
        if(!sex.isEmpty())
            query.addBindValue(sex);
        if(!type.isEmpty())
            query.addBindValue(type);

        // 显示结果
        query.exec();
        QSqlQueryModel* model = new QSqlQueryModel(ui->tableView);
        model->setQuery(query);
        ui->tableView->setModel(model);

    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

// function: called by AddressBookWindow on_searchItem_triggered function
//           to initialize the interface
void PresentSearchResult::UpdateUI()
{
    ui->comboBoxGroup->clear();
    ui->comboBoxGroup->addItem(""); // "" means select all the group
    for(auto key : global::typeNameTotypeID.keys())
        ui->comboBoxGroup->addItem(key);

    QSqlQuery query(global::db);
    query.prepare(QString(global::baseQuerySQL)
                  .arg(STRING("编号")).arg(STRING("姓名")).arg(STRING("性别"))
                  .arg(STRING("电话")).arg(STRING("邮箱")).arg(STRING("联系人类别"))
                  .arg(global::tableAddressItemName).arg(global::tableAddressTypeName)
                  .arg(global::tableAddressItemName).arg(global::tableAddressTypeName));
    query.exec();
    QSqlQueryModel* model = new QSqlQueryModel(ui->tableView);
    model->setQuery(query);
    ui->tableView->setModel(model);
}

// function: to update the comboxGroup's content when user add an new type
void PresentSearchResult::onAddNewType(QString& newType)
{
    ui->comboBoxGroup->addItem(newType);
}

// function: to update the comboxGroup's content when user delete a type
void PresentSearchResult::onDeleteType(QString &deletedType)
{
    int index = ui->comboBoxGroup->findText(deletedType);
    ui->comboBoxGroup->removeItem(index);
}
