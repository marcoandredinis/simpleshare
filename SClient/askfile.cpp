#include "askfile.h"

AskFile::AskFile(QObject *parent) :
    QObject(parent)
{

}

void AskFile::connectAndAsk(QMap<QString, QString> info, QString downloadPath){
    m_downloadPath = downloadPath;
    info["tipo"] = "downingFile";
    m_info = info;
    qDebug() << "AskFile::connectAndAsk();" << info;
    m_socket = new QTcpSocket;
    m_msg = new MsgSocket(m_socket);
    m_sumReceived = 0;
    connect(m_socket, &QTcpSocket::connected, [this](){
        m_msg->sendMsg(m_info);
    });

    connect(m_socket, &QTcpSocket::stateChanged, [this](){
        if(m_socket->state() == QAbstractSocket::UnconnectedState){
            emit errorConnecting(m_info["code"], m_info["seek"]);
            m_socket->abort();
        }
    });

    connect(m_socket, &QTcpSocket::disconnected, this, &AskFile::handleDC);
    //connect(m_socket, &QTcpSocket::aboutToClose, this, &AskFile::handleDC);
    connect(m_msg, &MsgSocket::newMessage, this, &AskFile::handleNewMsg);
    connect(m_msg, &MsgSocket::newRawChunck, this, &AskFile::handleNewChunck);

    //qDebug() << "AskFile::connectAndAsk Connecting to " << m_ipDest << m_portDest;
    m_socket->connectToHost(m_info["IP"], m_info["DataPort"].toInt());
}

void AskFile::handleNewMsg(QMap<QString, QString> info){

    m_info = info;
    qDebug() << "AskFile::handleNewMsg()" << m_info;
    if( m_info["tipo"] == "upingFile"){
        m_writeFile = new QFile(m_info["code"]+".qdownload");
        //if( m_writeFile->exists() && !m_info.contains("seek"))
        //    m_writeFile->remove();

        m_writeFile->open(QIODevice::Append);
    }else if(m_info["tipo"] == "downingFile"){
        qDebug() << "====ERRO==== AskFile::handleNewMsg(); " << m_info["tipo"];
    }

}


void AskFile::handleNewChunck(QByteArray data){
    qDebug() << "DataHandler::handleNewChunck()" << data.size();
    m_writeFile->write(data);
    m_sumReceived += data.size();
    emit newChunck(m_info["code"], m_info["fName"], m_info["fSize"].toInt(), m_sumReceived + m_info["seek"].toInt());
}

void AskFile::handleDC(){
    qDebug() << "-->DC";
    if( m_writeFile->isOpen() ){
        m_writeFile->flush();
        m_writeFile->close();
    }

    //recebeu o ficheiro todo
    qDebug() << "desligou-se" << m_sumReceived << m_info["seek"].toInt() << " != " << m_info["fSize"].toInt();
    if( (m_sumReceived + m_info["seek"].toInt()) != m_info["fSize"].toInt())
        emit errorTransfer(m_info["code"], m_info["fSize"].toInt(), m_sumReceived );
    else{
        QString absoluteFPath;
        absoluteFPath.append(m_downloadPath).append("/").append(m_info["code"]).append("_").append(m_info["fName"]);

        QFile oldFile(absoluteFPath);
        if (oldFile.exists())
            oldFile.remove();

        m_writeFile->rename(absoluteFPath);
        //m_writeFile->rename(m_info["code"]+"-"+m_info["fName"]);
        emit finished(m_info["code"], m_downloadPath);
    }
}
