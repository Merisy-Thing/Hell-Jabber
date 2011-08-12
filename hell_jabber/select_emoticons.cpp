#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "select_emoticons.h"
#include <QImage>
#include <QGridLayout>
#include <QMovie>
#include <QFileDialog>

select_emoticons::select_emoticons( QWidget *parent) :
    QWidget(parent)
{
    QString system_emoticons_path = "SYS_Emoticons/";
    QString custom_emoticons_path = "Custom_Emoticons/";
    emoticon_preview_label *label;
    QImage icon;
    QDir dir(system_emoticons_path);

    m_tab_widget = new QTabWidget(this);
    m_system_emoticon_tab = new QWidget(m_tab_widget);
    m_custom_emoticon_tab = new QWidget(m_tab_widget);
    m_tab_widget->move(2,1);
    m_tab_widget->addTab(m_system_emoticon_tab, "System");
    m_tab_widget->addTab(m_custom_emoticon_tab, "Custom");

    m_gif_preview_widget = new emoticon_preview_widget(this);
    m_gif_preview_widget->setWindowFlags(Qt::Popup);
    m_gif_preview_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_gif_preview_widget->setStyleSheet("background-color:blue;");
    m_gif_preview_widget->setVisible(true);
    m_gif_preview_widget->setVisible(false);
    connect(m_gif_preview_widget,SIGNAL(emoticon_preview_widget_hideEvent(QString)),
            SLOT(lb_mouse_press_event(QString)));

    m_gif_movie = new QMovie;
    m_gif_preview_label = new QLabel(m_gif_preview_widget);
    m_gif_preview_label->move(1,1);
    m_gif_preview_label->setMovie(m_gif_movie);
    m_gif_preview_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_screen_pos = QPoint(0,0);

    m_pb_add_emoticon = new QPushButton(tr("Add Emoticon"), this);
    //m_pb_add_emoticon->setWindowFlags(Qt::);
    m_pb_add_emoticon->setMinimumSize(96, 20);
    m_pb_add_emoticon->setMaximumSize(96, 20);
    m_pb_add_emoticon->setStyleSheet("background-color:Gainsboro;");
    m_pb_add_emoticon->move( 325, 0);
    connect(m_pb_add_emoticon, SIGNAL(clicked()), SLOT(pb_add_emoticon_clicked()));

    if (dir.exists()) {
        dir.setFilter(QDir::Files);
        QList<QFileInfo> list = dir.entryInfoList();
        for(int i=0; i<list.size(); i++){
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.isFile()) {
                label = new emoticon_preview_label(m_system_emoticon_tab);
                QString gif_file_name_temp = system_emoticons_path + fileInfo.fileName();
                if(gif_file_name_temp.endsWith(".gif", Qt::CaseInsensitive)
                        || gif_file_name_temp.endsWith(".jpg", Qt::CaseInsensitive)
                        || gif_file_name_temp.endsWith(".png", Qt::CaseInsensitive)) {

                    if(!icon.load(gif_file_name_temp)) {
                        continue;
                    }
                    label->m_label_gif_name = gif_file_name_temp;

                    if(icon.width() > PREVIEW_ICON_SIZE || icon.height() > PREVIEW_ICON_SIZE) {
                        icon = icon.scaled(PREVIEW_ICON_SIZE,PREVIEW_ICON_SIZE, Qt::KeepAspectRatio);
                    }

                    label->setMinimumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
                    label->setMaximumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
                    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                    label->setPixmap(QPixmap::fromImage(icon));
                    label->move(PREVIEW_GRID_SIZE*(i%ONE_LINE_ICON_NUM)+2,
                                (i/ONE_LINE_ICON_NUM)*PREVIEW_GRID_SIZE+2);

                    m_emoticon_list[gif_file_name_temp] = label;

                    connect(label,SIGNAL(lb_mouse_enterEvent(QString, QPoint, QSize)),
                            SLOT(lb_mouse_enterEvent(QString, QPoint, QSize)));
                    connect(label,SIGNAL(lb_mouse_leaveEvent(QString)),
                            SLOT(lb_mouse_leaveEvent(QString)));

                }
                //qDebug("gif_file_name = %s",gif_file_name.arg(i*ONE_LINE_ICON_NUM + j).toAscii().data());
            } else {
                qDebug("not a file 1");
            }
        }
    } else {
        qDebug("dir not exists 1");
    }

    dir.setPath(custom_emoticons_path);
    if (dir.exists()) {
        dir.setFilter(QDir::Files);
        QList<QFileInfo> list = dir.entryInfoList();
        for(m_custom_emoticon_count=0; m_custom_emoticon_count<list.size(); m_custom_emoticon_count++){
            QFileInfo fileInfo = list.at(m_custom_emoticon_count);
            if(fileInfo.isFile()) {
                label = new emoticon_preview_label(m_custom_emoticon_tab);
                QString gif_file_name_temp = custom_emoticons_path + fileInfo.fileName();
                if(gif_file_name_temp.endsWith(".gif", Qt::CaseInsensitive)
                        || gif_file_name_temp.endsWith(".jpg", Qt::CaseInsensitive)
                        || gif_file_name_temp.endsWith(".png", Qt::CaseInsensitive)) {

                    if(!icon.load(gif_file_name_temp)) {
                        qDebug("111111111111");
                        continue;
                    }
                    label->m_label_gif_name = gif_file_name_temp;

                    if(icon.width() > PREVIEW_ICON_SIZE || icon.height() > PREVIEW_ICON_SIZE) {
                        icon = icon.scaled(PREVIEW_ICON_SIZE,PREVIEW_ICON_SIZE, Qt::KeepAspectRatio);
                    }

                    label->setMinimumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
                    label->setMaximumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
                    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                    label->setPixmap(QPixmap::fromImage(icon));
                    label->move(PREVIEW_GRID_SIZE*(m_custom_emoticon_count%ONE_LINE_ICON_NUM)+2,
                                (m_custom_emoticon_count/ONE_LINE_ICON_NUM)*PREVIEW_GRID_SIZE+2);

                    //m_emoticon_list[gif_file_name_temp] = label;

                    connect(label,SIGNAL(lb_mouse_enterEvent(QString, QPoint, QSize)),
                            SLOT(lb_mouse_enterEvent(QString, QPoint, QSize)));
                    connect(label,SIGNAL(lb_mouse_leaveEvent(QString)),
                            SLOT(lb_mouse_leaveEvent(QString)));

                }
                //qDebug("gif_file_name = %s",gif_file_name.arg(i*ONE_LINE_ICON_NUM + j).toAscii().data());
            } else {
                qDebug("not a file 2");
            }
        }
    } else {
        qDebug("dir not exists 2");
    }

}
void select_emoticons::lb_mouse_enterEvent(QString gif_name, QPoint lb_at_xy, QSize lb_wh_size)
{
    if(gif_name.isEmpty()) {
        qDebug("error: gif_name is Empty");
        return;
    }

    //qDebug("m_screen_pos.x() = %d, m_screen_pos.y() = %d", m_screen_pos.x(),m_screen_pos.y());
    m_gif_preview_widget->move(m_screen_pos.x() + lb_at_xy.x()+lb_wh_size.width(),
                               m_screen_pos.y() + lb_at_xy.y()+lb_wh_size.height());
    m_gif_movie->setFileName(gif_name);
    m_gif_movie->start();
    QRect r= m_gif_movie->frameRect();
    m_gif_preview_widget->setMinimumSize(r.width()+ 2, r.height()+ 2);
    m_gif_preview_widget->setMaximumSize(r.width()+ 2, r.height()+ 2);
    m_gif_preview_label->setMinimumSize(r.width(), r.height());
    m_gif_preview_label->setMaximumSize(r.width(), r.height());
    m_gif_preview_widget->m_movie_file_name = gif_name;
    m_gif_preview_widget->setVisible(true);

    //qDebug("*widget receive signal: lb_mouse_enterEvent : %s",gif_name.toAscii().data());
}
void select_emoticons::lb_mouse_leaveEvent(QString gif_name)
{
    m_gif_movie->stop();
    m_gif_preview_widget->setVisible(false);
    //qDebug("*widget receive signal: lb_mouse_leaveEvent : %s",gif_name.toAscii().data());
}

void select_emoticons::lb_mouse_press_event(QString gif_name)
{
    //qDebug("select_emoticons::lb_mouse_press_event = %s",gif_name.toAscii().data());
    emit emoticons_lb_mouse_press_event(gif_name);
}

void select_emoticons::pb_add_emoticon_clicked()
{
    QString input_file = QFileDialog::getOpenFileName(this, "select file", "", tr("All (*.*)"));
    if(input_file.isEmpty() == true) {
        return;
    }
    QString custom_emoticons_path = "Custom_Emoticons/";
    emoticon_preview_label *label;
    QImage icon;
    QDir dir(custom_emoticons_path);
    QFile file;

    if (!dir.exists()) {
        QDir().mkdir(custom_emoticons_path);
    }
    file.setFileName(input_file);
    //qDebug("input_file = %s",input_file.toAscii().data());

    label = new emoticon_preview_label(m_custom_emoticon_tab);
    if(input_file.endsWith(".gif", Qt::CaseInsensitive)
            || input_file.endsWith(".jpg", Qt::CaseInsensitive)
            || input_file.endsWith(".png", Qt::CaseInsensitive)) {

        if(!icon.load(input_file)) {
            qDebug("111111111111");
            return;
        }
        label->m_label_gif_name = input_file;

        if(icon.width() > PREVIEW_ICON_SIZE || icon.height() > PREVIEW_ICON_SIZE) {
            icon = icon.scaled(PREVIEW_ICON_SIZE,PREVIEW_ICON_SIZE, Qt::KeepAspectRatio);
        }

        label->setMinimumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
        label->setMaximumSize(PREVIEW_ICON_SIZE, PREVIEW_ICON_SIZE);
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        label->setPixmap(QPixmap::fromImage(icon));
        label->move(PREVIEW_GRID_SIZE*(m_custom_emoticon_count%ONE_LINE_ICON_NUM)+2,
                    (m_custom_emoticon_count/ONE_LINE_ICON_NUM)*PREVIEW_GRID_SIZE+2);

        //m_emoticon_list[input_file] = label;

        connect(label,SIGNAL(lb_mouse_enterEvent(QString, QPoint, QSize)),
                SLOT(lb_mouse_enterEvent(QString, QPoint, QSize)));
        connect(label,SIGNAL(lb_mouse_leaveEvent(QString)),
                SLOT(lb_mouse_leaveEvent(QString)));
        //generate new file name and copy
        QString new_file_name;
        QFile new_file;
        do {
            QString name;
            //qrand()
            QByteArray byte_array;
            byte_array.clear();
            for(int i=0; i<16; i++) {
                byte_array.push_back(qrand()%10 + '0');
            }
            name.clear();
            name.append(byte_array);
            new_file_name = custom_emoticons_path + name + input_file.right(4);
            //qDebug("new_file_name = %s",new_file_name.toAscii().data());

            new_file.setFileName(new_file_name);
        }while(new_file.exists());

        if(!file.copy(new_file_name)) {
            qDebug("fail to copy emoticons file");
        }
        m_custom_emoticon_count++;
    }
}
