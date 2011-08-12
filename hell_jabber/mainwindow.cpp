
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdio.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QImage>

#include <QBuffer>
#include <QImageReader>
#include <QByteArray>
#include <QMessageBox>
#include<QScrollBar>

#include "QXmppMessage.h"
#include "QXmppRosterManager.h"

MainWindow::MainWindow(QWidget *parent,
                       QString name,
                       QString pwd,
                       QString host)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->menuBar->setVisible(false);

    QXmppLogger::getLogger()->setLoggingType(QXmppLogger::StdoutLogging);
    m_qxmpp_client = new QXmppClient;
    m_transfer_manager = new QXmppTransferManager;
    m_qxmpp_client->addExtension(m_transfer_manager);

    ui->pgb_receive_file->setVisible(false);
    ui->pb_abort_receive_file->setVisible(false);
    m_file_received_on_the_way = false;

    /*connect*/
    bool check = connect(m_qxmpp_client, SIGNAL(connected()),
        SLOT(clientConnected()));
    Q_ASSERT(check);

    /*presenceReceived*/
    check = connect(m_qxmpp_client, SIGNAL(presenceReceived(const QXmppPresence&)),
                    SLOT(presenceReceived(const QXmppPresence&)));
    Q_ASSERT(check);

    /*rosterReceived*/
    check = connect(&m_qxmpp_client->rosterManager(),
                    SIGNAL(rosterReceived()),
                    SLOT(rosterReceived()));
    Q_ASSERT(check);

    /*vCardReceived*/
    check = connect(&m_qxmpp_client->vCardManager(),
                    SIGNAL(vCardReceived(const QXmppVCard&)),
                    SLOT(vCardReceived(const QXmppVCard&)));
    Q_ASSERT(check);

    /*clientVCardReceived*/
    check = connect(&m_qxmpp_client->vCardManager(),
                    SIGNAL(clientVCardReceived()),
                    SLOT(clientVCardReceived()));
    Q_ASSERT(check);

    /*message Received*/
    check = connect(m_qxmpp_client, SIGNAL(messageReceived(const QXmppMessage&)),
                    SLOT(messageReceived(const QXmppMessage&)));
    Q_ASSERT(check);

    /* began chat */
    check = connect(ui->lv_friend_list,
                    SIGNAL(doubleClicked ( const QModelIndex &)),
                    SLOT(flm_item_doubleClicked ( const QModelIndex &)));
    Q_ASSERT(check);

    /* item selected */
    check = connect(ui->lv_friend_list,
                    SIGNAL(clicked ( const QModelIndex &)),
                    SLOT(flm_item_clicked ( const QModelIndex &)));
    Q_ASSERT(check);

    /* File transfer */
    check = connect(m_transfer_manager,
                    SIGNAL(fileReceived (QXmppTransferJob *)),
                    SLOT(transfer_file_Received (QXmppTransferJob *)));
    Q_ASSERT(check);

    m_client_ready = false;
    QString barejid = name + "@" + host;
    m_qxmpp_client->connectToServer(barejid, pwd);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMap<QString, chat_dlg*>::iterator ite;
    chat_dlg* dlg;

    if(m_barejid_chatDlg.size() > 0) {
        for( ite = m_barejid_chatDlg.begin(); ite != m_barejid_chatDlg.end(); ite++) {
            dlg = ite.value();
            dlg->m_force_close = true;
            dlg->close();
        }
        m_barejid_chatDlg.clear();
    }
    //qDebug("MainWindow closeEvent");
}

void MainWindow::transfer_file_Received (QXmppTransferJob *offer_job)
{
    /*
    qDebug("transfer_file_Received = %s, jid = %s, sid = %s",
           offer_job->fileName().toAscii().data(),
           offer_job->jid().toAscii().data(),
           offer_job->sid().toAscii().data());
    */
    if(m_file_received_on_the_way) {
        offer_job->abort();
    }
    m_file_received_on_the_way = true;

    QString from_name = m_qxmpp_client->rosterManager().getRosterEntry(jidToBareJid(offer_job->jid())).name();;
    QString message = tr("<B>%1</B>  send a file to you, File name: <B>%2</B>, Accept or NOT ?!");
    if(from_name.isEmpty()) {
        from_name = jidToBareJid(offer_job->jid());
    }

    int retButton = QMessageBox::question(
            this, tr("Transfer file Received"), message.arg(from_name).arg(offer_job->fileName()),
            QMessageBox::Yes, QMessageBox::No);
    switch(retButton){
    case QMessageBox::Yes: {
            //qDebug("QMessageBox::Yes");
            QString receive_file_name;
            do {
                receive_file_name = QFileDialog::getSaveFileName(this, tr("Save File"), offer_job->fileName(),
                                            tr("All (*.*)"));
                if(!receive_file_name.isEmpty()) {
                    break;
                }

            }while(QMessageBox::No == QMessageBox::question(this,
                                                            tr("Abort"),tr("Abort receiv file confirm ?!"),
                                                            QMessageBox::Yes, QMessageBox::No));
            if(!receive_file_name.isEmpty()) {
                //m_receive_file
                m_receive_file.setFileName(receive_file_name);
                if (!m_receive_file.open(QIODevice::WriteOnly)) {
                    m_file_received_on_the_way = false;
                    QMessageBox::warning(this, tr("Open file error"), "Open file fail",
                                QMessageBox::Yes);
                    break;
                }

                bool check = connect(offer_job, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(transfer_file_slotError(QXmppTransferJob::Error)));
                Q_ASSERT(check);
                check = connect(offer_job, SIGNAL(finished()), this, SLOT(transfer_file_slotFinished()));
                Q_ASSERT(check);
                check = connect(offer_job, SIGNAL(progress(qint64,qint64)), this, SLOT(transfer_file_slotProgress(qint64,qint64)));
                Q_ASSERT(check);

                ui->pgb_receive_file->reset();
                ui->pgb_receive_file->setVisible(true);
                ui->pb_abort_receive_file->setVisible(true);

                offer_job->accept(&m_receive_file);
            } else {
                m_file_received_on_the_way = false;
            }
        }
        break;
    case QMessageBox::No: {
            //qDebug("QMessageBox::No");
            offer_job->abort();
            m_file_received_on_the_way = false;
        }
        break;
    default:
            m_file_received_on_the_way = false;
            offer_job->abort();
        break;
    }
}

void MainWindow::transfer_file_slotFinished()
{
    m_receive_file.close();
    m_file_received_on_the_way = false;
    ui->pgb_receive_file->setVisible(false);
    ui->pb_abort_receive_file->setVisible(false);
}
void MainWindow::transfer_file_slotProgress(qint64 done,qint64 total)
{
    ui->pgb_receive_file->setMaximum(total);
    ui->pgb_receive_file->setValue(done);
}
void MainWindow::transfer_file_slotError(QXmppTransferJob::Error error)
{
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

    m_file_received_on_the_way = false;
}


void MainWindow::flm_item_doubleClicked ( const QModelIndex & index)
{
    if(index.isValid()) {
        start_chat_dlg(index.row());
    }
    //qDebug("flm_item_doubleClicked row = %d", index.row());
}

void MainWindow::flm_item_clicked ( const QModelIndex & index)
{
    if(index.isValid()) {
        m_flm->m_item_draw_rect = index.row();
        ui->lv_friend_list->setUpdatesEnabled(false);
        ui->lv_friend_list->setUpdatesEnabled(true);
        //ui->lv_friend_list->repaint();
    }
    //qDebug("flm_item_clicked row = %d", index.row());
}

void MainWindow::clientConnected()
{
    m_qxmpp_client->vCardManager().requestClientVCard();
    //qDebug("clientConnected:: CONNECTED");
}

void MainWindow::messageReceived(const QXmppMessage& msg)
{
    QString from = jidToBareJid(msg.from());

    if(m_barejid_chatDlg.contains(from)) {
        m_barejid_chatDlg[from]->messageReceived(msg);
    } else {
        qDebug("from %s : %s",jidToBareJid(from).toAscii().data(), msg.body().toAscii().data());
    }
}

void MainWindow::presenceReceived(const QXmppPresence& presence)
{
    QString from, mood, message;

    from = jidToBareJid(presence.from());
    switch(presence.type()){
    case QXmppPresence::Subscribe:{
            message = "<B>%1</B> wants to subscribe";
            int retButton = QMessageBox::question(
                    this, "Contact Subscription", message.arg(from),
                    QMessageBox::Yes, QMessageBox::No);
            switch(retButton){
            case QMessageBox::Yes:{
                    QXmppPresence subscribed;
                    subscribed.setTo(from);
                    subscribed.setType(QXmppPresence::Subscribed);
                    m_qxmpp_client->sendPacket(subscribed);

                    // reciprocal subscription
                    QXmppPresence subscribe;
                    subscribe.setTo(from);
                    subscribe.setType(QXmppPresence::Subscribe);
                    m_qxmpp_client->sendPacket(subscribe);
                }
                break;
            case QMessageBox::No:{
                    QXmppPresence unsubscribed;
                    unsubscribed.setTo(from);
                    unsubscribed.setType(QXmppPresence::Unsubscribed);
                    m_qxmpp_client->sendPacket(unsubscribed);
                }
                break;
            default:
                break;
            }
            return;
        }
        break;
    case QXmppPresence::Subscribed:
        message = "<B>%1</B> accepted your request";
        break;
    case QXmppPresence::Unsubscribe:
        message = "<B>%1</B> unsubscribe";
        break;
    case QXmppPresence::Unsubscribed:
        message = "<B>%1</B> unsubscribed";
        break;
    case QXmppPresence::Available:
        //qDebug("1");
    case QXmppPresence::Unavailable:
        //qDebug("2");
        mood = presence.status().statusText();
        if(mood.isEmpty()) {
            mood = tr("No mood message");
        }
        if(m_client_ready) {
            //qDebug("presenceReceived: from:%s,mood:%s",from.toAscii().data(), mood.toAscii().data());
            if(!m_flm->flm_update_mood(from, mood)) {
                qDebug("flm_update_mood fail");
            }
        } else {
            m_jid_presence[from] = mood;
        }
        break;
    default:
        return;
        break;
    }

    //qDebug("** presenceReceived");
}

void MainWindow::rosterReceived()
{
    friend_list_element_t element;
    QStringList list = m_qxmpp_client->rosterManager().getRosterBareJids();
    QStringList barejid_list;

    //Creat barejid list
    for(int i = 0; i < list.size(); ++i)
    {
        QString bareJid = list.at(i);
        barejid_list.push_back( bareJid );
    }
    m_flm = new friend_list_model(ui->lv_friend_list, barejid_list, this);
    m_fld = new friend_list_delegate(this);

    //Creat friend list model's element
    for(int i = 0; i < barejid_list.size(); ++i) {
        QString name = m_qxmpp_client->rosterManager().getRosterEntry(barejid_list.at(i)).name();
        if(name.isEmpty()) {
            element.name = barejid_list.at(i);
        } else {
            element.name = name;
        }
        m_flm->flm_add_element(barejid_list.at(i), element);
        if(m_jid_presence.contains(barejid_list.at(i))) {
            m_flm->flm_update_mood(barejid_list.at(i), m_jid_presence[barejid_list.at(i)]);
        } else {
            m_flm->flm_update_mood(barejid_list.at(i), tr("Offine line"));
        }
    }
    if(m_jid_presence.size() > 0) {
        m_jid_presence.clear();
    }

    //Request vcard
    for(int i = 0; i < barejid_list.size(); ++i) {
        m_qxmpp_client->vCardManager().requestVCard(barejid_list.at(i));
    }

    ui->lv_friend_list->setModel(m_flm);
    ui->lv_friend_list->setItemDelegate(m_fld);

    //ui->listView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    ui->lv_friend_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);//ScrollPerItem

    m_client_ready = true;
    //qDebug("rosterReceived()");
}
QImage MainWindow::get_image_from_bytearray(const QByteArray& image)
{
    QBuffer buffer;

    //qDebug("image.size() = %d",image.size());
    buffer.setData(image);
    buffer.open(QIODevice::ReadOnly);
    QImageReader imageReader(&buffer);
    return  imageReader.read();
}

void MainWindow::vCardReceived(const QXmppVCard& vcard)
{
    QString from = vcard.from();
    QImage default_avatar("default_avatar.png", "PNG");
    QImage vcard_avatar;

    if(from.isEmpty()) {
        return;//Fixme
    }

    if(vcard.photo().isNull() || vcard.photo().isEmpty()) {
        vcard_avatar = default_avatar;
    } else {
        vcard_avatar = get_image_from_bytearray(vcard.photo());
    }
    if(!m_flm->flm_update_avatar(from, vcard_avatar)) {
        qDebug("flm_update_avatar fail");
    }

    //qDebug("vCardReceived()");
}
//const QXmppVCardIq& clientVCard() const;
void MainWindow::clientVCardReceived()
{
    QImage default_avatar("default_avatar.png", "PNG");
    QImage my_avatar;
    QString status = tr("unknow");

    //avatar
    my_avatar = get_image_from_bytearray(m_qxmpp_client->vCardManager().clientVCard().photo());
    if(my_avatar.isNull()) {
        my_avatar = default_avatar;
    }
    ui->pb_avatar_btn->setIcon(QIcon(QPixmap::fromImage(my_avatar)));
    ui->pb_avatar_btn->setIconSize(QSize(42,42));

    //name
    ui->lb_client_name->setText(m_qxmpp_client->configuration().jidBare());

    //status
    //qDebug("status = %d", m_qxmpp_client->clientPresence().status());
    switch(m_qxmpp_client->clientPresence().status().type()) {
    case QXmppPresence::Status::Online:
    case QXmppPresence::Status::Chat:
        status = tr("Online");
        break;
    case QXmppPresence::Status::Away:
    case QXmppPresence::Status::XA:
        status = tr("Away");
        break;
    case QXmppPresence::Status::DND:
        status = tr("Do Not Disturb");
        break;
    case QXmppPresence::Status::Invisible:
    case QXmppPresence::Status::Offline:
        status = tr("Offline");
        break;
    }
    ui->lb_client_status->setText(status);

    //status message
    status = m_qxmpp_client->clientPresence().status().statusText();
    if(status.isEmpty()) {
        status = tr("No status message");
    }
    ui->le_client_status_message->setText(status);

    //qDebug("clientVCardReceived");
}

void MainWindow::chat_dlg_close(QString barejid)
{
    //qDebug("chat_dlg_close jid = %s", jid.toAscii().data());
    //qDebug("m_barejid_chatDlg.size() = %d", m_barejid_chatDlg.size());
    if(m_barejid_chatDlg.contains(barejid)) {
        m_barejid_chatDlg.erase(m_barejid_chatDlg.find(barejid));
    }
    //qDebug("m_barejid_chatDlg.size() = %d", m_barejid_chatDlg.size());
}

void MainWindow::start_chat_dlg(int index)
{
    chat_dlg *chatDlg;
    QString name, barejid;

    //qDebug("111");
    if(index >= m_flm->m_barejid_list.size()) {
        return;
    }
    //qDebug("222");

    barejid = m_flm->m_barejid_list.at(index);

    if(m_barejid_chatDlg.contains(barejid)) {
        chatDlg = m_barejid_chatDlg[barejid];
        chatDlg->activateWindow();
    } else {
        chatDlg = new chat_dlg;
        chatDlg->m_avatar = m_flm->m_map_jid_element[barejid]->avatar;
        chatDlg->m_barejid = barejid;
        chatDlg->m_mood = m_flm->m_map_jid_element[barejid]->mood;
        chatDlg->m_status = m_flm->m_map_jid_element[barejid]->status;
        name = m_qxmpp_client->rosterManager().getRosterEntry(barejid).name();
        if(name.isEmpty()) {
            name = tr("No name");
        }
        chatDlg->m_name = name;
        chatDlg->m_dlg_qxmpp_client = m_qxmpp_client;
        chatDlg->m_dlg_transfer_manager = m_transfer_manager;
        chatDlg->update_display();
        chatDlg->setAttribute(Qt::WA_DeleteOnClose);
        chatDlg->show();

        connect(chatDlg, SIGNAL(chat_dlg_close(QString)), SLOT(chat_dlg_close(QString)));

        m_barejid_chatDlg[barejid] = chatDlg;
    }
}

/* self chat */
void MainWindow::on_pb_avatar_btn_clicked()
{
    chat_dlg *chatDlg;
    QString barejid = m_qxmpp_client->configuration().jidBare();

    if(m_barejid_chatDlg.contains(barejid)) {
        m_barejid_chatDlg[barejid]->activateWindow();
        return;
    }

    chatDlg = new chat_dlg;
    //chatDlg->m_avatar = NULL;
    chatDlg->m_barejid = barejid;
    chatDlg->m_mood = "Hell";
    chatDlg->m_status = "I am OK";
    chatDlg->m_name = "MY self";
    chatDlg->m_dlg_qxmpp_client = m_qxmpp_client;
    chatDlg->m_dlg_transfer_manager = m_transfer_manager;
    chatDlg->update_display();
    chatDlg->setAttribute(Qt::WA_DeleteOnClose);
    chatDlg->show();

    connect(chatDlg, SIGNAL(chat_dlg_close(QString)), SLOT(chat_dlg_close(QString)));

    m_barejid_chatDlg[barejid] = chatDlg;
}
