#ifndef CHAT_INPUT_EDIT_H
#define CHAT_INPUT_EDIT_H

#include <QWidget>
#include <QTextEdit>
#include <QUrl>
#include <QMovie>

class chat_input_edit : public QTextEdit
{
    Q_OBJECT
public:
    explicit chat_input_edit(QWidget *parent = 0);

    void keyPressEvent ( QKeyEvent * e );
    void addAnimation(const QUrl& url, const QString& fileName);

    void del_all_movie();

    QHash<QMovie*, QUrl> urls;

signals:
    void enter_key_pressed();

public slots:

private slots:
    void animate();
};

#endif // CHAT_INPUT_EDIT_H
