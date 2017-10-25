#ifndef STDAFX_H
#define STDAFX_H

#define STRING(x) QStringLiteral(x).toStdString().c_str()
#include <QString>
#include <QMap>
#include <QPixmap>
#include <QSqlDatabase>
#include <QByteArray>
#include "threadcontroller.h"

struct AddressBookItem
{
    int itemID;
    QString name;
    QString type;
    QString sex;
    QString phone;
    QString email;
    QString note;
    //QString avatarPath;
    QByteArray avatarByteArr;
    bool succeed;

    bool operator < (const AddressBookItem& item)
    {
        return this->name < item.name;
    }
};

// external variable declaration
class global
{
public:
    // 加载QSS文件的函数
    static QByteArray LoadQSSFile(const QString& qssName);

    // 清理全局变量的函数
    static void ClearGlobalVaraible();

    // 数据库相关
    static bool OpenDatabase();
    static void CloseDatabase();

    static QString getHexString(const QByteArray& str);

public:
    static QString userName;
    static QString tableAddressItemName;
    static QString tableAddressTypeName;
    static QMap<QString, int>typeNameTotypeID;
    static const char* selectFilter;
    static const char* defaultAvatarDir;
    static const char* createAddressTypeSQL;
    static const char* createAddressItemSQL;
    static const char* createDefaultTypeSQL;
    static const int avatarWidth = 40;
    static const int avatarHeight = 40;
    static const int addItemDialogAvatarWidth = 80;
    static const int addItemDialogAvatarHeight = 80;
    // 判断是否插入了新类型名称
    static bool hasInsertNewType;
    static QString newTypeName;
    static QString newTypeDescription;

    // 判断是否修改了类型名称
    static bool hasModifyTypeName;
    static QString originalTypeName;
    static QString modifiedTypeName;
    static QString modifiedDetail;
    // 判断是否删除了类型名称
    static bool hasDeleteTypeName;
    static QString deletedTypeName;

    //查询用到的SQL语句
    static const char* baseQuerySQL;
    static const char* nameConditionSQL;
    static const char* sexConditionSQL;
    static const char* typeConditionSQL;

    // 判断是否修改了联系人
    static bool hasModifyAddressItem;
    static int modifiedItemID;
    static QString modifiedName;
    static QString modifiedSex;
    static QString originalType;
    static QString modifiedType;
    static QString modifiedPhone;
    static QString modifiedEmail;
    static QString modifiedNote;
    static QString modifiedAvatarDir;

    // 判断是否删除了联系人
    static bool hasDeleteAddressItem;
    static int deletedItemID;
    static QString deletedItemTypeName;

    // 指定修改某个特定的联系人
    static int specificItemID;

    // 使用QPixmap必须先构建GUI
    //static QPixmap specificAvatar;

    static const char* qqAvatarDir;
    static const char* downArrowDir;
    static const char* rightArrowDir;

    static QSqlDatabase db;
    static bool connectToDatabase;

    // 管理和服务器的连接
    static Controller* controller;

    // 用于选择服务器线程的反馈信号
    static bool loginHasPressed;
    static bool registerHasPressed;

    static unsigned char key;
    static QByteArray Encrypt(const QString& plaintext);
    static QString Decrypt(QByteArray ciphertext);

    // 判断是否选择了新的头像
    static bool hasSelectedNewAvatar;

};
#endif // STDAFX_H
