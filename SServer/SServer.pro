#-------------------------------------------------
#
# Project created by QtCreator 2013-06-15T16:56:08
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SServer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
LIBS += -ldl /usr/local/lib/libspread-core.a

macx {
    CONFIG +=c++11

    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c++11 -stdlib=libc+

    INCLUDEPATH += /Users/Rita/Downloads/spread-src-4.3.0/include
}

SOURCES += main.cpp\
        serverwindow.cpp \
    qserver.cpp \
    dataserver.cpp \
    controlserver.cpp \
    controlhandler.cpp \
    datahandler.cpp \
    ligacao.cpp \
    file.cpp \
    msgsocket.cpp \
    spreadrecmsg.cpp \
    spreadapi.cpp

HEADERS  += serverwindow.h \
    qserver.h \
    dataserver.h \
    controlserver.h \
    controlhandler.h \
    datahandler.h \
    ligacao.h \
    file.h \
    msgsocket.h \
    spreadapi.h \
    spreadrecmsg.h

FORMS    += serverwindow.ui
