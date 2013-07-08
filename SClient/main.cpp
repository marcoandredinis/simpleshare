#include "qclient.h"
#include <QApplication>

void mapPort(int port, int *st){

    //int port = m_dataPort;
    int status;
    char lanaddr[64];
    int r = UPNPCOMMAND_UNKNOWN_ERROR;

    UPNPUrls urls;
    IGDdatas data;
    QString qPort = QString("%1").arg(port);
    UPNPDev * devlist = upnpDiscover(2000, 0, 0, 0, 0, &status);

    int i = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
    if (i == 1){
        r = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype,
                                qPort.toUtf8().data(), qPort.toUtf8().data(), lanaddr, 0, "TCP", 0, 0);
    }

    if( r != UPNPCOMMAND_SUCCESS ){
        //m_portMapped = false;
        *st = 0;
    }else{
        *st = 1;
    }
    qDebug() << "[Log] Port mapping " << ((r == UPNPCOMMAND_SUCCESS) ? "true" : "false");
}


void unMapPort(int port){

    //int port = m_dataPort;
    //if( !m_portMapped ) return;

    int status;
    char lanaddr[64];
    int r = UPNPCOMMAND_UNKNOWN_ERROR;


    UPNPUrls urls;
    IGDdatas data;
    QString qPort = QString("%1").arg(port);
    UPNPDev * devlist = upnpDiscover(2000, 0, 0, 0, 0, &status);

    int i = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));
    if (i == 1){

        r = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype,
                                   qPort.toUtf8().data(), "TCP", 0);
    }

    qDebug() << "[Log] Port un-mapping " << ((r == UPNPCOMMAND_SUCCESS) ? "true" : "false");
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QClient *qc = new QClient;
    
    return a.exec();
}
