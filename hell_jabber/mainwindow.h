#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

#include "friend_list_model.h"
#include "friend_list_delegate.h"
#include "flm_listview.h"
#include "chat_dlg.h"

#include "QXmppClient.h"
#include "QXmppLogger.h"
#include "QXmppVCardManager.h"
#include "QXmppVCardIq.h"
#include "QXmppUtils.h"
#include "QXmppTransferManager.h"

namespace Ui {
    class MainWindow;
}
class friend_list_model;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,
                        QString name = "", QString pwd = "", QString host = "");
    ~MainWindow();

public:
    QXmppClient *m_qxmpp_client;
    QXmppTransferManager *m_transfer_manager;

    friend_list_model *m_flm;
    friend_list_delegate *m_fld;


public:
    QImage get_image_from_bytearray(const QByteArray& image);
    void start_chat_dlg(int index);

public slots:
    void clientConnected();
    void rosterReceived();
    void vCardReceived(const QXmppVCardIq&);
    void clientVCardReceived();
    void presenceReceived(const QXmppPresence&);
    void messageReceived(const QXmppMessage& msg);

    void transfer_file_slotFinished();
    void transfer_file_slotError(QXmppTransferJob::Error);
    void transfer_file_slotProgress(qint64,qint64);

private:
    bool m_client_ready;
    bool m_file_received_on_the_way;
    QFile m_receive_file;
    QMap<QString, QString> m_jid_presence;
    QMap<QString, chat_dlg*> m_barejid_chatDlg;
    bool m_sys_send_image_file;

private slots:
    void on_pb_avatar_btn_clicked();
    void flm_item_doubleClicked ( const QModelIndex & index);
    void flm_item_clicked ( const QModelIndex & index);
    void chat_dlg_close(QString jid);
    void transfer_file_Received (QXmppTransferJob *offer);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
