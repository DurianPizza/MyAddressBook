// modified by Brian at 2017.6.14
// * 删除了PresentSearchResult类构造函数的QString参数以及userName成员

#ifndef PRESENTSEARCHRESULT_H
#define PRESENTSEARCHRESULT_H

#include <QMainWindow>
#include <QString>

namespace Ui {
class PresentSearchResult;
}

class PresentSearchResult : public QMainWindow
{
    Q_OBJECT

public:
    explicit PresentSearchResult(QWidget *parent = 0);
    ~PresentSearchResult();

    // 每次显示窗口时都被调用
    void UpdateUI();

public slots:
    void onAddNewType(QString& newType);
    void onDeleteType(QString& deletedType);

private slots:
    void on_search_clicked();

private:
    Ui::PresentSearchResult *ui;
};

#endif // PRESENTSEARCHRESULT_H
