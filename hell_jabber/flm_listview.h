#ifndef FLM_LISTVIEW_H
#define FLM_LISTVIEW_H

#include <QObject>
#include <QListView>


class flm_listView : public QListView
{
    Q_OBJECT
public:
    explicit flm_listView(QObject *parent = 0);

signals:
    void item_double_click(QString jid);

public slots:
    void mouseMoveEvent ( QMouseEvent * e );
    //void mouseDoubleClickEvent ( QMouseEvent * event );
};

#endif // FLM_LISTVIEW_H
