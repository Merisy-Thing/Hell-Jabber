#ifndef CHAT_INPUT_EDIT_H
#define CHAT_INPUT_EDIT_H

#include <QObject>
#include <QTextEdit>

class chat_input_edit : public QTextEdit
{
    Q_OBJECT
public:
    explicit chat_input_edit(QObject *parent = 0);

    void keyPressEvent ( QKeyEvent * e );

signals:
    void enter_key_pressed();

public slots:

};

#endif // CHAT_INPUT_EDIT_H
