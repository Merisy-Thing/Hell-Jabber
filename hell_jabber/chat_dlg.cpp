#include "chat_dlg.h"
#include "ui_chat_dlg.h"
#include <QDropEvent>
#include <QScrollBar>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QSizePolicy>
#include <QPalette>
#include <QCryptographicHash>

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

    connect(ui->te_typing_msg,SIGNAL(enter_key_pressed()),
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
    ui->te_typing_msg->setFocus();

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
        stamp = stamp + " - " + time.toString("hh:mm:ss") + " :";
    } else {
        stamp = stamp + " - " + dt.toLocalTime().toString("hh:mm:ss") + " :";
    }
    //qDebug("dlg messageReceived : %s", msg.body().toAscii().data());
    ui->te_chat_content->append(stamp);
    ui->te_chat_content->append(msg.body());
    ui->te_chat_content->append("");
    ui->te_chat_content->verticalScrollBar()->setSliderPosition(1024*1024);
}
void chat_dlg::on_pb_send_msg_clicked()
{
    if(!m_dlg_qxmpp_client) {
        qDebug("on_pb_send_msg_clicked error: m_dlg_qxmpp_client is NULL");
        return;
    }

    QString msg = ui->te_typing_msg->toPlainText();
    if(!msg.isEmpty()) {
        QTime time= QTime::currentTime();
        QString note = m_barejid + " - " + time.toString("hh:mm:ss") + " :";
        m_dlg_qxmpp_client->sendMessage(m_barejid, ui->te_typing_msg->toPlainText());/**/
        ui->te_chat_content->append(note);
        ui->te_chat_content->append(msg);
        ui->te_chat_content->append("");
        ui->te_chat_content->verticalScrollBar()->setSliderPosition(1024*1024);
        ui->te_typing_msg->clear();
    }
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

    m_transfer_job = m_dlg_transfer_manager->sendFile(m_barejid + "/QXmpp", file_name,  generateStanzaHash());

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
        error_msg = "no error";
    } else if(error & QXmppTransferJob::AbortError) {
        error_msg = "abort error";
    } else if(error & QXmppTransferJob::FileAccessError) {
        error_msg = "file access error";
    } else if(error & QXmppTransferJob::FileCorruptError) {
        error_msg = "file corrupted error";
    } else if(error & QXmppTransferJob::ProtocolError) {
        error_msg = "protocol error";
    } else {
        error_msg = "Unknow error";
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

void chat_dlg::emoticons_lb_mouse_press_event(QString gif_name)
{
    QByteArray file_bytes, Sha1_hash_bytes;

    m_w_emoticons->setVisible(false);

    QFile file(gif_name);
    if(!file.exists()) {
        return;
    }

    file.open(QIODevice::ReadOnly);
    file_bytes = file.readAll();
    file.close();
    Sha1_hash_bytes = QCryptographicHash::hash(file_bytes ,QCryptographicHash::Sha1);
    qDebug("Sha1_hash_bytes = %s",QString(Sha1_hash_bytes.toHex()).toAscii().data());

    qDebug("chat_dlg::emoticons_lb_mouse_press_event : %s",gif_name.toAscii().data());
}
