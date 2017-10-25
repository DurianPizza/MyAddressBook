#include <QString>
#include <QtTest>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include "testmd5.h"

class TestMD5Test : public QObject
{
    Q_OBJECT

public:
    TestMD5Test();

private Q_SLOTS:
    void testCase1();
private:

    QString Local_MD5Check(const char* FileName);
};

QString TestMD5Test::Local_MD5Check(const char* FileName)
{
    char File_MD5[33];
    char databuff[1024];
    std::fstream file(FileName, std::ios::in | std::ios::binary);
    MD5 md5;
    file.seekg(0, std::ios::end);
    auto FileLength = file.tellg();
    file.seekg(0, std::ios::beg);
    for (int i = 0; i < (FileLength / 1024); i++)
    {
        file.read(databuff, 1024);
        md5.MD5Update((unsigned char *)databuff, 1024);
    }
    file.read(databuff, (FileLength % 1024));
    md5.MD5Update((const unsigned char *)databuff, FileLength % 1024);
    md5.MD5Final();
    for (int i = 0; i < 16; i++)
    {
        sprintf(&File_MD5[i * 2], "%02X", md5.digest[i]);
    }
    file.close();
    return (QString)File_MD5;
}

TestMD5Test::TestMD5Test()
{
}

void TestMD5Test::testCase1()
{


    QString md5=Local_MD5Check("C:\\Users\\Crazyman\\Documents\\testMD5\\²âÊÔÎÄ¼þ.txt");

    QVERIFY2(md5.toLower()=="5a67474ba76d3f6962f1f6038d054075", "Failure");
}

QTEST_APPLESS_MAIN(TestMD5Test)

#include "tst_testmd5test.moc"
