#include "dataserver.h"

DataServer::DataServer(QObject *parent) :
    QObject(parent)
{
    m_listen = new QTcpServer;
    m_downers = new QMap<QString, QList<DataHandler*> *>;
    m_uppers = new QMap<QString, QList<DataHandler*> *>;

    connect(m_listen, &QTcpServer::newConnection, this, &DataServer::newConn);
    bool listening = m_listen->listen(QHostAddress::AnyIPv4, 9999);
    qDebug() << "[Log] DataServer:" << listening;
}


void DataServer::newConn(){
    QTcpSocket *tmp = m_listen->nextPendingConnection();
    DataHandler *dh = new DataHandler(tmp);

    connect(dh, &DataHandler::newDown, [this](QString code, DataHandler *dhR){
        if( m_downers->contains(code))
            m_downers->value(code)->append(dhR);
        else{
            QList<DataHandler *> *lst = new QList<DataHandler *>;
            lst->append(dhR);

            m_downers->insert(code, lst);
        }
        findPartner(code);
    });

    connect(dh, &DataHandler::newUp, [this](QString code, DataHandler *dhW){
        if( m_uppers->contains(code))
            m_uppers->value(code)->append(dhW);
        else{
            QList<DataHandler *> *lst = new QList<DataHandler *>;
            lst->append(dhW);

            m_uppers->insert(code, lst);
        }
        findPartner(code);
    });
}


void DataServer::findPartner(QString code){
    qDebug() << "Find partner, code:" << code;

    //pode ser servido um par com este codigo?
    if( m_uppers->contains(code) && m_downers->contains(code) ){
        //vai ser servido um par(sender, transfer)
        DataHandler *dhU = m_uppers->value(code)->first();
        DataHandler *dhD = m_downers->value(code)->first();

        dhU->startTransfer(dhD);


        m_uppers->value(code)->removeFirst();
        m_downers->value(code)->removeFirst();

        //Senders ficou vazio?
        if( m_uppers->value(code)->isEmpty() )
            //limpar entrada na Hash
            m_uppers->remove(code);

        //Transfers ficou vazio?
        if( m_downers->value(code)->isEmpty() )
            //limpar entrada na Hash
            m_downers->remove(code);
    }
}
