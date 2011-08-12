#include "emoticon_preview_widget.h"

emoticon_preview_widget::emoticon_preview_widget(QWidget *parent) :
    QWidget(parent)
{
    m_movie_file_name = "";
}

void emoticon_preview_widget::mousePressEvent ( QMouseEvent * event )
{
    emit emoticon_preview_widget_hideEvent(m_movie_file_name);
    //qDebug("emoticon_preview_widget::mousePressEvent");

    QWidget::mousePressEvent(event);
}

//void emoticon_preview_widget::hideEvent ( QHideEvent * event )
//{
//    //emit emoticon_preview_widget_hideEvent(m_movie_file_name);
//    qDebug("emoticon_preview_widget::hideEvent");
//}
