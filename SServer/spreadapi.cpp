#include "spreadapi.h"

SpreadAPI::SpreadAPI(QString nick, QObject *parent) :
    QObject(parent)
{
    //nick = "sec";
    char private_group[MAX_GROUP_NAME];
    sp_time timeOut;
    timeOut.sec = 3;
    timeOut.usec = 0;
    int ret = SP_connect_timeout("4803@172.19.106.170", nick.toUtf8().data(), 0, 1, &mbox, private_group, timeOut);
    qDebug() << "[Log] Spread " << ( ret==1 ? "true" : "false");

    if( ret == 1)
        connected = true;
    else
        connected = false;


    join("qserver");
}


void SpreadAPI::join(QByteArray group){

    if( !connected ) return;
    int ret = SP_join(mbox, group.data());

    SpreadRecMsg *srm = new SpreadRecMsg(&mbox);
    QThread *t = new QThread;

    srm->moveToThread(t);
    connect(t, &QThread::started, srm, &SpreadRecMsg::waitForNewMsg);

    connect(srm, &SpreadRecMsg::receivedMsg, [this] (QMap<QString, QString> info) {
        emit newMsgReceived(info);
    });

    t->start();
}

void SpreadAPI::sendMsg(QByteArray msg){
    QByteArray group = "qserver";
    char groups[10][32];
    memcpy(groups[0], group.data(), group.length());
    int ret = SP_multigroup_multicast(mbox, SAFE_MESS, 1, (const char (*)[MAX_GROUP_NAME]) groups, 1, msg.size()+1, msg.data());
}
