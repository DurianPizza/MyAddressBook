#include <QString>
#include <QtTest>
#include <list>
using namespace std;
struct BiNode
{
    BiNode(){};
    BiNode(char Data,int Pri):data(Data),priority(Pri),lchild(NULL),rchild(NULL){};
    char data;
    int priority;
    BiNode *lchild, *rchild;
};
class TestCreatTreeTest : public QObject
{
    Q_OBJECT

public:
    TestCreatTreeTest();
    BiNode* CreatTree(list<BiNode*> &Huffarray);

private Q_SLOTS:
    void testCase1();
};

BiNode* TestCreatTreeTest::CreatTree(list<BiNode*> &Huffarray)
{
    int n = Huffarray.size();

    list<BiNode*>::iterator it = Huffarray.begin();
    list<BiNode*>::iterator it2;

    while (Huffarray.size() < (unsigned)2 * n - 1)
    {
        int priority = (*it)->priority;
        priority += (*++it)->priority;

        BiNode *newNode = (BiNode *)malloc(sizeof(BiNode));
        newNode->lchild = (*--it);
        newNode->rchild = (*++it);
        newNode->priority = priority;
        for (it2 = Huffarray.begin(); it2 != Huffarray.end(); it2++)//把新产生的那个树节点按大小插入原来的链表
        {
            if (((*it2)->priority) >= (newNode->priority))
            {
                Huffarray.insert(it2, newNode);
                break;
            }
        }
        if (it2 == Huffarray.end())  //最大则插在最后面
        {
            Huffarray.push_back(newNode);
        }
        it++;
    }

    BiNode *tree = (BiNode *)malloc(sizeof(BiNode));
    list<BiNode*>::iterator itend = Huffarray.end();
    tree = *(--itend);

    return tree;
}

bool bitree_compare(BiNode* proot1, BiNode* proot2)
{
  if (proot1 == NULL && proot2 == NULL)//都为空
    return true;
  if((proot1 != NULL && proot2 == NULL) ||
    (proot1 == NULL && proot2 != NULL))//一个空，一个非空
    return false;
  /*比较元素*/
  if(proot1->priority != proot2->priority)
    return false;
  bool left_compare = bitree_compare(proot1->lchild, proot2->lchild);
  bool right_compare = bitree_compare(proot1->rchild, proot2->rchild);
  return (left_compare && right_compare);
}

TestCreatTreeTest::TestCreatTreeTest()
{
}

void TestCreatTreeTest::testCase1()
{
    BiNode node1('a',13),node2('b',4),node3('d',2);
    list<BiNode*> NodeList;
    NodeList.push_back(&node3);
    NodeList.push_back(&node2);
    NodeList.push_back(&node1);

    BiNode* tree=CreatTree(NodeList);

    BiNode treeNode1,treeNode2,treeNode3,treeNode4,treeNode5;
    treeNode1.priority=19;treeNode1.lchild=&treeNode2;treeNode1.rchild=&treeNode3;
    treeNode2.priority=6;treeNode2.lchild=&treeNode4;treeNode2.rchild=&treeNode5;
    treeNode3.priority=13;treeNode3.lchild=NULL;treeNode3.rchild=NULL;
    treeNode4.priority=2;treeNode4.lchild=NULL;treeNode4.rchild=NULL;
    treeNode5.priority=4;treeNode5.lchild=NULL;treeNode5.rchild=NULL;

    bool TestResult=bitree_compare(tree,&treeNode1);
    QVERIFY2(TestResult, "Failure");
}

QTEST_APPLESS_MAIN(TestCreatTreeTest)

#include "tst_testcreattreetest.moc"
