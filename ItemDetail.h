// created by Brian at 2017.6.16

#ifndef ITEMDETAIL_H
#define ITEMDETAIL_H

#include <QWidget>

namespace Ui {
class ItemDetail;
}

class ItemDetail : public QWidget
{
    Q_OBJECT

public:
    explicit ItemDetail(QWidget *parent = 0);
    ~ItemDetail();

    // setter
    void setPresentName(const QString& s);
    void setPresentType(const QString& s);
    void setPresentSex(const QString& s);
    void setPresentPhone(const QString& s);
    void setPresentEmail(const QString& s);
    void setPresentNote(const QString& s);
    void setPresentAvatar(const QPixmap& map);

private:
    Ui::ItemDetail *ui;
};

#endif // ITEMDETAIL_H
