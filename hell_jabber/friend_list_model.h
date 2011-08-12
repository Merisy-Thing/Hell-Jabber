#ifndef FRIEND_LIST_MODEL_H
#define FRIEND_LIST_MODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include <QImage>
#include <QList>
#include <QMap>
#include <QListView>

#include "QXmppVCardIq.h"
#include "QXmppPresence.h"

typedef struct friend_list_element_t
{
    QXmppPresence::Status::Type status;
    QString name;
    QString mood;
    QImage  avatar;
} friend_list_element_t;

class friend_list_model : public QAbstractListModel
{
    Q_OBJECT

public:
    friend_list_model(QListView *p_friend_list_view, QStringList jid_list, QObject *parent = 0);

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;

    bool flm_add_element(QString jid, friend_list_element_t element);
    bool flm_update_jid_list(QStringList& jid_list);
    bool flm_update_presence_status(QString jid, QXmppPresence::Status::Type status);
    bool flm_update_mood(QString jid, QString mood);
    bool flm_update_avatar(QString jid, QImage  avatar);

    /*Map Jid  to custom data*/
    QMap<QString, friend_list_element_t*> m_map_jid_element;
    QStringList m_barejid_list;

    int m_item_draw_rect;

private:
    int m_element_size;
    friend_list_element_t* m_pflm;
    QListView *m_plv_friend_list;

};

#endif // FRIEND_LIST_MODEL_H
