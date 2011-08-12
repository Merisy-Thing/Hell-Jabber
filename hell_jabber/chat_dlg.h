#ifndef CHAT_DLG_H
#define CHAT_DLG_H

#include <QDialog>
#include "QXmppClient.h"
#include "QXmppMessage.h"
#include "chat_input_edit.h"
#include "QXmppTransferManager.h"
#include "QXmppUtils.h"
#include "select_emoticons.h"

namespace Ui {
    class chat_dlg;
}

class chat_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit chat_dlg(QWidget *parent = 0);
    ~chat_dlg();

    void update_display();
    void messageReceived(const QXmppMessage& msg);

    QImage  m_avatar;
    QString m_name;
    QString m_barejid;
    QString m_mood;
    QString m_status;

    QXmppClient *m_dlg_qxmpp_client;
    QXmppTransferManager *m_dlg_transfer_manager;
    QXmppTransferJob *m_transfer_job;

    bool m_force_close;
    select_emoticons *m_w_emoticons;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

signals:
    void chat_dlg_close(QString jid);

private slots:
    void on_pb_cancle_clicked();
    void on_pb_send_msg_clicked();
    void chat_input_edit_enter_key_pressed();
    void on_pb_send_file_clicked();
    void fileFinished();
    void error(QXmppTransferJob::Error error);
    void progress(qint64 done, qint64 total);

    void on_pb_smilies_clicked();

    void emoticons_lb_mouse_press_event(QString gif_name);

private:
    Ui::chat_dlg *ui;

    bool m_ctrl_key_down;
    void dlg_send_file(QString file_name);
};

#endif // CHAT_DLG_H
