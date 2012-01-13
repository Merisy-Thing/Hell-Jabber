#include "chat_log_edit.h"
#include <QFile>

chat_log_edit::chat_log_edit(QWidget *parent) :
    QTextEdit(parent)
{
}

void chat_log_edit::animate()
{
    if (QMovie* movie = qobject_cast<QMovie*>(sender())) {
        document()->addResource(QTextDocument::ImageResource,
                                urls.value(movie), movie->currentPixmap());
        setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // causes reload
    }
}

void chat_log_edit::addAnimation(const QUrl& url, const QString& fileName)
{
    QFile file(fileName);

    if(file.exists()) {
        QMovie* movie = new QMovie(this);
        movie->setFileName(fileName);
        urls.insert(movie, url);
        connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate()));
        movie->start();
    }
}
