#include "controlserver.h"

ControlServer::ControlServer(Ligacao *lbd, QObject *parent) :
    QObject(parent)
{
    m_lbd = lbd;
    m_server = new QTcpServer;
    m_lch = new QList<ControlHandler*>;

    bool listening = m_server->listen(QHostAddress::AnyIPv4, 8888);

    qDebug() << "[Log] ControlServer:" << listening;

    connect(m_server, &QTcpServer::newConnection, this, &ControlServer::handleNewConn);
}

void ControlServer::handleNewConn(){
    QTcpSocket *tmpS = m_server->nextPendingConnection();
    ControlHandler *tmpCH = new ControlHandler(m_lch, m_lbd, tmpS);

    m_lch->append(tmpCH);
    connect(tmpCH, &ControlHandler::onUser, [this](QString s){
        emit onlineUser(s);
    });

    connect(tmpCH, &ControlHandler::offUser, [this](QString s){
        bool found = false;
        for(int i = 0; i<m_lch->size() && !found; i++){
            ControlHandler *ch = m_lch->at(i);
            if( ch->getName() == s){
                m_lch->removeAt(i);
                found = true;
                emit offlineUser(s);
            }
        }
    });

    connect(tmpCH, &ControlHandler::addLog, [this](QString s){
        emit newLog(s);
    });
}
