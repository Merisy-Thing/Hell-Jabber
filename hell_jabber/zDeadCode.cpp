#if 0



QVariant friend_list_model::data(const QModelIndex &index, int role) const
{
    if ( (!index.isValid()) || (index.row() >= m_name_list.size()))
        return QVariant();

    if(role == Qt::UserRole) {
        //qDebug("Qt::UserRole");
        if(m_name_list.size() > index.row()) {
            return m_name_list.at(index.row());
        }
    } else if(role == Qt::UserRole + 1) {
        //qDebug("Qt::UserRole + 1");
        if(m_avatar_list.size() > index.row()) {
            if(m_avatar_list.at(index.row()).isNull()) {
                qDebug("m_avatar_list.at(index.row()).isNull(), row = %d", index.row());
            }
            return m_avatar_list.at(index.row());
        }
    } else if(role == Qt::UserRole + 2) {
        if(m_map_jid_status_msg.contains(m_name_list.at(index.row()))) {
            return m_map_jid_status_msg[m_name_list.at(index.row())];
            //return tr("status message");
        } else {
            return tr("No status message");
        }
    }

    return QVariant();
}

















#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "stdio.h"
#include <QDir>
#include <QFile>
#include <QListView>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QImage>

#include <QBuffer>
#include <QImageReader>
#include <QByteArray>

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
                    SIGNAL(vCardReceived(const QXmppVCardIq&)),
                    SLOT(vCardReceived(const QXmppVCardIq&)));
    Q_ASSERT(check);

    /*clientVCardReceived*/
    check = connect(&m_qxmpp_client->vCardManager(),
                    SIGNAL(clientVCardReceived()),
                    SLOT(clientVCardReceived()));
    Q_ASSERT(check);

    m_flm = new friend_list_model(this);
    m_fld = new friend_list_delegate(this);

    QString jid = name + "@" + host;
    m_my_jid = jid;
    m_qxmpp_client->connectToServer(jid, pwd);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::clientConnected()
{
    m_qxmpp_client->vCardManager().requestClientVCard();
    /*qDebug("clientConnected:: CONNECTED");*/
}
void MainWindow::presenceReceived(const QXmppPresence& presence)
{
    QString from = presence.from();

    QString name = m_qxmpp_client->rosterManager().getRosterEntry(from).name();
    if(name.isEmpty()) {
        name = from;
    }

    m_flm->m_map_jid_status_msg[name] = presence.status().statusText();

    //qDebug("presenceReceived, Presence.from() = %s, statusText = %s", Presence.from().toAscii().data(), Presence.status().statusText().toAscii().data());
}

void MainWindow::rosterReceived()
{
    QStringList list = m_qxmpp_client->rosterManager().getRosterBareJids();

    for(int i = 0; i < list.size(); ++i)
    {
        QString bareJid = list.at(i);
        m_friend_list.jid_list.push_back( bareJid );

        QString name = m_qxmpp_client->rosterManager().getRosterEntry(bareJid).name();
        if(name.isEmpty()) {
            m_friend_list.name_list.push_back( bareJid );
        } else {
            m_friend_list.name_list.push_back( name );
        }

        m_qxmpp_client->vCardManager().requestVCard(bareJid);
    }

    /*qDebug("rosterReceived()");*/
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

void MainWindow::vCardReceived(const QXmppVCardIq& vcard)
{
    QString from = vcard.from();
    QImage default_avatar("default_avatar.png", "PNG");
    QImage vcard_avatar;
    int i;

    if(from.isEmpty()) {
        return;//Fixme
    }

    m_map_jid_vcard[from] = vcard;

    if(m_map_jid_vcard.size() >= m_friend_list.name_list.size()) {
        for(i=0; i<m_friend_list.jid_list.size(); i++) {
            qDebug("jid_list.at(i) = %s", m_friend_list.jid_list.at(i).toAscii().data());
            if(m_map_jid_vcard.contains(m_friend_list.jid_list.at(i))) {
                vcard_avatar = get_image_from_bytearray(m_map_jid_vcard[m_friend_list.jid_list.at(i)].photo());
                if(vcard_avatar.isNull()) {
                    m_friend_list.avatar_list.push_back(default_avatar);
                } else {
                    m_friend_list.avatar_list.push_back(vcard_avatar);
                }
            }
        }
        //qDebug("avatar_list size = %d",m_friend_list.avatar_list.size());

        m_flm->set_friend_list_param(m_friend_list);
        ui->lv_friend_list->setModel(m_flm);
        ui->lv_friend_list->setItemDelegate(m_fld);

        //ui->listView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
        ui->lv_friend_list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    }

    //qDebug("vCardReceived()");
}
//const QXmppVCardIq& clientVCard() const;
void MainWindow::clientVCardReceived()
{
    QImage default_avatar("default_avatar.png", "PNG");
    QImage my_avatar;
    QString status = "unknow";

    //avatar
    m_my_vcard = m_qxmpp_client->vCardManager().clientVCard();
    my_avatar = get_image_from_bytearray(m_my_vcard.photo());
    if(my_avatar.isNull()) {
        my_avatar = default_avatar;
    }
    ui->pb_avatar_btn->setIcon(QIcon(QPixmap::fromImage(my_avatar)));
    ui->pb_avatar_btn->setIconSize(QSize(42,42));

    //name
    if(!m_qxmpp_client->rosterManager().getRosterEntry(m_my_jid).name().isEmpty()) {
        ui->lb_client_name->setText(m_qxmpp_client->rosterManager().getRosterEntry(m_my_jid).name());
    } else {
        ui->lb_client_name->setText(m_my_jid);
    }

    //status
    //qDebug("status = %d", m_qxmpp_client->clientPresence().status());
    switch(m_qxmpp_client->clientPresence().status().type()) {
    case QXmppPresence::Status::Online:
    case QXmppPresence::Status::Chat:
        status = "Online";
        break;
    case QXmppPresence::Status::Away:
    case QXmppPresence::Status::XA:
        status = "Away";
        break;
    case QXmppPresence::Status::DND:
        status = "Do Not Disturb";
        break;
    case QXmppPresence::Status::Invisible:
    case QXmppPresence::Status::Offline:
        status = "Offline";
        break;
    }
    ui->lb_client_status->setText(status);

    //status message
    status = m_qxmpp_client->clientPresence().status().statusText();
    if(status.isEmpty()) {
        status = tr("No status message");
    }
    ui->le_client_status_message->setText(status);

    qDebug("clientVCardReceived");
}


void MainWindow::on_lv_friend_list_doubleClicked(const QModelIndex &index)
{

}




























void MainWindow::presenceReceived(const QXmppPresence& presence)
{
    QString from = presence.from();
    QString mood;
    QString message;
    ui->le_contact_find->setText(from);
    switch(presence.type())
    {
    case QXmppPresence::Subscribe:
        {
            message = "<B>%1</B> wants to subscribe";

            int retButton = QMessageBox::question(
                    this, "Contact Subscription", message.arg(from),
                    QMessageBox::Yes, QMessageBox::No);

            switch(retButton)
            {
            case QMessageBox::Yes:
                {
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
            case QMessageBox::No:
                {
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
        qDebug("1");
    case QXmppPresence::Unavailable:
        qDebug("2");
        mood = presence.status().statusText();
        if(mood.isEmpty()) {
            mood = tr("No mood message");
        }
        ui->le_client_status_message->setText(mood);
        qDebug("presenceReceived: from:%s,mood:%s",from.toAscii().data(), mood.toAscii().data());
        if(!m_flm->flm_update_mood("hell@192.168.2.168", mood)) {
            qDebug("flm_update_mood fail");
        }
        break;
    default:
        return;
        break;
    }

    if(message.isEmpty())
        return;

    QMessageBox::information(this, "Contact Subscription", message.arg(from),
            QMessageBox::Ok);
    qDebug("** presenceReceived");
}










#endif
