#include "chat_dlg.h"
#include "ui_chat_dlg.h"
#include <QDropEvent>
#include <QScrollBar>
#include <QUrl>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSizePolicy>
#include <QPalette>
#include <QCryptographicHash>
#include <QThread.h>
#include <QElapsedTimer>

chat_dlg::chat_dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chat_dlg)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    m_ctrl_key_down = false;

    m_name = "Name";
    m_barejid = "Jid";
    m_mood = "Mood";
    m_status = "Status";
    m_force_close = false;
    m_dlg_qxmpp_client = NULL;
    m_dlg_transfer_manager = NULL;

    connect(ui->te_msg_input,SIGNAL(enter_key_pressed()),
            SLOT(chat_input_edit_enter_key_pressed ()));

    ui->cb_send_key_mode->addItem("CTRL + Enter");
    /*ui->cb_send_key_mode->addItem("Enter");*/
    ui->pgb_file_transfer->setVisible(false);
    ui->pb_cancle_transfer->setVisible(false);

    //emoicons
    m_w_emoticons = new select_emoticons(this);
    m_w_emoticons->setWindowFlags(Qt::Popup);
    //qDebug("m_w_emoticons->m_emoticon_list.size() = %d", m_w_emoticons->m_emoticon_list.size());
    int height = (m_w_emoticons->m_emoticon_list.size()/10 + 1) * 42 + 4;
    m_w_emoticons->setMinimumSize(428, height);
    m_w_emoticons->setMaximumSize(428, height);
    m_w_emoticons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_w_emoticons->m_tab_widget->setMinimumSize(424,height-2);
    m_w_emoticons->m_tab_widget->setMaximumSize(424,height-2);
    m_w_emoticons->setStyleSheet("background-color:white;");
    m_w_emoticons->setVisible(true);
    m_w_emoticons->setVisible(false);

    connect(m_w_emoticons, SIGNAL(emoticons_lb_mouse_press_event(QString)),
            SLOT(emoticons_lb_mouse_press_event(QString)));

    setWindowFlags(Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);


    QDir dir(EMOTICONS_IMAGES_DIR_NAME);
    if (!dir.exists()) {
        QDir().mkdir(EMOTICONS_IMAGES_DIR_NAME);
    }
}

chat_dlg::~chat_dlg()
{
    delete ui;
}

void chat_dlg::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        qDebug("dragEnterEvent");
        event->acceptProposedAction();
    }
}

void chat_dlg::dropEvent(QDropEvent *event)
{
    QList<QUrl> url_list = event->mimeData()->urls();

    if(url_list.size() == 1) {
        //qDebug("dropEvent url: %s",url_list.at(0).toString().toAscii().data());
        QString file_name = url_list.at(0).toString().split("///").last();
        if(!file_name.isEmpty()) {
            dlg_send_file(file_name);
        }
    }
    //qDebug("dropEvent, end");
}

void chat_dlg::closeEvent(QCloseEvent *event)
{
    if(!m_force_close) {
        emit chat_dlg_close(m_barejid);
    }
    //qDebug("dlg closeEvent");
}

void chat_dlg::on_pb_cancle_clicked()
{
    this->close();
}

void chat_dlg::update_display()
{
    ui->lb_avatar->setPixmap(QPixmap::fromImage(m_avatar));
    ui->lb_name->setText(m_name);
    ui->lb_jid->setText(m_barejid);
    ui->lb_mood->setText(m_mood);
    ui->lb_status->setText(m_status);
}

void chat_dlg::keyPressEvent(QKeyEvent* e)
{
    ui->te_msg_input->setFocus();

    //qDebug("dlg keyPressEvent");
    if(e->key() == Qt::Key_Control) {
        m_ctrl_key_down = true;
        //qDebug("dlg Key_Control press");
    }
}
void chat_dlg::keyReleaseEvent(QKeyEvent* e)
{
    //qDebug("dlg keyReleaseEvent");
    if(e->key() == Qt::Key_Control) {
        m_ctrl_key_down = false;
        //qDebug("dlg Key_Control release");
    }
}
void chat_dlg::chat_input_edit_enter_key_pressed()
{
    if(m_ctrl_key_down) {
        ui->pb_send_msg->click();
    }
    //qDebug("%s :dlg chat_input_edit_enter_key_pressed",m_barejid.toAscii().data());
}
void chat_dlg::messageReceived(const QXmppMessage& msg)
{
    QString stamp;
    QDateTime dt;
    QTime time= QTime::currentTime();

    if(m_name.isEmpty()) {
        stamp = m_barejid;
    } else {
        stamp = m_name;
    }

    dt = msg.stamp();
    if(dt.isNull()) {
        stamp = stamp + " - " + time.toString("hh:mm:ss") + " :\r\n";
    } else {
        stamp = stamp + " - " + dt.toLocalTime().toString("hh:mm:ss") + " :";
    }

    QTextCursor cur = ui->te_chat_content->textCursor();
    cur.movePosition(QTextCursor::End);
    ui->te_chat_content->setTextCursor(cur);

    //qDebug("dlg messageReceived : %s", msg.body().toAscii().data());
    ui->te_chat_content->append(stamp);
    //ui->te_chat_content->append(msg.body());
    QString message = msg.body();
    ui->te_chat_content->insertHtml(message);
    if(message.contains(QRegExp("<img src=\".*\" />"))) {
        creat_animation_movie(message);
    }
    ui->te_chat_content->append("");

    ui->te_chat_content->verticalScrollBar()->setSliderPosition(1024*1024);
    //ui->te_chat_content->setCursor();

}
void chat_dlg::on_pb_send_msg_clicked()
{
    if(!m_dlg_qxmpp_client) {
        qDebug("on_pb_send_msg_clicked error: m_dlg_qxmpp_client is NULL");
        return;
    }

    if(!ui->te_msg_input->toPlainText().isEmpty()) {
        QTextCursor cur = ui->te_chat_content->textCursor();
        cur.movePosition(QTextCursor::End);
        ui->te_chat_content->setTextCursor(cur);
        QTime time= QTime::currentTime();
        QString note = m_barejid + " - " + time.toString("hh:mm:ss") + " :";
        ui->te_chat_content->append(note);
        QString html_msg = ui->te_msg_input->toHtml();
        if(html_msg.contains(QRegExp("<img src=\".*\" />"))) {
            //qDebug("insertHtml");
            send_emoticon_file(html_msg);
            ui->te_chat_content->append("");
            ui->te_chat_content->insertHtml(html_msg);

            creat_animation_movie(html_msg);
            ui->te_chat_content->append("");
            m_dlg_qxmpp_client->sendMessage(m_barejid, ui->te_msg_input->toHtml());/**/
        } else {
            //qDebug("toPlainText");
            ui->te_chat_content->append(ui->te_msg_input->toPlainText());
            m_dlg_qxmpp_client->sendMessage(m_barejid, ui->te_msg_input->toPlainText());
        }
        ui->te_chat_content->append("");
        ui->te_chat_content->verticalScrollBar()->setSliderPosition(1024*1024);
        ui->te_msg_input->del_all_movie();
        ui->te_msg_input->clear();
    }
}

QStringList chat_dlg::read_image_name_from_html(QString html)
{
    //QStringList files;
    int i;

    QStringList file_list = html.split(QRegExp("<img src=\".*\" />"), QString::SkipEmptyParts);
    for(i=0; i<file_list.size(); i++) {
        //qDebug("*** name = %s", file_list.at(i).toAscii().data());
        html.remove(file_list.at(i));
    }

    //qDebug("*** html = %s", html.toAscii().data());
    file_list = html.split(QRegExp("\".*/.*\""), QString::SkipEmptyParts);
    for(i=0; i<file_list.size(); i++) {
        //qDebug("*** name = %s", file_list.at(i).toAscii().data());
        html.remove(file_list.at(i));
    }
    //qDebug("### html = %s", html.toAscii().data());

    file_list = html.split("\"", QString::SkipEmptyParts);

    return file_list;
}
void chat_dlg::send_emoticon_file(QString html_msg)
{
    QStringList files;
    //qDebug("&&& html = %s", html.toAscii().data());

    m_dlg_transfer_manager->setSupportedMethods(QXmppTransferJob::AnyMethod);
    files = read_image_name_from_html(html_msg);
    QElapsedTimer t;
    t.start();
    for(int i=0; i<files.size(); i++) {
        //qDebug("SYS_SEND_EMOTICON_MSG_FLAG");
        m_dlg_qxmpp_client->sendMessage(m_barejid, SYS_SEND_EMOTICON_MSG_FLAG);
        t.restart();
        while(t.elapsed()<30) {
            QCoreApplication::processEvents();
        }
        //qDebug("m_dlg_transfer_manager->sendFile");
        m_dlg_transfer_manager->sendFile(m_barejid + "/QXmpp", files.at(i), QXmppUtils::generateStanzaHash());
        t.restart();
        while(t.elapsed()<100) {
            QCoreApplication::processEvents();
        }
    }
}
void chat_dlg::creat_animation_movie(QString html)
{
    QStringList files;
    //qDebug("&&& html = %s", html.toAscii().data());

    files = read_image_name_from_html(html);
    for(int i=0; i<files.size(); i++) {
        ui->te_chat_content->addAnimation(QUrl(files.at(i)),files.at(i));
    }
}

void chat_dlg::emoticons_lb_mouse_press_event(QString gif_name)
{
    QByteArray file_bytes, Sha1_hash_bytes;
    QString Emoticons_Image_dir_file;

    m_w_emoticons->setVisible(false);

    QFile gif_file(gif_name);
    if(!gif_file.exists()) {
        return;
    }

    gif_file.open(QIODevice::ReadOnly);
    file_bytes = gif_file.readAll();
    gif_file.close();
    Sha1_hash_bytes = QCryptographicHash::hash(file_bytes ,QCryptographicHash::Sha1);
    //qDebug("Sha1_hash_bytes = %s",QString(Sha1_hash_bytes.toHex()).toAscii().data());

    Emoticons_Image_dir_file = QString(EMOTICONS_IMAGES_DIR_NAME)
                             + QString("/")
                             + QString(Sha1_hash_bytes.toHex())
                             + gif_name.right(4);
    //qDebug("Emoticons_Image_dir_file = %s",Emoticons_Image_dir_file.toAscii().data());

    QFile sha1_file(Emoticons_Image_dir_file);
    if(!sha1_file.exists()) {

    }
    gif_file.copy(Emoticons_Image_dir_file);

    QString html = "<img src='%1'/>";
    ui->te_msg_input->insertHtml(html.arg(Emoticons_Image_dir_file));
    ui->te_msg_input->addAnimation(QUrl(Emoticons_Image_dir_file), Emoticons_Image_dir_file);

    //qDebug("chat_dlg::emoticons_lb_mouse_press_event : %s",gif_name.toAscii().data());
}

void chat_dlg::on_pb_send_file_clicked()
{
    if(!m_dlg_transfer_manager) {
        qDebug("on_pb_send_msg_clicked error: m_dlg_qxmpp_client is NULL");
        return;
    }

    QString input_file = QFileDialog::getOpenFileName(this, "select file", "", tr("All (*.*)"));
    if(input_file.isEmpty() == true) {
        return;
    }
    //qDebug("input_file = %s, to jid = %s", input_file.toAscii().data(),m_barejid.toAscii().data());
    dlg_send_file(input_file);
}
void chat_dlg::dlg_send_file(QString file_name)
{
    m_dlg_transfer_manager->setSupportedMethods(QXmppTransferJob::AnyMethod);

    m_transfer_job = m_dlg_transfer_manager->sendFile(m_barejid + "/QXmpp", file_name,  QXmppUtils::generateStanzaHash());

    connect(m_transfer_job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(error(QXmppTransferJob::Error)));
    connect(m_transfer_job, SIGNAL(finished()), this, SLOT(fileFinished()));
    connect(m_transfer_job, SIGNAL(progress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));

    ui->pb_send_file->setEnabled(false);
    ui->pgb_file_transfer->setVisible(true);
    ui->pb_cancle_transfer->setVisible(true);
}
void chat_dlg::fileFinished() {
    ui->te_chat_content->append("");
    ui->te_chat_content->append("NOTE: file transfer finished!");
    ui->te_chat_content->append("");
    ui->te_chat_content->verticalScrollBar()->setSliderPosition(1024*1024);

    ui->pgb_file_transfer->setVisible(false);
    ui->pb_cancle_transfer->setVisible(false);
    ui->pb_send_file->setEnabled(true);
    //qDebug("fileFinished");
}

void chat_dlg::error(QXmppTransferJob::Error error) {
    QString error_msg;
    if(error & QXmppTransferJob::NoError) {
        error_msg = tr("no error");
    } else if(error & QXmppTransferJob::AbortError) {
        error_msg = tr("abort error");
    } else if(error & QXmppTransferJob::FileAccessError) {
        error_msg = tr("file access error");
    } else if(error & QXmppTransferJob::FileCorruptError) {
        error_msg = tr("file corrupted error");
    } else if(error & QXmppTransferJob::ProtocolError) {
        error_msg = tr("protocol error");
    } else {
        error_msg = tr("Unknow error");
    }
    QMessageBox::warning(this, tr("Transfer file error"), "Error message: " + error_msg,
                QMessageBox::Yes);
    ui->pgb_file_transfer->setVisible(false);
    ui->pb_cancle_transfer->setVisible(false);
    ui->pb_send_file->setEnabled(true);
}

void chat_dlg::progress(qint64 done, qint64 total) {
    //qDebug() << "Transmission progress:" << done << "/" << total;
    ui->pgb_file_transfer->setMaximum(total);
    ui->pgb_file_transfer->setValue(done);
}

void chat_dlg::on_pb_smilies_clicked()
{
    QPoint emoticons_pos(ui->pb_smilies->x() + x() + 3,
             ui->pb_smilies->y() + y() - m_w_emoticons->minimumSize().height() + 30);
    m_w_emoticons->m_screen_pos = emoticons_pos;
    m_w_emoticons->update();
    m_w_emoticons->move(emoticons_pos);
    m_w_emoticons->setVisible(true);
    //qDebug("x = %d , y= %d, wx = %d , wy= %d",x(), y(), ui->pb_smilies->x(), ui->pb_smilies->y());
}


void chat_dlg::on_pb_cancle_transfer_clicked()
{
    m_transfer_job->abort();
    ui->pgb_file_transfer->setVisible(false);
    ui->pb_cancle_transfer->setVisible(false);
    ui->pb_send_file->setEnabled(true);
}
