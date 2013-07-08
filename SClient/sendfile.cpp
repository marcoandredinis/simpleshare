#include "sendfile.h"

SendFile::SendFile(QObject *parent) :
    QObject(parent)
{
}

void SendFile::connectAndTransfer(QMap<QString, QString> info){
    m_info = info;
    m_socket = new QTcpSocket;
    m_msg = new MsgSocket(m_socket);
    connect(m_socket, &QTcpSocket::connected, this, &SendFile::startTransfer);
    qDebug() << "SendFile::connectAndTransfer Connecting to " << info["IP"] <<  info["DataPort"].toInt();
    qDebug() << "SendFile::connectAndTransfer();" << m_info;
    m_socket->connectToHost(info["IP"], info["DataPort"].toInt());


    connect(m_socket, &QTcpSocket::stateChanged, [this](){
        if(m_socket->state() == QAbstractSocket::UnconnectedState && m_socket->bytesToWrite() > 0){
            emit errorConnecting(m_info["code"], m_info["seek"], m_info["destNick"]);
            m_socket->close();
        }
    });

}

void SendFile::startTransfer(){
    int bs = 100000;
    //int bs = 10;
    int startpoint = m_info["seek"].toInt();
    QFile file(m_info["fPath"]+"/"+m_info["fName"]);

    file.open(QIODevice::ReadOnly);
    int fileSize = file.size();

    //Send header info
    QMap<QString, QString> header;
    header["tipo"] = "upingFile";
    header["code"] = m_info["code"];
    header["fName"] = m_info["fName"];
    header["fSize"] = QString::number(fileSize);
    header["dataMode"] = "true";
    header["seek"] = m_info["seek"];
    m_msg->sendMsg(header);

    //Read and send data (100KB)
    QByteArray tmp;

    file.seek(startpoint);

    for(int i=startpoint; i<fileSize && m_socket->state() != QAbstractSocket::UnconnectedState; i+=bs){
        tmp = file.read(bs);
        m_msg->sendRawData( tmp);
        while( m_socket->bytesToWrite() && m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForBytesWritten();

        emit chunkSent(m_info["code"], m_info["fName"], tmp.size());
    }

    file.close();
    m_socket->flush();
    m_socket->close();
    emit finished(m_info);
}
