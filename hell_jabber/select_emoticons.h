#ifndef SELECT_EMOTICONS_H
#define SELECT_EMOTICONS_H

#include <QWidget>
#include <QMap>
#include <QLabel>
#include <QTabWidget>
#include <QPushButton>

#include "emoticon_preview_label.h"
#include "emoticon_preview_widget.h"

class QDir;
class QFile;
class QFileInfo;

class select_emoticons : public QWidget
{
    Q_OBJECT
public:
    explicit select_emoticons(QWidget *parent = 0);

    QMap<QString, QLabel*> m_emoticon_list;
    enum ICON_Size
    {
        PREVIEW_ICON_SIZE = 42,
        PREVIEW_GRID_SIZE = 42,
        ONE_LINE_ICON_NUM = 10,
        ICON_MAX_LINE = 9
    };

    QTabWidget *m_tab_widget;

    emoticon_preview_widget *m_gif_preview_widget;
    QLabel *m_gif_preview_label;
    QMovie *m_gif_movie;

    QWidget *m_system_emoticon_tab;
    QWidget *m_custom_emoticon_tab;
    int m_custom_emoticon_count;

    QPoint m_screen_pos;
    QPushButton *m_pb_add_emoticon;

signals:
    void emoticons_lb_mouse_press_event(QString gif_name);

public slots:
    void lb_mouse_enterEvent(QString gif_name, QPoint lb_at_xy, QSize lb_wh_size);
    void lb_mouse_leaveEvent(QString gif_name);
    void lb_mouse_press_event(QString gif_name);
    void pb_add_emoticon_clicked();
protected:
    //void mouseMoveEvent( QMouseEvent * event );

private:

};

#endif // SELECT_EMOTICONS_H
