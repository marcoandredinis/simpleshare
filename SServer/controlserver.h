#ifndef CONTROLSERVER_H
#define CONTROLSERVER_H

#include "ligacao.h"
#include "controlhandler.h"
#include <QObject>
#include <QTcpServer>


class ControlServer : public QObject
{
    Q_OBJECT
public:
    explicit ControlServer(Ligacao *lbd, QObject *parent = 0);
    
signals:
    void onlineUser(QString);
    void offlineUser(QString);
    void newLog(QString);
    
public slots:
    void handleNewConn();

private:
    Ligacao *m_lbd;
    QTcpServer *m_server;
    QList<ControlHandler*> *m_lch;
    
};

#endif // CONTROLSERVER_H
