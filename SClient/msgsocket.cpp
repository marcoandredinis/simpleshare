#include "msgsocket.h"

MsgSocket::MsgSocket(QTcpSocket *sock, QObject *parent) :
    QObject(parent)
{
    m_socket = sock;
    m_msgSize = 0;
    m_buffer = new QByteArray;
    m_dataMode = false;

    connect(m_socket, &QTcpSocket::readyRead, this, &MsgSocket::readyRead);
}

void MsgSocket::readyRead(){
    if( m_dataMode ) return readRaw();

    if( m_msgSize > 0){
        int bytesLeft = m_msgSize - m_buffer->size();
        m_buffer->append( m_socket->read(bytesLeft) );

        if( m_buffer->size() == m_msgSize){
            m_msgSize = 0;
            QDataStream out(m_buffer, QIODevice::ReadOnly);
            QMap<QString, QString> msg;
            out >> msg;
            m_buffer->clear();
            m_buffer->resize(0);
            emit newMessage(msg);
            if( msg.contains("dataMode") )
                m_dataMode = true;
        }
    }else{
        m_socket->read((char*) &m_msgSize, sizeof(int));
        m_buffer->clear();
        m_buffer->resize(0);
    }

    if( m_socket->bytesAvailable() ){
        readyRead();
    }
}


void MsgSocket::readRaw(){

    if( m_msgSize > 0){
        int bytesLeft = m_msgSize - m_buffer->size();
        m_buffer->append( m_socket->read(bytesLeft) );

        if( m_buffer->size() == m_msgSize){
            m_msgSize = 0;
            QDataStream out(m_buffer, QIODevice::ReadOnly);
            QByteArray msg;
            out >> msg;
            m_buffer->clear();
            m_buffer->resize(0);
            //emit newRawChunck(msg);
            emit newRawChunck(qUncompress(msg));
        }
    }else{
        m_socket->read((char*) &m_msgSize, sizeof(int));
        m_buffer->clear();
        m_buffer->resize(0);
    }

    if( m_socket->bytesAvailable() ){
        readyRead();
    }
}

void MsgSocket::sendMsg(QMap<QString, QString> source){
    QByteArray arr;
    QDataStream out(&arr, QIODevice::WriteOnly);
    out << source;
    int size = arr.size();
    m_socket->write( (const char*) &size, sizeof(int));
    m_socket->write(arr);
}


void MsgSocket::sendRawData(QByteArray data){
    data = qCompress(data);
    QByteArray arr;
    QDataStream out(&arr, QIODevice::WriteOnly);
    out << data;
    int size = arr.size();
    m_socket->write( (const char*) &size, sizeof(int));
    m_socket->write(arr);
}
