#include <QString>
#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>

class DatasetTest : public QObject
{
    Q_OBJECT

public:
    DatasetTest();

private Q_SLOTS:
    void testCase1();
};

DatasetTest::DatasetTest()
{
}

void DatasetTest::testCase1()
{
    auto db = QSqlDatabase::addDatabase("QODBC", "SqlServerConnection");
    db.setDatabaseName(QString("DRIVER={SQL SERVER};" "SERVER=%1;" "DATABASE=%2;" "UID=%3;" "PWD=%4;")
                           .arg("192.168.1.177")
                           .arg("AddressBookDatabase")
                           .arg("sa")
                           .arg("1Q2W3EASD")
                           );
    QVERIFY2(db.open(), "open database failed");

    QSqlQuery query(db);
    query.prepare(QString("INSERT INTO %1(typeName, detail)"
                  "VALUES(?, ?)").arg("AddressType_lusian"));
    query.addBindValue("newTypeName");
    query.addBindValue("detail");

    QVERIFY2(query.exec(), "insert into table failed");

    db.close();
}

QTEST_APPLESS_MAIN(DatasetTest)

#include "tst_datasettest.moc"
