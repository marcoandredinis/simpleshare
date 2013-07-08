#ifndef SPREADAPI_H
#define SPREADAPI_H

#include <QObject>
#include <QDebug>
#include <sp.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spreadrecmsg.h"
#include <QThread>

class SpreadAPI : public QObject
{
    Q_OBJECT
public:
    explicit SpreadAPI(QString nick, QObject *parent = 0);
    
signals:
    void newMsgReceived(QMap<QString, QString> info);

private slots:

public slots:
    void join(QByteArray group);
    void sendMsg(QByteArray msg);

private:
    mailbox mbox;
    bool connected;
    
};

#endif // SPREADAPI_H
