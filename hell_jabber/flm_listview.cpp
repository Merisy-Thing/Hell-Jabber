#include "flm_listview.h"
#include <QMouseEvent>

flm_listView::flm_listView(QObject *parent)
{
    this->setMouseTracking(true);
}

void flm_listView::mouseMoveEvent ( QMouseEvent * e )
{

    //qDebug("x = %d, y = %d",e->x(), e->y());
}
/*
void flm_listView::mouseDoubleClickEvent ( QMouseEvent * e )
{
    //qDebug("x = %d, y = %d",e->x(), e->y());
}
*/
