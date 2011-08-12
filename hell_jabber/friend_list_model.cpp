#include "friend_list_model.h"
#include <QPixmap>
#include <QImage>
#include <QList>
#include <QVariant>
#include <QStringList>

friend_list_model::friend_list_model(QListView *p_friend_list_view, QStringList jid_list, QObject *parent ) :
    QAbstractListModel(parent)
{
    m_barejid_list = jid_list;
    m_map_jid_element.clear();
    m_element_size = 0;
    m_plv_friend_list = p_friend_list_view;
    m_item_draw_rect = -1;
}

bool friend_list_model::flm_add_element(QString jid, friend_list_element_t element)
{
    if(jid.isNull() || jid.isEmpty()) {
        return false;
    }

    friend_list_element_t *flm = new friend_list_element_t;
    *flm = element;

    m_map_jid_element[jid] = flm;
    m_element_size = m_map_jid_element.size();
    return true;
}

bool friend_list_model::flm_update_jid_list(QStringList& jid_list)
{
    if(jid_list.isEmpty()) {
        return false;
    }
    m_barejid_list = jid_list;
    m_plv_friend_list->setUpdatesEnabled(false);
    m_plv_friend_list->setUpdatesEnabled(true);
    return true;
}

bool friend_list_model::flm_update_presence_status(QString jid, QXmppPresence::Status::Type status)
{
    if(m_map_jid_element.contains(jid)) {
        m_pflm = m_map_jid_element[jid];
        m_pflm->status = status;
        m_plv_friend_list->setUpdatesEnabled(false);
        m_plv_friend_list->setUpdatesEnabled(true);
        return true;
    } else {
        return false;
    }
}

bool friend_list_model::flm_update_mood(QString jid, QString mood)
{
    if(m_map_jid_element.contains(jid) && !mood.isNull() && !mood.isEmpty()) {
        m_pflm = m_map_jid_element[jid];
        m_pflm->mood = mood;
        m_plv_friend_list->setUpdatesEnabled(false);
        m_plv_friend_list->setUpdatesEnabled(true);
        return true;
    } else {
        return false;
    }
}

bool friend_list_model::flm_update_avatar(QString jid, QImage avatar)
{
    if(m_map_jid_element.contains(jid)) {
        m_pflm = m_map_jid_element[jid];
        m_pflm->avatar = avatar;
        m_plv_friend_list->setUpdatesEnabled(false);
        m_plv_friend_list->setUpdatesEnabled(true);
        return true;
    } else {
        return false;
    }
}

int friend_list_model::rowCount(const QModelIndex &parent) const
{
    //qDebug("element_size = %d", m_element_size);
    return m_element_size;
}

QVariant friend_list_model::data(const QModelIndex &index, int role) const
{
    static friend_list_element_t* pflm;
    //qDebug("friend_list_model:: %d", index.row());
    if ( (!index.isValid()) || (index.row() >= m_element_size))
        return QVariant();

    if( index.row() < m_barejid_list.size() ) {
        pflm = m_map_jid_element[m_barejid_list.at(index.row())];
    } else {
        qDebug("m_barejid_list.contains check fail");
        return QVariant();
    }
    if(role == Qt::UserRole) {//name
        //qDebug("Qt::UserRole");
        return pflm->name;
    } else if(role == Qt::UserRole + 1) {//image
        //qDebug("Qt::UserRole + 1");
        return pflm->avatar;
    } else if(role == Qt::UserRole + 2) {
        return pflm->mood;
    } else if(role == Qt::UserRole + 3) {
        //qDebug("m_item_draw_rect = %d", m_item_draw_rect);
        return m_item_draw_rect;
    }

    return QVariant();
}
