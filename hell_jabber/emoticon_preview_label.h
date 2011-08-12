#ifndef EMOTICON_PREVIEW_LABEL_H
#define EMOTICON_PREVIEW_LABEL_H

#include <QLabel>
#include <QString>
#include <QEvent>
class emoticon_preview_label : public QLabel
{
    Q_OBJECT
public:
    emoticon_preview_label(QWidget *parent = 0);

    QString m_label_gif_name;

protected:
    void enterEvent( QEvent * event );
    void leaveEvent( QEvent * event );
    //void mousePressEvent ( QMouseEvent * event );
signals:
    void lb_mouse_enterEvent(QString gif_name, QPoint lb_at_xy, QSize lb_wh_size);
    void lb_mouse_leaveEvent(QString gif_name);
    //void lb_mouse_press_event(QString gif_name);


};

#endif // EMOTICON_PREVIEW_LABEL_H
