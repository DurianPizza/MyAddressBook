#ifndef TYPEDETAIL_H
#define TYPEDETAIL_H

#include <QWidget>

namespace Ui {
class TypeDetail;
}

class TypeDetail : public QWidget
{
    Q_OBJECT

public:
    explicit TypeDetail(QWidget *parent = 0);
    ~TypeDetail();

    // setter
    void setPresentTypeName(const QString& str);
    void setPresentDescription(const QString& str);

private:
    Ui::TypeDetail *ui;
};

#endif // TYPEDETAIL_H
