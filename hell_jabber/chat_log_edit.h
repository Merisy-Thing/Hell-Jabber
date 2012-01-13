#ifndef CHAT_LOG_EDIT_H
#define CHAT_LOG_EDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QUrl>
#include <QMovie>

class chat_log_edit : public QTextEdit
{
    Q_OBJECT
public:
    explicit chat_log_edit(QWidget *parent = 0);

    void addAnimation(const QUrl& url, const QString& fileName);
signals:

public slots:

private slots:
    void animate();

private:
    QHash<QMovie*, QUrl> urls;
};

#endif // CHAT_LOG_EDIT_H
