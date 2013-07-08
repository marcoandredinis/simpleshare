#ifndef QCLIENT_H
#define QCLIENT_H

#include <QObject>
#include <QTime>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>
#include "loginwindow.h"
#include "registrywindow.h"
#include "dataserver.h"
#include "controlhandler.h"
#include "menuwindow.h"
#include "settings.h"

class QClient : public QObject
{
    Q_OBJECT
public:
    explicit QClient(QObject *parent = 0);
    
signals:
    
public slots:
    QList<QString> getServerIP();
    void recoverDownloads(MenuWindow *mw, QList<QString> incompleted);
    QList<QString> detectIncompleteFiles();
    //void mapPort();
    //void unMapPort();


private:
    int m_dataPort;
    QList<QString> m_ips;
    QList<QString> m_activeTransfers;

    int m_status;
    int m_serverIPPos;

    bool m_chConnected;
    QElapsedTimer timer;

    
};

#endif // QCLIENT_H
