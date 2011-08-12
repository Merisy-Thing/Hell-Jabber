#ifndef FRIEND_LIST_DELEGATE_H
#define FRIEND_LIST_DELEGATE_H

#include <QStyledItemDelegate>

class friend_list_delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    friend_list_delegate(QObject *parent = 0);

    //basic function for a read-only delegate, you can draw anything with the painter
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    //vriable
    QFont m_name_font;
    QFont m_smsg_font;

};

#endif // FRIEND_LIST_DELEGATE_H
