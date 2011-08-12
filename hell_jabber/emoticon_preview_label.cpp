#include "emoticon_preview_label.h"

emoticon_preview_label::emoticon_preview_label(QWidget *parent) :
    QLabel(parent)
{
    setMouseTracking(true);
}

void emoticon_preview_label::enterEvent ( QEvent * event )
{
    emit lb_mouse_enterEvent(m_label_gif_name, pos(), size());
    //qDebug("lb enterEvent: %s", m_label_gif_name.toAscii().data());
}
void emoticon_preview_label::leaveEvent ( QEvent * event )
{
    emit lb_mouse_leaveEvent(m_label_gif_name);
    //qDebug("lb leaveEvent: %s", m_label_gif_name.toAscii().data());
}

//void emoticon_preview_label::mousePressEvent ( QMouseEvent * event )
//{
//    emit lb_mouse_press_event(m_label_gif_name);
//}


