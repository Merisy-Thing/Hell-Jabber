#include "chat_input_edit.h"
#include <QKeyEvent>
#include <QTextEdit>

chat_input_edit::chat_input_edit(QWidget *parent) :
    QTextEdit(parent)
{
}

void chat_input_edit::keyPressEvent ( QKeyEvent * e )
{
    //qDebug("chat_input_edit keyPressEvent");
    if(e->key() == Qt::Key_Return) {
        emit enter_key_pressed();
    }
    QTextEdit::keyPressEvent(e);
}

void chat_input_edit::animate()
{
    if (QMovie* movie = qobject_cast<QMovie*>(sender())) {
        document()->addResource(QTextDocument::ImageResource,
                                urls.value(movie), movie->currentPixmap());
        setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // causes reload
    }
}

void chat_input_edit::addAnimation(const QUrl& url, const QString& fileName)
{
    QMovie* movie = new QMovie(this);
    movie->setFileName(fileName);
    urls.insert(movie, url);
    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate()));
    movie->start();
}

void chat_input_edit::del_all_movie()
{
    if(urls.size() > 0) {
        QList<QMovie*> movie_list;
        QList<QMovie*>::iterator ite;
        QMovie *m;
        movie_list = urls.keys();
        ite = movie_list.begin();
        while(ite != movie_list.end()) {
            m = *ite;
            m->stop();
            delete m;
            ite++;
        }
        urls.clear();
    }
}

