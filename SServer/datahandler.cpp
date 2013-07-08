#include "datahandler.h"

DataHandler::DataHandler(QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    m_socket = socket;
    m_type = DataHandler::Undifined;
    m_msg = new MsgSocket(m_socket);
    m_transfering = false;

    m_lbff = new QList<QByteArray>;

    connect(m_msg, &MsgSocket::newMessage, this, &DataHandler::receiveRequest);
    connect(m_msg, &MsgSocket::newRawChunck, this, &DataHandler::receiveChunck);
}


void DataHandler::receiveRequest(QMap<QString, QString> info){
    qDebug() << "DataHandler::receiveRequest();" << info;
    m_info = info;
    if( info["tipo"] == "downingFile"){
        emit newDown(info["code"], this);
    }else if( info["tipo"] == "upingFile"){
        emit newUp(info["code"], this);
        //m_msg->stopTransfer();
    }
}

void DataHandler::receiveChunck(QByteArray b){
    //qDebug() << "DataHandler::receiveChunck()" << b.size();
    //m_msgReceptor->sendRawData(b);
    if( m_transfering ){
        m_msgReceptor->sendRawData(b);
    }
    else
        m_lbff->append(b);
}

int DataHandler::getType(){ return m_type; }
QString DataHandler::getCode(){ return m_info["code"]; }
QMap<QString, QString> DataHandler::getInfo(){ return m_info; }
MsgSocket *DataHandler::getMsgSocket(){ return m_msg; }
QTcpSocket *DataHandler::getSocket(){ return m_socket; }

void DataHandler::startTransfer(DataHandler *dhR){
    m_msgReceptor = dhR->getMsgSocket();
    m_msgReceptor->sendMsg(m_info);

    while(m_lbff->size() > 0){
        m_msgReceptor->sendRawData(m_lbff->first());
        m_lbff->removeFirst();
    }

    m_transfering = true;

    QTcpSocket *sReceptor = dhR->getSocket();

    QTimer *qt = new QTimer;
    qt->start(3000);

    connect(qt, &QTimer::timeout, [this, sReceptor, qt](){
        if( m_lbff->size() > 0){
            while(m_lbff->size() > 0){
                m_msgReceptor->sendRawData(m_lbff->first());
                m_lbff->removeFirst();
            }
        }

        if(  m_socket->state() == QTcpSocket::UnconnectedState && m_lbff->size()==0 ){
            qt->stop();
            sReceptor->close();
        }

    });

    connect(sReceptor, &QTcpSocket::disconnected, m_socket, &QTcpSocket::disconnectFromHost);
}
