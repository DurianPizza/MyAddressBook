// modified by Brian at 2017.6.12
// * create new table AddressItem_UserName AddressType_UserName
// modified by Brian at 2017.6.13
// * 将默认分组添加到数据库中
// modified by Brian at 2017.6.14
// * 对密码进行SHA-256处理

#include "RegisterDialog.h"
#include "ui_RegisterDialog.h"
#include "stdafx.h"
#include <exception>
#include <stdexcept>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

// function: setup the interface and bind signal with slot
RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    this->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/Dialog.qss"));

    connect(global::controller, SIGNAL(onRegisterFeedback()),
            this, SLOT(onRegister()));
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

// function: called when user press the "ok" button
//           to insert new data entry to database
void RegisterDialog::on_buttonBox_accepted()
{
    try
    {
        // 改进：在线程中打开数据库的连接
        if(!global::connectToDatabase)
            global::connectToDatabase = global::OpenDatabase();

        if(!global::connectToDatabase)
            throw std::runtime_error(STRING("无法连接到服务器"));

        QString userName = ui->userNameLineEdit->text();
        QString password = ui->passwordLineEdit->text();
        QString ensure = ui->ensureLineEdit->text();
        if(userName == "")
            throw std::runtime_error(STRING("用户名不能为空"));
            //QMessageBox::warning(NULL, "warning", STRING("用户名不能为空"), QMessageBox::Ok, QMessageBox::Ok);
        if(password == "")
            throw std::runtime_error(STRING("密码不能为空"));
        if(ensure == "")
            throw std::runtime_error(STRING("确认密码不能为空"));
        if(password != ensure)
            throw std::runtime_error(STRING("密码和确认密码不一致"));

        QByteArray hashCode = QCryptographicHash::hash(QByteArray(password.toStdString().c_str()),
                                              QCryptographicHash::Algorithm::Sha256);

        QSqlQuery query(global::db);
        query.prepare("INSERT INTO UserInfo (userName, password) VALUES(?, ?)");
        query.addBindValue(userName);
        query.addBindValue(hashCode);
        bool result = query.exec();

        if(!result)
            throw std::runtime_error(query.lastError().text().toStdString());

        result = query.exec(QString(global::createAddressTypeSQL).arg(userName));
        if(!result)
            throw std::runtime_error(query.lastError().text().toStdString());

        result = query.exec(QString(global::createAddressItemSQL)
                            .arg(userName).arg(userName)
                            .arg(STRING("男")).arg(STRING("女")));
        if(!result)
            throw std::runtime_error(STRING("注册失败：") + query.lastError().text().toStdString());

        result = query.exec(QString(global::createDefaultTypeSQL).arg(userName)
                            .arg(STRING("默认")).arg(STRING("系统创建的分组，不能删除")));
        if(!result)
            throw std::runtime_error(STRING("注册失败：") + query.lastError().text().toStdString());
    }
    catch(std::exception e)
    {
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

void RegisterDialog::on_buttonBox_rejected()
{

}

void RegisterDialog::onRegister()
{

}
