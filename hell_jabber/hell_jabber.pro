TEMPLATE = app
QT += core gui network xml webkit

# Config qxmpp
CONFIG += debug_and_release
#CONFIG += console

INCLUDEPATH += ../lib/qxmpp/src

CONFIG(debug, debug|release) {
    QXMPP_LIB = qxmpp_d
    QXMPP_DIR = ../lib/qxmpp/lib
    TARGET = hell_jabber_d
} else {
    QXMPP_LIB = qxmpp
    QXMPP_DIR = ../lib/qxmpp/lib
    TARGET = hell_jabber
}

#LIBS += -L$$QXMPP_DIR -l$$QXMPP_LIB
#LIBS += $${QXMPP_DIR}/lib$${QXMPP_LIB}.a
#PRE_TARGETDEPS += $${QXMPP_DIR}/lib$${QXMPP_LIB}.a
LIBS += E:/3.Datum/Qt_GPL_Pro/Hell_jabber/Hell/lib/qxmpp/lib/libqxmpp.a

#Code
SOURCES += main.cpp\
        mainwindow.cpp \
    login.cpp \
    main_frame.cpp \
    friend_list_model.cpp \
    friend_list_delegate.cpp \
    zDeadCode.cpp \
    flm_listview.cpp \
    chat_dlg.cpp \
    chat_input_edit.cpp \
    select_emoticons.cpp \
    emoticon_preview_label.cpp \
    emoticon_preview_widget.cpp

HEADERS  += mainwindow.h \
    login.h \
    main_frame.h \
    friend_list_model.h \
    friend_list_delegate.h \
    flm_listview.h \
    chat_dlg.h \
    chat_input_edit.h \
    select_emoticons.h \
    emoticon_preview_label.h \
    emoticon_preview_widget.h

FORMS    += mainwindow.ui \
    login.ui \
    chat_dlg.ui

RESOURCES += \
    Emoticons.qrc
