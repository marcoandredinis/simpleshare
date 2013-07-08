#include "spreadrecmsg.h"

SpreadRecMsg::SpreadRecMsg(mailbox *mbox, QObject *parent) :
    QObject(parent)
{
    m_mbox = mbox;
}


void SpreadRecMsg::waitForNewMsg(){
    while( true ){
        int MAX_MESSLEN = 102400;
        int MAX_MEMBERS = 100;
        int service_type;
        char sender[MAX_GROUP_NAME];
        char mess[MAX_MESSLEN];
        char target_groups[MAX_MEMBERS][MAX_GROUP_NAME];
        int num_groups;
        int16 mess_type;
        int endian_mismatch;
        int ret = SP_receive( *m_mbox, &service_type,
                              sender, 100, &num_groups,
                              target_groups, &mess_type,
                              &endian_mismatch,
                              sizeof(mess), mess );

        if( ret < 0 )
            continue;
        QMap<QString, QString> info;
        info["tipo"] = "unknown";
        if( service_type == 32){
            info["tipo"] = "msg";
            info["msg"] = QString(mess).left(ret);

        }else if( service_type == 5120){
            info["tipo"] = "out";

        }else if( service_type == 4352){
            info["tipo"] = "in";
        }
        //qDebug() << "SpreadRecMsg::waitForNewMsg();" << info;
        emit receivedMsg(QMap<QString, QString>(info));
    }
}
