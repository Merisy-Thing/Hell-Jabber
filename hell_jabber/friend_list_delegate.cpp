#include "friend_list_delegate.h"
#include <QtGui>
#include <QImage>

#define FLM_ROW_HEIGHT      46
#define FLM_ITEM_HEIGHT     42
#define FLM_ITEM_GAP        (FLM_ROW_HEIGHT-FLM_ITEM_HEIGHT)

friend_list_delegate::friend_list_delegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
        m_name_font = QFont("Helvetica", 10, QFont::Bold);
        m_smsg_font = QFont("Helvetica", 9, QFont::Light);
}

void friend_list_delegate::paint ( QPainter * painter, const QStyleOptionViewItem & opt, const QModelIndex & index ) const
{
    int start_xpos,start_ypos;

    //qDebug("friend_list_delegate ::paint opt.x = %d, opt.y = %d",opt.rect.x(), opt.rect.y());
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    start_xpos = opt.rect.x();
    start_ypos = opt.rect.y();
    //Name
    const QString& text = index.data(Qt::UserRole).toString();
    painter->setPen(QColor(0,0,0));
    painter->setFont(m_name_font);
    QFontMetrics fm = painter->font();
    QRect name_Rect = QRect(QPoint(start_xpos + 48,start_ypos),QSize(fm.width(text),fm.height()));
    painter->drawText(name_Rect, text);

    //Avatar
    const QImage avatar = qvariant_cast<QImage>(index.data(Qt::UserRole+1));
    QPixmap pixmap_avatar = QPixmap::fromImage(avatar);
    QRect pixmap_Rect = QRect(QPoint(start_xpos,start_ypos), QSize(42,42));
    painter->drawPixmap(pixmap_Rect, pixmap_avatar);

    //Status message
    const QString& status_msg = index.data(Qt::UserRole+2).toString();
    painter->setPen(QColor(131,131,131));
    painter->setFont(m_smsg_font);
    fm = painter->font();
    name_Rect = QRect(QPoint(start_xpos + 48,start_ypos + FLM_ITEM_HEIGHT/2 + FLM_ITEM_GAP/2),QSize(fm.width(status_msg),fm.height()));
    painter->drawText(name_Rect, status_msg);

    //Border
    int item_dram_rect = index.data(Qt::UserRole+3).toInt();
    //qDebug("item_dram_rect = %d",item_dram_rect);
    if(item_dram_rect == index.row()) {
        //qDebug("item_dram_rect == index.row()");
        painter->setPen(QColor(0,0,128));
        painter->drawRect(QRect(start_xpos,start_ypos, opt.rect.width()-1, 42));
    }

    painter->restore();


}


QSize friend_list_delegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    //qDebug("sizeHint");
    return QSize(option.rect.width(),FLM_ROW_HEIGHT);
    //return QStyledItemDelegate::sizeHint(option,index);
}
