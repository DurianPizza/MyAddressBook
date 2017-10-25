#include "customlabel.h"

#include <QMouseEvent>

// usage: CustomLabel inherit QLabel to emit an signal
//        which parameters is the movement relate to the last position

CustomLabel::CustomLabel(QWidget *parent) : QLabel(parent)
{
    lastPos.setX(-1);
    lastPos.setY(-1);
}

CustomLabel::~CustomLabel()
{

}

// function: deal the mouse move event
//           if lastPos != (-1, -1), then emit the signal onMouseMove
//           parameter is the movement relate to the last position
void CustomLabel::mouseMoveEvent(QMouseEvent *ev)
{
    this->setCursor(Qt::ClosedHandCursor);
    if(lastPos.x() == -1 && lastPos.y() == -1)
        lastPos = ev->pos();
    else{
        emit onMouseMove(QPoint(ev->pos().x() - lastPos.x(),
                                ev->pos().y() - lastPos.y()));
        lastPos = ev->pos();
    }
}

// function: deal with the mouseReleaseEvent
//           when mouse is release, reset lastPos to (-1, -1)
void CustomLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    lastPos.setX(-1);
    lastPos.setY(-1);
    this->setCursor(Qt::OpenHandCursor);
}
