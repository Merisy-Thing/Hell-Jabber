#ifndef EMOTICON_PREVIEW_WIDGET_H
#define EMOTICON_PREVIEW_WIDGET_H

#include <QWidget>
#include <QHideEvent>

class emoticon_preview_widget : public QWidget
{
    Q_OBJECT
public:
    explicit emoticon_preview_widget(QWidget *parent = 0);

    QString m_movie_file_name;

    void mousePressEvent ( QMouseEvent * event );
    //void hideEvent ( QHideEvent * event );
signals:
    void emoticon_preview_widget_hideEvent (QString movie_file_name);

public slots:

};

#endif // EMOTICON_PREVIEW_WIDGET_H
