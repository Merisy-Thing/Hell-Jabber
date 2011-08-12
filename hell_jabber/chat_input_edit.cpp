#include "chat_input_edit.h"
#include <QKeyEvent>

chat_input_edit::chat_input_edit(QObject *parent)
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
