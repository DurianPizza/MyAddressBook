// modified by Brian at 2017.6.12
// * constructor of AddressGroupItem
// modified by Brian at 2017.6.13
// * 向AddressItem类添加了成员itemID, itemType, itemNote成员
// * 修改了AddressItem类构造函数以及AddAddressItem的定义
// * 将AddressGroupItem的Vector改成QMap<int, QListWidgetItem*>,
// *   并修改了AddItem和setItemHidden等函数的定义
// * 在类AddressGroupItem中添加了getItem函数
// * 在类AddressItem添加了UpdateItem函数

// modified by Brian at 2017.6.14
// * 不在通过QMap<QListWidgetItem*, bool> groupHiddenList 来维护组的状态
// *    而使用内部变量

// modified by Brian at 2017.6.15
// * 启用了mouseTracking

// modified by Brian at 2017.6.18
// * 将构造函数中从数据库获取数据的代码放到PrepareData函数中

// modified by Brian at 2017.6.19
// * 右键点击联系人弹出的菜单添加了移动到子菜单，删除了成员指针itemMenu
#ifndef ADDRESSBOOKWINDOW_H
#define ADDRESSBOOKWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <QMap>
#include <QLabel>
#include <QEvent>
#include <QPainter>
#include <QTimer>
#include "PresentSearchResult.h"
#include "ItemDetail.h"
#include "TypeDetail.h"
#include "stdafx.h"

namespace Ui {
class AddressBookWindow;
}

class AddressBookWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddressBookWindow(QWidget *parent, const QString& userName);
    ~AddressBookWindow();

    // 为加快界面启动速度，在构造函数之外从数据库获取数据
    void PrepareData();

    // 自定义信号
signals:
    void onUserLogout();
    void addNewType(QString& newType);
    void deleteType(QString& deletedType);

private slots:
    void on_addItem_triggered();

    void on_searchItem_triggered();

    void on_addType_triggered();

    void on_modifyType_triggered();

    void on_deleteType_triggered();

    void on_modifyItem_triggered();

    void on_deleteItem_triggered();

    void on_about_triggered();

    void on_logout_triggered();

    void on_exit_triggered();

    void on_destroy_triggered();

    void on_addressList_customContextMenuRequested(const QPoint &pos);

    void on_addressList_itemClicked(QListWidgetItem *item);

    /// 要启用这个槽，必须设置mousceTracking选项
    /// 自定义的Item也必须设置
    void on_addressList_itemEntered(QListWidgetItem *item);

    // 自定义槽
    void onEditTypeNameFinished();
    void onCustomMenuRequestModifyTypeName();
    void onCustomMenuRequestDeleteTypeName();
    void onCustomMenuRequestModifyItem();
    void onCustomMenuRequestDeleteItem();
    // 在指针不指向任何联系人或者类型的时候，就需要关闭详细信息面板
    void CheckHasCursorPointToAnyItem();
    void onMoveToSubMenuTriggered(QString typeName);

private:
    /*
     * inner class
     */
    class AddressGroupItem : public QListWidgetItem
    {
    public:
        AddressGroupItem(const QIcon &icon, const QString &name, const QString& detail):
            QListWidgetItem(icon, name + "(0)"),
            groupName(name),
            isHidden(false),
            description(detail)
        {

        }
        ~AddressGroupItem(){}

        void ResetGroupName(const QString& str){
            this->setText(str + QString("(%1)").arg(itemList.size())); //重置显示的内容
            groupName = str;
        }

        void AddItem(int itemID, QListWidgetItem* item){
            itemList.insert(itemID, item);
            this->setText(QString("%1(%2)").arg(groupName).arg(itemList.size()));
        }

        void RemoveItem(int itemID){
            itemList.remove(itemID);
            this->setText(QString("%1(%2)").arg(groupName).arg(itemList.size()));
        }

        bool ContainItem(QListWidgetItem* item){
            return itemList.values().contains(item);
        }

        QListWidgetItem* getItem(int itemID)const{
            return itemList.value(itemID, NULL);
        }

        void onStatusChanged(){
            isHidden = !isHidden;
            for(auto itemPtr : itemList.values())
                itemPtr->setHidden(isHidden);
            if(isHidden)
                setIcon(QIcon(global::rightArrowDir));
            else
                setIcon(QIcon(global::downArrowDir));
        }

        int ItemNum() const {return itemList.size();}

        QMap<int, QListWidgetItem*> getItemList()const{return itemList;}

        void insertItemList(const QMap<int, QListWidgetItem*>& newItemList){
            for(auto key : newItemList.keys()){
               itemList.insert(key, newItemList.value(key));
            }
            this->setText(QString("%1(%2)").arg(groupName).arg(itemList.size()));
        }

        // 访问器
        bool getStatus()const{return isHidden;}
        QString getTypeName()const{return groupName;}
        QString getDescription()const{return description;}

        // 设置器
        void setTypeName(const QString& name){ResetGroupName(name);}
        void setDescription(const QString& str){description = str;}

    private:
        QMap<int, QListWidgetItem*> itemList;
        QString groupName;
        QString description;
        bool isHidden;
    };

    class AddressItem : public QWidget
    {
    public:
        AddressItem(QWidget* parent, int ID, const QString& name, const QString& type,
                    const QString& sex, const QString& phone, const QString& email,
                    const QString& note, const QPixmap& img):
            QWidget(parent), itemID(ID), name(name), type(type), sex(sex),
            phone(phone), email(email), note(note),
            avatarImg(img), avatar(new QWidget(this)),
            labelName(new QLabel(this)), labelPhone(new QLabel(this))
        {
            avatar->setFixedSize(40,40);
            labelName->setText(name);
            labelPhone->setText(phone);

            QPalette color;
            color.setColor(QPalette::Text,Qt::gray);
            labelPhone->setPalette(color);

            avatar->move(7,7);
            labelName->move(54,10);
            labelPhone->move(54,27);

            avatar->installEventFilter(this);

            this->setMouseTracking(true);
        }
        ~AddressItem(){}

        void UpdateItem(const QString& name, const QString& type,
                        const QString& sex, const QString& phone, const QString& email,
                        const QString& note, const QPixmap& img){
            this->name = name;
            this->type = type;
            this->sex = sex;
            this->phone = phone;
            this->email = email;
            this->note = note;
            avatarImg = img;

            QPainter painter(avatar);
            painter.drawPixmap(avatar->rect(), avatarImg);
            labelName->setText(name);
            labelPhone->setText(phone);
        }

        //访问器
        int getItemID()const{return itemID;}
        QString getName()const{return name;}
        QString getType()const{return type;}
        QString getSex()const{return sex;}
        QString getPhone()const{return phone;}
        QString getEmail()const{return email;}
        QString getNote()const{return note;}
        QPixmap getAvatarImg()const{return avatarImg;}

    private:
        int itemID;
        QString name, type, sex, phone, email, note;
        QWidget* avatar;
        QLabel *labelName;
        QLabel *labelPhone;
        QPixmap avatarImg;
        bool eventFilter(QObject *obj, QEvent *event)
        {
            if(obj == avatar)
            {
                if(event->type() == QEvent::Paint)
                {
                    QPainter painter(avatar);
                    painter.drawPixmap(avatar->rect(), avatarImg);
                }
            }
            return QWidget::eventFilter(obj, event);
        }
    };
    /*
     * inner class
     */

private:
    AddressBookWindow::AddressGroupItem* AddGroupItem(const QString& groupName, const QString& detail);

    void AddAddressItem(AddressGroupItem* group, int ID, const QString& name, const QString& type,
                        const QString& sex, const QString& phone, const QString& email,
                        const QString& note, const QPixmap& avatar);

    void GenerateItemCustomMenu(QListWidgetItem* selectedItem);

    Ui::AddressBookWindow *ui;
    QString userName;
    QMap<QString, QListWidgetItem*> groupList;
    QMap<QListWidgetItem*, bool> groupHiddenList;
    PresentSearchResult* presentResultWnd;

    QMenu *blankMenu;
    QMenu *typeMenu;

    QLineEdit* typeNameEdit;

    QListWidgetItem* currentEditingTypeItem;
    QListWidgetItem* customMenuRequestItem; //产生右键菜单的item

    ItemDetail* detailPanel;
    TypeDetail* typeDetailPanel;
    QTimer* timer;

    bool isInitializing; // 如果不是初始化则添加联系人时就定位到正确的位置
    bool isDeletingType; // 删除类型时以及修改联系人时都是如此
    bool isModifyingItem;
};

#endif // ADDRESSBOOKWINDOW_H
