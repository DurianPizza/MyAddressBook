// created by Brain at 2017.6.20
// * 继承了QLabel类，实现了发送鼠标相对移动量的功能
// * 通过处理mouseMoveEvent事件实现


#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    CustomLabel(QWidget *parent = 0);
    ~CustomLabel();

signals:
    void onMouseMove(QPoint p);
public slots:

protected:
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

private:
    QPoint lastPos;
};

#endif // CUSTOMLABEL_H
