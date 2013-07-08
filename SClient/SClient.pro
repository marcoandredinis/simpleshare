#-------------------------------------------------
#
# Project created by QtCreator 2013-06-15T23:21:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SClient
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

linux{
    LIBS += -lminiupnpc
}



macx {
    CONFIG +=c++11

    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c++11 -stdlib=libc+

    INCLUDEPATH += /usr/include

    LIBS += /usr/lib/libminiupnpc.a
}

SOURCES += main.cpp\
        loginwindow.cpp \
    qclient.cpp \
    controlhandler.cpp \
    dataserver.cpp \
    datahandler.cpp \
    msgsocket.cpp \
    menuwindow.cpp \
    sendfile.cpp \
    askfile.cpp \
    registrywindow.cpp \
    settings.cpp

HEADERS  += loginwindow.h \
    qclient.h \
    controlhandler.h \
    dataserver.h \
    datahandler.h \
    msgsocket.h \
    menuwindow.h \
    sendfile.h \
    askfile.h \
    registrywindow.h \
    settings.h

FORMS    += loginwindow.ui \
    menuwindow.ui \
    registrywindow.ui
