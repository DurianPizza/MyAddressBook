// created by Brian
// modified by Brian at 2017.6.12
// * constructor of class AddressBookWindow
// * SQL Script
// * not longer query the number of a specific type in constructor of AddressBookWindow
// * modify on_addItem_triggered function, no need to pay attention to the type is not exist

// * modified by Brian at 2017.6.13
// * 添加了添加类型、删除类型、修改类型的事件处理函数
// * 不在需要为默认分组特别处理（type = NULL）

// * modified by Brian at 2017.6.14
// * 添加了关于对话框
// * 将mainwindow.cpp 对类型名映射到类型ID的代码移动到AddressBookWindow的构造函数中
// * 不再保留PresentSearchResult的指针作为成员
// * 解决了将QVector<QListWidgetItem*>改为QMap<int, QListWidgetItem*>后删除
// *     联系人条目不能正确显示的问题(目前解决方法：重新构建)

// * modified by Brian at 2017.6.15
// * 添加了右键菜单

// * modified by Brian at 2017.6.16
// * 添加了联系人和类型的详细信息面板
// * 完善了联系人的代码
// * 删除了AddressBookWindow的全局指针，改成MainWindow成员指针

#include "AddressBookWindow.h"
#include "ui_AddressBookWindow.h"
#include "AddAddressDialog.h"
#include "AddTypeDialog.h"
#include "ModifyTypeDialog.h"
#include "DeleteTypeDialog.h"
#include "ModifyItemDialog.h"
#include "DeleteItemDialog.h"
#include "AboutDialog.h"
#include "MyMenuItem.h"
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include <QMouseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QLineEdit>
#include <QDebug>

AddressBookItem newItemInfo;

// MainWindow的全局指针
#include "mainwindow.h"
extern MainWindow* mainWindowPtr;
extern QByteArray smallAvatarByteArr;

// parameters
//    parent: parent pointer of this window
//    userName: current user name
// function : intitialize interface and bind the signal with slot.
AddressBookWindow::AddressBookWindow(QWidget *parent, const QString& userName) :
    QMainWindow(parent),
    ui(new Ui::AddressBookWindow),
    userName(userName),
    presentResultWnd(new PresentSearchResult(this)),
    blankMenu(new QMenu()),
    typeMenu(new QMenu()),
    typeNameEdit(new QLineEdit()),
    detailPanel(new ItemDetail()),
    typeDetailPanel(new TypeDetail()),
    timer(new QTimer()),
    isInitializing(true)
{
    ui->setupUi(this);
    //ui->busyProgressBar->hide();

    ui->addressList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //加载QSS
    ui->addressList->setStyleSheet(global::LoadQSSFile(":/QSS/QSS/AddressList.qss"));

    // 初始化右键菜单
    QAction* addTypeMenuItem = new QAction(STRING("添加类型名"), this);
    QAction* modifiedTypeMenuItem = new QAction(STRING("修改类型名"), this);
    QAction* deleteTypeMenuItem = new QAction(STRING("删除类型名"), this);
    QAction* separator = new QAction(this);
    separator->setSeparator(true);
    QAction* addItemMenuItem = new QAction(STRING("添加联系人"), this);
    QAction* searchItemMenuItem = new QAction(STRING("查询联系人"), this);
    QAction* modifiedItemMenuItem = new QAction(STRING("修改联系人"), this);
    QAction* deleteItemMenuItem = new QAction(STRING("删除联系人"), this);

    typeNameEdit->setParent(ui->addressList); // 必须在ui->setupUi(this)后才能使用ui中的指针
    typeNameEdit->hide();

    /*blankMenu->addActions({addTypeMenuItem, modifiedTypeMenuItem, deleteTypeMenuItem,
                          separator, addItemMenuItem, searchItemMenuItem, modifiedItemMenuItem,
                          deleteItemMenuItem});

    typeMenu->addActions({modifiedTypeMenuItem, deleteTypeMenuItem});*/

    QAction* list1[] = {addTypeMenuItem, modifiedTypeMenuItem, deleteTypeMenuItem,
                        separator, addItemMenuItem, searchItemMenuItem, modifiedItemMenuItem,
                        deleteItemMenuItem};
    QList<QAction*> list_1;
    for(auto a :list1)
        list_1.append(a);
    blankMenu->addActions(list_1);

    QAction* list2[] = {modifiedItemMenuItem, deleteItemMenuItem};
    QList<QAction*> list_2;
    for(auto a :list2)
        list_2.append(a);
    typeMenu->addActions(list_2);

    //itemMenu->addActions({modifiedItemMenuItem, deleteItemMenuItem});

    // 绑定自定义槽
    connect(addTypeMenuItem, SIGNAL(triggered()), this, SLOT(on_addType_triggered()));
    connect(modifiedTypeMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestModifyTypeName()));
    connect(deleteTypeMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestDeleteTypeName()));

    connect(addItemMenuItem, SIGNAL(triggered()),
            this, SLOT(on_addItem_triggered()));
    connect(searchItemMenuItem, SIGNAL(triggered()),
            this, SLOT(on_searchItem_triggered()));
    connect(modifiedItemMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestModifyItem()));
    connect(deleteItemMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestDeleteItem()));

    // 按下enter时editingFinished和enterPressed会同时触发，因此槽函数被调用两次
    connect(typeNameEdit, SIGNAL(editingFinished()), this, SLOT(onEditTypeNameFinished()));
    connect(timer, SIGNAL(timeout()), this, SLOT(CheckHasCursorPointToAnyItem()));
    connect(this, SIGNAL(onUserLogout()), mainWindowPtr, SLOT(onUserLogout()));

    connect(this, SIGNAL(addNewType(QString&)), presentResultWnd, SLOT(onAddNewType(QString&)));
    connect(this, SIGNAL(deleteType(QString&)), presentResultWnd, SLOT(onDeleteType(QString&)));

    typeNameEdit->setMaxLength(30);
}

// function : release resource
AddressBookWindow::~AddressBookWindow()
{
    if(timer->isActive())timer->stop();
    delete ui;
}

// function : get data from database and initialize interface.
void AddressBookWindow::PrepareData()
{
    QSqlQuery query(global::db);
    query.prepare(QString("SELECT typeID, typeName, detail FROM %1").arg(global::tableAddressTypeName));
    query.exec();

    while(query.next())
    {
        int typeID = query.value(0).toInt();
        QString typeName = query.value(1).toString();

        // 创建类别名到类别编号的映射
        global::typeNameTotypeID.insert(typeName, typeID);

        QSqlQuery selectTypeCount(global::db);
        auto groupItem = AddGroupItem(typeName, query.value(2).toString());

        // get the content, i.e. the item inside group
        // there is a little confusing, but it is ok
        selectTypeCount.prepare(QString("SELECT itemID, itemName, sex, phone, email, note, avatar FROM %1 "
                                "WHERE type = ?").arg(global::tableAddressItemName));
        selectTypeCount.bindValue(0, typeID);
        selectTypeCount.exec();
        while(selectTypeCount.next())
        {
            try{
                QByteArray imgByteArray= selectTypeCount.value(6).toByteArray();
                QPixmap avatar;
                if(!imgByteArray.isEmpty()){
                    QDataStream ds(imgByteArray);
                    ds >> avatar;
                }
                else
                    avatar = QPixmap(global::defaultAvatarDir);

                AddAddressItem(groupItem,
                               selectTypeCount.value(0).toInt(), // ID
                               selectTypeCount.value(1).toString(), // name
                               query.value(1).toString(), // typeName
                               selectTypeCount.value(2).toString(), // sex
                               selectTypeCount.value(3).toString(), // phone
                               selectTypeCount.value(4).toString(), // email
                               selectTypeCount.value(5).toString(), // note
                               avatar);
            }catch(std::exception e){
                QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }
    timer->start(500);
    isInitializing = false;
}

// function: add an new group type
AddressBookWindow::AddressGroupItem* AddressBookWindow::AddGroupItem(const QString& str, const QString &detail)
{
    AddressGroupItem* newItem = new AddressGroupItem(QIcon(QPixmap(global::downArrowDir)),str, detail);
    newItem->setSizeHint(QSize(this->width(),25));
    // 默认分组特别总放在最前面
    if(str == STRING("默认"))
        ui->addressList->insertItem(0, newItem);
    else
        ui->addressList->addItem(newItem);
    groupList.insert(str, newItem); // 将AddressGroupItem的指针转型为QListWidgetItem的指针
    //groupHiddenList.insert(newItem, false);
    return newItem;
}

// parameters
//     group: the pointer of group item this item belong to
//     ID: the ID in database
//     name: the item name
//     type: the type name this item belong to
//     sex: the sex of the new people
//     phone: the phone of the new people
//     email: the email of the new people
//     note: the detail description of the new item
//     avatar: avatar show in interface
// function: add a new item below the group item, and store the information
//inside the item
void AddressBookWindow::AddAddressItem(AddressGroupItem* group, int ID, const QString& name, const QString& type,
                                       const QString& sex, const QString& phone, const QString& email,
                                       const QString& note, const QPixmap& avatar)
{
    QListWidgetItem* newItem = new QListWidgetItem();
    newItem->setSizeHint(QSize(ui->addressList->width(), 54));
    ui->addressList->insertItem(ui->addressList->row(group) + group->ItemNum() + 1, newItem);
    QWidget* newWidget = new AddressItem(NULL, ID, name, type, sex, phone, email, note, avatar);
    ui->addressList->setItemWidget(newItem, newWidget);

    // 考虑默认组的状态（隐藏或者显示）
    if(group->getStatus())
        newItem->setHidden(true);
    else
        if(!isInitializing && !isDeletingType && !isModifyingItem)
            ui->addressList->setCurrentItem(newItem);
    // bind the newItem to groupItem logically, so that control it easily
    group->AddItem(ID, newItem);
}

// function: add an new type
void AddressBookWindow::on_addType_triggered()
{
    AddTypeDialog* typeDialog = new AddTypeDialog();
    typeDialog->exec();
    delete typeDialog;

    // 这里应该使用信号和槽的机制
    if(global::hasInsertNewType){
        auto newGroup = AddGroupItem(global::newTypeName, global::newTypeDescription);

        // 添加类别名到newTypeItem的映射
        groupList.insert(global::newTypeName, newGroup);
        //groupHiddenList.insert(newGroup, false);

        // 如果查询窗口被打开，那么需要更新其类型下拉列表的内容
        if(!presentResultWnd->isHidden())
            emit addNewType(global::newTypeName);
        global::hasInsertNewType = false;
    }
}

// function: modify an existing type
void AddressBookWindow::on_modifyType_triggered()
{
    try{
        ModifyTypeDialog* modifyTypeDialog = new ModifyTypeDialog();
        modifyTypeDialog->UpdateUI();
        modifyTypeDialog->exec();
        delete modifyTypeDialog;

        // 这里应该使用信号和槽的机制
        if(global::hasModifyTypeName){
            auto ptr = groupList.value(global::originalTypeName);
            // 修改AddressGroupItem储存的groupName字段，以及显示的内容
            ((AddressGroupItem*)ptr)->ResetGroupName(global::modifiedTypeName);
            ((AddressGroupItem*)ptr)->setDescription(global::modifiedDetail);
            // 重新指定key
            groupList.remove(global::originalTypeName);
            groupList.insert(global::modifiedTypeName, ptr);
            global::hasModifyTypeName = false;
            // 修改类型名到ID的映射
            int typeID = global::typeNameTotypeID.value(global::originalTypeName);
            global::typeNameTotypeID.remove(global::originalTypeName);
            global::typeNameTotypeID.insert(global::modifiedTypeName, typeID);
        }
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

// function: deleting an existing type
void AddressBookWindow::on_deleteType_triggered()
{
    try{
        isDeletingType = true;
        DeleteTypeDialog* dialog = new DeleteTypeDialog();
        dialog->exec();
        delete dialog;

        // 这里应该使用信号和槽的机制
        if(global::hasDeleteTypeName){
            auto ptr = (AddressGroupItem*)groupList.value(global::deletedTypeName);
            auto itemList = ptr->getItemList();
            auto defaultType = (AddressGroupItem*)groupList.value(STRING("默认"));
            ui->addressList->takeItem(ui->addressList->row(ptr));
            // 删除类型名到QListWidgetItem*的映射
            groupList.remove(global::deletedTypeName);

            // Qt 不支持移动某个item到另外的位置，因此需要重新构建
            for(auto itemPtr : itemList.values()){
                // 重新构建
                auto widget = (AddressItem*)ui->addressList->itemWidget(itemPtr);
                AddAddressItem(defaultType, widget->getItemID(), widget->getName(), STRING("默认"),
                               widget->getSex(), widget->getPhone(), widget->getEmail(),
                               widget->getNote(), widget->getAvatarImg());

                ui->addressList->takeItem(ui->addressList->row(itemPtr));
            }
            delete ptr;

            // 删除类型名到类型ID的映射
            global::typeNameTotypeID.remove(global::deletedTypeName);

            if(!presentResultWnd->isHidden())
                emit deleteType(global::deletedTypeName);

            global::hasDeleteTypeName = false;
            isDeletingType = false;
        }
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

// function: add an new item
void AddressBookWindow::on_addItem_triggered()
{
    AddAddressDialog* addressDialog = new AddAddressDialog(NULL, userName);
    addressDialog->exec();
    delete addressDialog;

    QPixmap avatar;
    QDataStream ds(&newItemInfo.avatarByteArr, QIODevice::ReadOnly);
    ds >> avatar;

    // 使用信号和槽的机制更好
    if(newItemInfo.succeed)
    {
        auto group = groupList.value(newItemInfo.type);
        AddAddressItem((AddressGroupItem*)group, newItemInfo.itemID, newItemInfo.name,
                       newItemInfo.type, newItemInfo.sex, newItemInfo.phone,
                       newItemInfo.email, newItemInfo.note, avatar.
                       scaled(global::avatarWidth, global::avatarHeight));
    }
    // restore the original value
    newItemInfo.succeed = false;
}

// function: show the search result in an new window
void AddressBookWindow::on_searchItem_triggered()
{
    if(presentResultWnd->isHidden()){
        presentResultWnd->show();
    }
    presentResultWnd->UpdateUI();
}

// function: modify an item
void AddressBookWindow::on_modifyItem_triggered()
{
    try{
        isModifyingItem = true;
        ModifyItemDialog* dialog = new ModifyItemDialog();
        dialog->exec();
        delete dialog;
        if(global::hasModifyAddressItem){
            global::hasModifyAddressItem = false;
            // 如果修改了类型，那么就需要将item从旧的组移动到新的组中
            QListWidgetItem* itemPtr;
            if(QString::compare(global::originalType, global::modifiedType) != 0){
                auto originalGroupPtr = (AddressGroupItem*)groupList.value(global::originalType);
                auto newGroupPtr = (AddressGroupItem*)groupList.value(global::modifiedType);
                itemPtr = originalGroupPtr->getItem(global::modifiedItemID);

                // 删除AddressGroupItem中itemID对QListWidgetItem* 的映射
                originalGroupPtr->RemoveItem(global::modifiedItemID);

                QPixmap avatar;

                // 覆盖位图的图像
                auto widget = (AddressItem*)ui->addressList->itemWidget(itemPtr);
                if(global::hasSelectedNewAvatar){
                    QDataStream ds(&smallAvatarByteArr, QIODevice::ReadOnly);
                    ds >> avatar;
                }else // 不覆盖的情况。
                    avatar = widget->getAvatarImg();

                // 重新创建
                //auto widget = (AddressItem*)ui->addressList->itemWidget(itemPtr);
                AddAddressItem(newGroupPtr, global::modifiedItemID, global::modifiedName, global::modifiedType,
                               global::modifiedSex, global::modifiedPhone, global::modifiedEmail,
                               global::modifiedNote, avatar);

                ui->addressList->takeItem(ui->addressList->row(itemPtr));

            }
            else
            {
                itemPtr = ((AddressGroupItem*)groupList.value(global::modifiedType))
                        ->getItem(global::modifiedItemID);

                QPixmap avatar;
                auto widget = (AddressItem*)ui->addressList->itemWidget(itemPtr);
                if(global::hasSelectedNewAvatar){
                    QDataStream ds(&smallAvatarByteArr, QIODevice::ReadOnly);
                    ds >> avatar;
                }else
                    avatar = widget->getAvatarImg();

                // 重新设置这个item的widget
                QWidget* newWidget = new AddressItem(NULL, global::modifiedItemID,
                                     global::modifiedName, global::modifiedType,
                                     global::modifiedSex, global::modifiedPhone,
                                     global::modifiedEmail, global::modifiedNote,
                                     avatar);

                ui->addressList->setItemWidget(itemPtr, newWidget);
            }
            isModifyingItem = false;
        }
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok, QMessageBox::Ok);
    }
}

// function : called when delete an item
void AddressBookWindow::on_deleteItem_triggered()
{
    try{
        DeleteItemDialog* dialog = new DeleteItemDialog();
        dialog->exec();
        delete dialog;
        if(global::hasDeleteAddressItem){
            global::hasDeleteAddressItem = false;
            auto groupPtr = (AddressGroupItem*)groupList.value(global::deletedItemTypeName);
            auto itemPtr = groupPtr->getItem(global::deletedItemID);
            groupPtr->RemoveItem(global::deletedItemID);
            ui->addressList->takeItem(ui->addressList->row(itemPtr));
        }
    }catch(std::exception e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }
}

// function: called when about menu item is pressed
void AddressBookWindow::on_about_triggered()
{
   AboutDialog* dialog = new AboutDialog();
    dialog->exec();
    delete dialog;
}

// function: called when user logout
//           what's more, this function wiil emit an onUserLogout signal
//           to inform mainwindow to reset the inteface.
void AddressBookWindow::on_logout_triggered()
{
    auto ret = QMessageBox::question(NULL, STRING("确认"), STRING("确认注销当前账户？"),
                                    QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if(ret == QMessageBox::Yes){
        // 清理全局变量
        global::ClearGlobalVaraible();
        this->destroy();
        emit onUserLogout();
        mainWindowPtr->show();
    }
}

// function: called thwn user pressed exit, and close the whole application
void AddressBookWindow::on_exit_triggered()
{
    auto ret = QMessageBox::question(NULL, STRING("确认"), STRING("确认退出？"),
                                    QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    if(ret == QMessageBox::Yes){
        global::ClearGlobalVaraible();
        qApp->exit();
    }
}

// function: called when user choose to destroy this account
void AddressBookWindow::on_destroy_triggered()
{
    auto ret = QMessageBox::critical(NULL, STRING("危险操作"), STRING("确实要删除该账户？"),
                                    QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);

        try{
            if(ret == QMessageBox::Yes){
                //执行删除账户的操作
                QSqlQuery query(global::db);
                query.prepare("DELETE FROM UserInfo WHERE userName = ?");
                query.addBindValue(global::userName);
                if(!query.exec())
                    throw QString(STRING("删除用户失败") + query.lastError().text());

                // AddressItem引用AddressType的typeID,因此先删除AddressItem这个表
                if(!query.exec("DROP TABLE " + global::tableAddressItemName))
                    throw QString(STRING("删除用户失败") + query.lastError().text());
                if(!query.exec("DROP TABLE " + global::tableAddressTypeName))
                    throw QString(STRING("删除用户失败") + query.lastError().text());

                global::ClearGlobalVaraible();

                this->destroy();
                emit onUserLogout();
                mainWindowPtr->show();

            }
        }catch(QString& e){
            QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
        }catch(std::exception& e){
            QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
        }

}

// function: when user press right key, the function will be called
// note: 为处理自定义菜单事件，QWidget的contextMenuPolicy须设置为Qt::CustomContextMenu
void AddressBookWindow::on_addressList_customContextMenuRequested(const QPoint &pos)
{
    try{

        //if(currentEditingTypeItem)
         //   onEditTypeNameFinished();

        auto itemPointTo = ui->addressList->itemAt(pos);
        auto selectedItem = ui->addressList->currentItem();
        // 多次调用takeItem以及insertItem之后itemAt就会返回错误的结果，原因未明
        // 为此使用了currentItem代替，但这是空白处的菜单就会显示不出来
        // 因此当itemAt返回为空的时候就用空值替代selectedItem原来的值

        if(itemPointTo == NULL)
            selectedItem = NULL;
        if(groupList.values().contains(selectedItem)){
            if(((AddressGroupItem*)selectedItem)->getTypeName() == STRING("默认"))
                return;
            customMenuRequestItem = selectedItem;
            typeMenu->exec(QCursor::pos());
            return;
        }
        for(auto typePtr : groupList.values()){
            if(((AddressGroupItem*)typePtr)->ContainItem(selectedItem)){
                customMenuRequestItem = selectedItem;
                //itemMenu->exec(QCursor::pos());
                GenerateItemCustomMenu(selectedItem);
                return;
            }
        }
        customMenuRequestItem = NULL;
        blankMenu->exec(QCursor::pos());
    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception& e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }
}

// function: when item in QListWidget is pressed(left key), this function is called
//           use this function to show/hide a group's sub item,
//           or close the line edit of modifying type name.
void AddressBookWindow::on_addressList_itemClicked(QListWidgetItem *item)
{
    if(groupList.values().contains(item)){
        ((AddressGroupItem*)item)->onStatusChanged();
    }

    if(currentEditingTypeItem)
        onEditTypeNameFinished();
}

// functon: called when cursor enter an item
//          use this slot to show the floating information window
//          of specific group or item
void AddressBookWindow::on_addressList_itemEntered(QListWidgetItem *item)
{
    if(groupList.values().contains(item)){
        if(!detailPanel->isHidden())detailPanel->hide();
        auto typePtr = (AddressGroupItem*)item;
        typeDetailPanel->setPresentTypeName(typePtr->getTypeName());
        typeDetailPanel->setPresentDescription(typePtr->getDescription());
        auto geometry = ui->addressList->visualItemRect(item);
        auto point = QPoint(geometry.x(), geometry.y());
        auto globalPoint = ui->addressList->mapToGlobal(point);
        typeDetailPanel->setGeometry(globalPoint.x() - typeDetailPanel->width(),
                                     globalPoint.y(),
                                     typeDetailPanel->width(),
                                     typeDetailPanel->height());

        if(typeDetailPanel->isHidden())
            typeDetailPanel->show();
        return;
    }
    else
    {
        if(!typeDetailPanel->isHidden())typeDetailPanel->hide();

        for(auto typePtr : groupList.values()){
            if(((AddressGroupItem*)typePtr)->ContainItem(item)){

                AddressItem* widget = (AddressItem*)ui->addressList->itemWidget(item);
                detailPanel->setPresentName(widget->getName());
                detailPanel->setPresentType(widget->getType());
                detailPanel->setPresentSex(widget->getSex());
                detailPanel->setPresentPhone(widget->getPhone());
                detailPanel->setPresentEmail(widget->getEmail());
                detailPanel->setPresentNote(widget->getNote());
                detailPanel->setPresentAvatar(widget->getAvatarImg());
                auto geometry = ui->addressList->visualItemRect(item);
                auto point = QPoint(geometry.x(), geometry.y());
                auto globalPoint = ui->addressList->mapToGlobal(point);
                detailPanel->setGeometry(globalPoint.x() - detailPanel->width(),
                                                 globalPoint.y(),
                                                 detailPanel->width(),
                                                 detailPanel->height());

                if(detailPanel->isHidden())
                    detailPanel->show();
                return;
            }
        }
    }
}

// function: called when user press the menu item "modify type" of custom menu.
void AddressBookWindow::onCustomMenuRequestModifyTypeName()
{
    if(!customMenuRequestItem)
        on_modifyType_triggered();
    else{
        // 显示编辑类型的line edit
        if(((AddressGroupItem*)customMenuRequestItem)->getTypeName() == STRING("默认"))
            return;

        currentEditingTypeItem = customMenuRequestItem;
        typeNameEdit->raise();
        //出现的位置
        typeNameEdit->setGeometry(ui->addressList->visualItemRect(customMenuRequestItem));
        typeNameEdit->setText(((AddressGroupItem*)customMenuRequestItem)->getTypeName());
        typeNameEdit->show();
        typeNameEdit->selectAll();
        typeNameEdit->setFocus();
    }
}

// function: called when user press the menu item "delete type" of custom menu.
void AddressBookWindow::onCustomMenuRequestDeleteTypeName()
{
    if(!customMenuRequestItem)
        on_deleteType_triggered();
    else{ // 直接删除
        try{
            isDeletingType = true;
            auto deletedItem = (AddressGroupItem*)customMenuRequestItem;
            auto deletedTypeName = deletedItem->getTypeName();
            if(deletedTypeName == STRING("默认"))
                return;

            // 这里的代码应该能够重用
            int defaultTypeID = global::typeNameTotypeID.value(STRING("默认"));
            int referencedTypeID = global::typeNameTotypeID.value(deletedTypeName);
            QSqlQuery query(global::db);
            query.prepare(QString("UPDATE %1 SET type = ? WHERE type = ?").arg(global::tableAddressItemName));
            query.addBindValue(defaultTypeID);
            query.addBindValue(referencedTypeID);

            if(!query.exec())
                throw QString(STRING("删除联系人类别失败：") + query.lastError().text());

            query.prepare(QString("DELETE FROM %1 WHERE typeName = ?").arg(global::tableAddressTypeName));
            query.addBindValue(deletedTypeName);
            if(!query.exec())
                throw QString(STRING("删除联系人类别失败：") + query.lastError().text());

            auto itemList = deletedItem->getItemList();
            auto defaultType = (AddressGroupItem*)groupList.value(STRING("默认"));
            ui->addressList->takeItem(ui->addressList->row(deletedItem));
            // 删除类型名到QListWidgetItem*的映射
            groupList.remove(deletedTypeName);

            for(auto itemPtr : itemList.values()){
                // 重新构建
                auto widget = (AddressItem*)ui->addressList->itemWidget(itemPtr);
                AddAddressItem(defaultType, widget->getItemID(),widget->getName(), STRING("默认"),
                               widget->getSex(), widget->getPhone(), widget->getEmail(),
                               widget->getNote(), widget->getAvatarImg());

                ui->addressList->takeItem(ui->addressList->row(itemPtr));
                delete itemPtr;
            }
            delete deletedItem;

            // 删除类型名到类型ID的映射
            global::typeNameTotypeID.remove(deletedTypeName);

            if(!presentResultWnd->isHidden())
                emit deleteType(deletedTypeName);

            customMenuRequestItem = NULL;
            isDeletingType = false;
        }catch(QString& e){
            QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
        }catch(std::exception& e){
            QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
        }
    }
}

// function: called when user press the menu item "modify item" of custom menu.
void AddressBookWindow::onCustomMenuRequestModifyItem()
{
    if(!customMenuRequestItem){
        global::specificItemID = -1;
        on_modifyItem_triggered();
    }
    else{
        try{
            // 使用信号和槽更加好
            auto widget = (AddressItem*)ui->addressList->itemWidget(customMenuRequestItem);
            global::specificItemID = widget->getItemID();
            on_modifyItem_triggered();
        }catch(QString& e){
            QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
        }catch(std::exception& e){
            QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
        }
    }
}

// function: called when user press the menu item "delete item" of custom menu.
void AddressBookWindow::onCustomMenuRequestDeleteItem()
{
    if(!customMenuRequestItem)
        on_deleteItem_triggered();
    else{
        try{
            // 使用信号和槽更加好
            auto widget = (AddressItem*)ui->addressList->itemWidget(customMenuRequestItem);
            int itemID = widget->getItemID();

            QSqlQuery query(global::db);
            query.prepare(QString("DELETE FROM %1 WHERE itemID = ?").arg(global::tableAddressItemName));
            query.addBindValue(itemID);
            if(!query.exec())
                throw QString(STRING("删除联系人失败：") + query.lastError().text());

            auto groupPtr = (AddressGroupItem*)groupList.value(widget->getType());
            groupPtr->RemoveItem(itemID);
            ui->addressList->takeItem(ui->addressList->row(customMenuRequestItem));
            customMenuRequestItem = NULL;
        }catch(QString& e){
            QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
        }catch(std::exception& e){
            QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
        }
    }
}

// function: called when user finish edit the type name.
// note: 类型名编辑完成槽
void AddressBookWindow::onEditTypeNameFinished()
{
    try{
        typeNameEdit->hide();

        if(!currentEditingTypeItem)
            throw "";

        auto newTypeName = typeNameEdit->text();
        if(newTypeName.isEmpty())return;
        auto originalTypeName = ((AddressGroupItem*)currentEditingTypeItem)->getTypeName();
        if(originalTypeName == newTypeName)
            return;
        ((AddressGroupItem*)currentEditingTypeItem)->setTypeName(typeNameEdit->text());

        QSqlQuery query(global::db);
        query.prepare(QString("UPDATE %1 SET typeName = ? WHERE typeName = ?")
                      .arg(global::tableAddressTypeName));
        query.addBindValue(newTypeName);
        query.addBindValue(originalTypeName);

        if(!query.exec())
            throw QString(STRING("修改类型名失败 ") + query.lastError().text());

        // 重新映射
        groupList.remove(originalTypeName);
        groupList.insert(newTypeName, currentEditingTypeItem);
        int typeID = global::typeNameTotypeID.value(originalTypeName);
        global::typeNameTotypeID.remove(originalTypeName);
        global::typeNameTotypeID.insert(newTypeName, typeID);

        if(!presentResultWnd->isHidden())
            emit addNewType(newTypeName);

        currentEditingTypeItem = NULL;

    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception& e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }catch(...){
        // 不用处理的异常
    }
}

// function: called by tiemr to hide the detail panel
// when cursor didn't point to any item
void AddressBookWindow::CheckHasCursorPointToAnyItem()
{
    try{
        auto item = ui->addressList->itemAt(ui->addressList->mapFromGlobal(QCursor::pos()));
        if(!groupList.values().contains(item)){
            if(!typeDetailPanel->isHidden())
                typeDetailPanel->hide();
        }
        bool isAddressItem = false;
        for(auto typePtr : groupList.values()){
            if(((AddressGroupItem*)typePtr)->ContainItem(item)){
                isAddressItem = true;
                break;
            }
        }
        if(!isAddressItem)
            if(!detailPanel->isHidden())
                detailPanel->hide();
    }catch(std::exception& e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }
    catch(...){
        QMessageBox::warning(NULL, "warning", STRING("发生错误"), QMessageBox::Ok);
    }
}

// function: called when received the self defination signal onTriggered(QString)
//           this will figure out which menu item user has choose
void AddressBookWindow::onMoveToSubMenuTriggered(QString typeName)
{
    try{
        isModifyingItem = true;
        if(!customMenuRequestItem)
            throw QString(STRING("移动联系人失败"));
        auto widget = (AddressItem*)ui->addressList->itemWidget(customMenuRequestItem);
        int originalTypeID = global::typeNameTotypeID.value(widget->getType());
        int newTypeID = global::typeNameTotypeID.value(typeName);

        if(originalTypeID == newTypeID){
            customMenuRequestItem = NULL;
            isModifyingItem = false;
            return;
        }

        auto originalTypePtr = (AddressGroupItem*)groupList.value(widget->getType());
        auto newTypePtr = (AddressGroupItem*)groupList.value(typeName);

        QSqlQuery query(global::db);
        query.prepare(QString("UPDATE %1 SET type = ? WHERE type = ? AND itemID = ?").arg(global::tableAddressItemName));
        query.addBindValue(newTypeID);
        query.addBindValue(originalTypeID);
        query.addBindValue(widget->getItemID());

        if(!query.exec())
            throw QString(STRING("移动联系人失败：") + query.lastError().text());

        AddAddressItem(newTypePtr, widget->getItemID(), widget->getName(), typeName,
                       widget->getSex(), widget->getPhone(), widget->getEmail(),
                       widget->getNote(), widget->getAvatarImg());

        ui->addressList->takeItem(ui->addressList->row(customMenuRequestItem));
        originalTypePtr->RemoveItem(widget->getItemID());
        customMenuRequestItem = NULL;
        isModifyingItem = false;
    }catch(QString& e){
        QMessageBox::warning(NULL, "warning", e, QMessageBox::Ok);
    }catch(std::exception& e){
        QMessageBox::warning(NULL, "warning", e.what(), QMessageBox::Ok);
    }

}

// function: according to the type of address item to
//           generate the unique custom menu
void AddressBookWindow::GenerateItemCustomMenu(QListWidgetItem* selectedItem)
{
    auto widget = (AddressItem*)ui->addressList->itemWidget(selectedItem);
    auto groupName = widget->getType();
    QMenu* itemMenu = new QMenu();
    QAction* seperator = new QAction(this);
    seperator->setSeparator(true);
    QMenu* moveToSubMenu = new QMenu();
    for(auto typeName : global::typeNameTotypeID.keys())
    {
        if(typeName != groupName){
            MyMenuItem* action = new MyMenuItem(typeName);
            connect(action, SIGNAL(onTriggered(QString)), this, SLOT(onMoveToSubMenuTriggered(QString)));
            moveToSubMenu->addAction(action);
        }
    }
    moveToSubMenu->setTitle(STRING("移动到"));

    QAction* modifiedItemMenuItem = new QAction(STRING("修改联系人"), this);
    QAction* deleteItemMenuItem = new QAction(STRING("删除联系人"), this);
    connect(modifiedItemMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestModifyItem()));
    connect(deleteItemMenuItem, SIGNAL(triggered()),
            this, SLOT(onCustomMenuRequestDeleteItem()));

    itemMenu->addMenu(moveToSubMenu);
    //itemMenu->addActions({seperator, modifiedItemMenuItem, deleteItemMenuItem});
    QAction* list2[] = {seperator, modifiedItemMenuItem, deleteItemMenuItem};
    QList<QAction*> list_2;
    for(auto a :list2)
        list_2.append(a);
    itemMenu->addActions(list_2);
    itemMenu->exec(QCursor::pos());
}
