#include <QString>
#include <QtTest>

class TestEncryptTest : public QObject
{
    Q_OBJECT

public:
    TestEncryptTest();

private Q_SLOTS:
    void testCase1();

private:
    QByteArray Encrypt(const QString& plaintext);
    QString Decrypt(QByteArray ciphertext);
    unsigned char key=0x45;
};

TestEncryptTest::TestEncryptTest()
{
}
QByteArray TestEncryptTest::Encrypt(const QString& plaintext)
{
    std::string stdStr = plaintext.toStdString();
    int plainTextLen = plaintext.length();
    unsigned char cipherText[20];

    cipherText[0] = ((unsigned char)stdStr[0]) ^ key;
    // 不要尝试读取std::string c_str()返回的指针里的字符
    // C++ 98 指出 不要修改这个指针的任何字符
    // C++ 11 指出这个指针指向对象内部的一个储存字符的数组
    for(int i = 1; i < plainTextLen; i++){
        cipherText[i] = ((unsigned char)stdStr[i])  ^ cipherText[i - 1];
    }
    return QByteArray((char*)cipherText, plainTextLen);
}

QString TestEncryptTest::Decrypt(QByteArray ciphertext)
{
    unsigned char plainText[21];
    int Len = ciphertext.length();

    plainText[0] = ciphertext[0] ^ key;
    for(int i = 1; i < Len; i++){
        plainText[i] = ciphertext[i] ^ ciphertext[i - 1];
    }
    plainText[Len] = 0; // 添加空字符
    return QString((char*)plainText);
}
void TestEncryptTest::testCase1()
{
    QString str="hello world!";
    auto result=Encrypt(str);
    auto plaintext=Decrypt(result);


    QVERIFY2(plaintext == str, "Failure");
}

QTEST_APPLESS_MAIN(TestEncryptTest)

#include "tst_testencrypttest.moc"
