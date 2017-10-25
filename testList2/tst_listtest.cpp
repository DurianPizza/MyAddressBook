#include <QString>
#include <QtTest>
#include <list>
#include <fstream>
#include <vector>
using namespace std;
struct BiNode
{
    char data;
    int priority;
    BiNode *lchild, *rchild;
};

class ListTest : public QObject
{
    Q_OBJECT

public:
    ListTest();
    bool IsSameList(std::list<BiNode *> list1,std::list<BiNode *> list2);
    std::list<BiNode*> Creatlist(int probability[]);
private Q_SLOTS:
    void testCase1();

};

std::list<BiNode *> ListTest::Creatlist(int probability[])
{
    list<BiNode*> Huffarray;
    for (int i = 0; i < 256; i++)
    {
        if (probability[i] != 0)
        {
            BiNode* aux = (BiNode*)malloc(sizeof(BiNode));
            aux->priority = probability[i];
            aux->data = (char)i;
            aux->lchild = NULL;
            aux->rchild = NULL;
            if (Huffarray.empty())
                Huffarray.push_back(aux);
            else
            {
                list<BiNode*>::iterator it;
                for (it = Huffarray.begin(); it != Huffarray.end(); it++)
                {
                    if ( ((*it)->priority) >= (aux->priority) )
                    {
                        Huffarray.insert(it,aux);
                        break;
                    }
                }
                if (it==Huffarray.end())
                {
                    Huffarray.push_back(aux);
                }
            }

        }
    }
    return Huffarray;

}
bool ListTest::IsSameList(std::list<BiNode *> list1,std::list<BiNode *> list2)
{
    if(list1.size()!=list2.size())
        return false;
     list<BiNode *>::iterator itor1,itor2;

     itor1 = list1.begin();
     itor2 = list2.begin();
         while(itor1!=list1.end())
         {
             if((*itor1)->data!=(*itor2)->data ||
                (*itor1)->priority!=(*itor2)->priority ||
                (*itor1)->lchild!=(*itor2)->lchild ||
                (*itor1)->rchild!=(*itor2)->rchild)
                 return false;

             itor1++;
             itor2++;
         }
    return true;
}

ListTest::ListTest()
{
}

void ListTest::testCase1()
{
        ifstream infile0("D:\\word.txt");
        infile0.seekg(0, ios_base::end);
        int FileSize = infile0.tellg();

        infile0.close();

        ifstream infile("D:\\word.txt", ifstream::binary);

        char * array = new char[FileSize];

        infile.read(array, FileSize);

        infile.close();


        int * probability = (int *)malloc(sizeof(int)* 256);

        for (int i = 0; i < 256; i++)
            probability[i] = 0;

        for (int i = 0; i < FileSize;i++)
            probability[(unsigned char)array[i]]++;
        list<BiNode*> list1= Creatlist(probability);
        list<BiNode*> list2;
        BiNode node1,node2;
        node1.data='b';node1.priority=5;node1.lchild=node1.rchild=NULL;
        node2.data='a';node2.priority=23;node2.lchild=node2.rchild=NULL;
        list2.push_back(&node1);
        list2.push_back(&node2);

    QVERIFY2(IsSameList(list1,list2), "Failure");
}

QTEST_APPLESS_MAIN(ListTest)

#include "tst_listtest.moc"
