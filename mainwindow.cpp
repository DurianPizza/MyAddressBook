// modified by Brian at 2017.6.12
// * add a map from typeName to typeID

// modified by Brian at 2017.6.13
// * 将对类型名映射到类型ID的代码移动到AddressBookWindow.cpp的构造函数中

// modified by Brian at 2017.6.14
// * 不再需要储存AddressBookWindow的指针作为成员变量

// modified by Brian at 2017.6.15
// * 将数据库对象变成全局变量
// * 添加对无法连接到服务器的逻辑的处理
// * 将连接数据库服务器的代码放在另外一个线程里执行

// modified by Brian at 2017.6.16
// * 将密码验证改为SHA-256验证
// * 将输入用户名的LineEdit改成edittable的comboBox
// * 将AddressBookWindow的全局指针改成成员变量

// modified by Brian at 2017.6.18
// * 将进度条改为通过界面编辑器实现

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdafx.h"
#include "RegisterDialog.h"
#include "threadcontroller.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QComboBox>

// 保留MainWindow的全局指针
MainWindow* mainWindowPtr;

// function: open the sqlite database and load the username and the
//           hash code of password, while initialize the interface
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    addressBookWnd(0)
{
    ui->setupUi(this);
    ui->busyProgressBar->hide();

    mainWindowPtr = this;
    global::controller = new Controller();
    //connect(global::controller, &global::controller->connectFinish(), this, SLOT(logIn));
    connect(global::controller, SIGNAL(onLogInFeedback()), this, SLOT(logIn()));

    try{
        sqliteDatabase = QSqlDatabase::addDatabase("QSQLITE", "SQLiteConnection");
        sqliteDatabase.setDatabaseName("record");
        if(!sqliteDatabase.open())
            throw QString(STRING("打开SQLite数据库失败"));

        QSqlQuery query(sqliteDatabase);
        query.exec("SELECT id, userName, password, passwordLen FROM record");
        if(query.lastError().type() != QSqlError::ErrorType::NoError)
            throw QString(STRING("查询失败： ") + query.lastError().text());
        while(query.next()){
            QString decryptUserName = global::Decrypt(query.value(1).toByteArray());
            PasswordHashCode* hashCode = new PasswordHashCode();
            hashCode->recordID = query.value(0).toInt();
            hashCode->hashCode = query.value(2).toByteArray();
            hashCode->passwordLen = query.value(3).toInt();
            passwordHashCodes.insert(decryptUserName, hashCode);

            // 向comboBox插入项也可能会触发onCurrentIndexChanged信号
            // 因此先插入记录，在处理信号的槽函数中查询记录就不会出错
            ui->userNameComboBox->addItem(decryptUserName);
        }
    }catch(QString& e){
        QMessageBox::warning(NULL, "Info", e, QMessageBox::Ok);
    }catch(std::exception& e){
        QMessageBox::warning(NULL, "Info", e.what(), QMessageBox::Ok);
    }
}

// function: release resource and close the connection to sqlite database
MainWindow::~MainWindow()
{
    for(auto ptr : passwordHashCodes)
        delete ptr;
    if(sqliteDatabase.isOpen())
        sqliteDatabase.close();
    delete ui;
}

// function: try to connect to database in another thread
void MainWindow::on_login_clicked()
{
    global::loginHasPressed = true;
    auto geometry = ui->busyProgressBar->geometry();
    auto wndGeometry = this->geometry();

    // 调整progressBar的位置
    QPoint progressBarPos = QPoint((wndGeometry.width() - geometry.width()) / 2,
                                   (wndGeometry.height() - geometry.height()) / 2);

    ui->busyProgressBar->setGeometry(progressBarPos.x(),
                                     progressBarPos.y(),
                                     geometry.width(), geometry.height());

    ui->busyProgressBar->show();
    // 发出信号启动线程
    emit global::controller->onTryToConnectServer();
}

// function: open the register window
void MainWindow::on_regist_clicked()
{
    RegisterDialog* registerWindow = new RegisterDialog(NULL);
    registerWindow->exec();
    delete registerWindow;
}

// function: show the help information
void MainWindow::on_forget_clicked()
{
    QMessageBox::warning(NULL, "Info", STRING("请联系管理员：灵虚\nCrazyMan\nL·F·Flower"), QMessageBox::Ok);
}

// function: when user input new username, the check box should be uncheck
void MainWindow::on_userNameComboBox_currentTextChanged(const QString &arg1)
{
    // 为了在用户输入新账号的时候将密码输入框设置为空
    // 以及将checkBox设置为未选中的状态
    if(!passwordHashCodes.keys().contains(arg1)){
        ui->passwordLineEdit->setText("");
        ui->savePasswordCheckBox->setChecked(false);
        return;
    }
}

// function: set the number of placeholder in password line edit
//           when clear record, currentIndexChanged will be triggered,
//           this time should set the password line edit to blank
//           and uncheck the checkbox
void MainWindow::on_userNameComboBox_currentIndexChanged(const QString &arg1)
{
    // 为了在清空所有记录的时候将密码输入框设置为空
    // 以及将checkBox设置为未选中的状态
    if(!passwordHashCodes.keys().contains(arg1)){
        ui->passwordLineEdit->setText("");
        ui->savePasswordCheckBox->setChecked(false);
        return;
    }

    char placeHolder[21] = "                    ";
    int passwordLen = passwordHashCodes.value(arg1)->passwordLen;
    placeHolder[passwordLen] = 0;
    if(passwordLen > 0)
        ui->savePasswordCheckBox->setChecked(true);
    else
        ui->savePasswordCheckBox->setChecked(false);
    ui->passwordLineEdit->setText(placeHolder);
}

// function: when user uncheck the checkbox, the password line edit
//           should be clear
void MainWindow::on_savePasswordCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Unchecked){
        ui->passwordLineEdit->setText("");
        auto ptr = passwordHashCodes.value(ui->userNameComboBox->currentText());
        if(!ptr) return;
        ptr->passwordLen = 0;
        ptr->hashCode.clear();
    }
}

// function: called when signal onLogInFeedback is received
//           the connection to database may succeed or failed
void MainWindow::logIn()
{
    QString userName,password;
    try
    {
        //busyProgressBar->close();
        if(!global::connectToDatabase)
            throw QString(STRING("无法连接到服务器"));

        userName = ui->userNameComboBox->currentText();
        password = ui->passwordLineEdit->text();
        if(userName == "")
            throw std::runtime_error(STRING("用户名不能为空"));
        if(password == "")
            throw std::runtime_error(STRING("密码不能为空"));

        QByteArray hashCode;
        if(passwordHashCodes.keys().contains(userName) &&
                passwordHashCodes.value(userName)->passwordLen != 0){
            hashCode = passwordHashCodes.value(userName)->hashCode;
        }else{
            hashCode = QCryptographicHash::hash(QByteArray(password.toStdString().c_str()),
                                                QCryptographicHash::Algorithm::Sha256);
        }

        // 将hashCode转换成16进制字符串进行比较
        // 当然也可以获取密码的哈希码到本地再来比较
        auto hashCodeStr = global::getHexString(hashCode);
        QSqlQuery query(global::db);
        query.prepare(QString("SELECT * FROM UserInfo WHERE userName = ? AND password = %1").arg(hashCodeStr));
        query.addBindValue(userName);
        query.exec();
        if(!query.next())
        {
            throw std::runtime_error(STRING("用户名或密码错误"));
        }

        // 保存数据到SQLite数据库
        if(ui->savePasswordCheckBox->isChecked()){
            try{
                // 已经存在记录的情况
                if(passwordHashCodes.keys().contains(userName))
                {
                    auto ptr = passwordHashCodes.value(userName);
                    int recordID = ptr->recordID;
                    QSqlQuery query(sqliteDatabase);
                    query.prepare("UPDATE record SET password = ?, "
                                  "passwordLen = ? WHERE id = ?");
                    query.addBindValue(hashCode);
                    query.addBindValue(password.length());
                    query.addBindValue(recordID);
                    query.exec();
                    if(!query.exec())
                        throw QString(STRING("更新SQLite数据库失败: ") + query.lastError().text());

                    // 更新数据
                    ptr->hashCode = hashCode;
                    ptr->passwordLen = password.length();
                }
                else
                {
                    QByteArray encryptUserName = global::Encrypt(userName);
                    int passwordLength = password.length();
                    QSqlQuery query(sqliteDatabase);
                    query.prepare("INSERT INTO record(userName, password, passwordLen) "
                                  "VALUES(?, ?, ?)");
                    query.addBindValue(encryptUserName);
                    query.addBindValue(hashCode);
                    query.addBindValue(passwordLength);
                    if(!query.exec())
                        throw QString(STRING("插入SQLite数据库失败: ") + query.lastError().text());

                    PasswordHashCode* record = new PasswordHashCode();
                    query.exec("SELECT MAX(id) FROM record");
                    query.next();
                    record->recordID = query.value(0).toInt();
                    record->hashCode = hashCode;
                    record->passwordLen = password.length();
                    passwordHashCodes.insert(userName, record);
                    ui->userNameComboBox->addItem(userName);
                }
            }catch(QString& e){
                QMessageBox::warning(NULL, "Info", e, QMessageBox::Ok);
            }catch(std::exception& e){
                QMessageBox::warning(NULL, "Info", e.what(), QMessageBox::Ok);
            }
        }
        else{
            if(passwordHashCodes.keys().contains(userName))
            {
                try{
                    auto ptr = passwordHashCodes.value(userName);
                    int recordID = ptr->recordID;
                    QSqlQuery query(sqliteDatabase);
                    query.prepare("UPDATE record SET password = NULL, "
                                  "passwordLen = 0 WHERE id = ?");
                    query.addBindValue(recordID);
                    query.exec();
                    if(!query.exec())
                        throw QString(STRING("更新SQLite数据库失败: ") + query.lastError().text());

                    // 更新数据
                    ptr->hashCode.clear();
                    ptr->passwordLen = 0;

                }catch(QString& e){
                    QMessageBox::warning(NULL, "Info", e, QMessageBox::Ok);
                }catch(std::exception& e){
                    QMessageBox::warning(NULL, "Info", e.what(), QMessageBox::Ok);
                }

            }
            else
            {
                try{
                    QByteArray encryptUserName = global::Encrypt(userName);
                    QSqlQuery query(sqliteDatabase);
                    query.prepare("INSERT INTO record(userName, passwordLen) "
                                                      "VALUES(?, ?)");
                    query.addBindValue(encryptUserName);
                    query.addBindValue(QVariant(0));
                    if(!query.exec())
                        throw QString(STRING("插入SQLite数据库失败: ") + query.lastError().text());

                    PasswordHashCode* record = new PasswordHashCode();
                    query.exec("SELECT MAX(id) FROM record");
                    query.next();
                    record->recordID = query.value(0).toInt();
                    record->passwordLen = 0;
                    passwordHashCodes.insert(userName, record);
                    ui->userNameComboBox->addItem(userName);
                }catch(QString& e){
                    QMessageBox::warning(NULL, "Info", e, QMessageBox::Ok);
                }catch(std::exception& e){
                    QMessageBox::warning(NULL, "Info", e.what(), QMessageBox::Ok);
                }
            }
        }

        this->hide();
        ui->busyProgressBar->hide();

        //初始化用户的类型表名以及联系人表名
        global::userName = userName;
        global::tableAddressTypeName = "AddressType_" + userName;
        global::tableAddressItemName = "AddressItem_" + userName;

        addressBookWnd = new AddressBookWindow(NULL, userName);
        addressBookWnd->show();
        addressBookWnd->PrepareData();
    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }
    ui->busyProgressBar->hide();
}

// function: when user logout, the function will be called
//           this function just reset the interface after user logout
void MainWindow::onUserLogout()
{
    QString userName = ui->userNameComboBox->currentText();
    if(passwordHashCodes.keys().contains(userName))
    {
        PasswordHashCode* ptr = passwordHashCodes.value(userName);
        char placeHolder[21] = "                    ";
        placeHolder[ptr->passwordLen] = 0;
        ui->passwordLineEdit->setText(placeHolder);
    }
    else
    {
        ui->userNameComboBox->setCurrentText("");
        ui->passwordLineEdit->setText("");
    }
}

// function: when user choose delete current record
//           this function just remove the record from sqlite database
void MainWindow::on_deleteRecord_triggered()
{
    try
    {
        QString userName = ui->userNameComboBox->currentText();
        int itemIndex = ui->userNameComboBox->findText(userName);
        if(itemIndex == -1)return;

        int keyID = passwordHashCodes.value(userName)->recordID;
        QSqlQuery query(sqliteDatabase);
        query.prepare("DELETE FROM record WHERE id = ?");
        query.addBindValue(keyID);
        query.exec();
        if(!query.exec())
            throw QString(STRING("删除当前记录失败"));
        ui->userNameComboBox->removeItem(itemIndex);

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }

}

// function: called when user choose to clear all record
void MainWindow::on_clearAllRecord_triggered()
{
    try
    {
        QSqlQuery query(sqliteDatabase);
        if(!query.exec("DELETE FROM record"))
            throw QString(STRING("清除记录失败"));

        for(auto key : passwordHashCodes.keys()){
            auto keyIndex = ui->userNameComboBox->findText(key);
            ui->userNameComboBox->removeItem(keyIndex);
        }
        passwordHashCodes.clear();
    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }
}
