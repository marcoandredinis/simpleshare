#include "datahandler.h"

DataHandler::DataHandler(QTcpSocket *socket, QString downloadFPath, QObject *parent) :
    QObject(parent)
{
    m_socket = socket;
    m_msg = new MsgSocket(m_socket);
    m_writeFile = new QFile;
    m_readFile = new QFile;
    m_sumReceived = 0;

    m_controlMode = false;

    m_downloadPath = downloadFPath;

    connect(m_msg, &MsgSocket::newMessage, this, &DataHandler::handleNewMsg);
    connect(m_msg, &MsgSocket::newRawChunck, this, &DataHandler::handleNewChunck);
    connect(m_socket, &QTcpSocket::disconnected, [this](){

        if( !m_controlMode ){
            qDebug() << "-->DC";
            if( m_writeFile->isOpen() ){
                m_writeFile->flush();
                m_writeFile->close();
            }
            //recebeu o ficheiro todo
            if( m_writeFile->size() == m_info["size"].toInt() ){
                qDebug() << "Recebi tudo";
                m_writeFile->rename(m_info["code"]+"-"+m_info["fName"]);
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
                emit finishedRec(m_info["code"], m_downloadPath);
            }
        }

    });
}


void DataHandler::handleNewMsg(QMap<QString, QString> info){
    m_info = info;
    qDebug() << "DataHandler::handleNewMsg()" << m_info;
    if( m_info["tipo"] == "upingFile"){
        m_writeFile = new QFile(m_info["code"]+".qdownload");
        //if( m_writeFile->exists() )
        //    m_writeFile->remove();

        m_writeFile->open(QIODevice::Append);
    }else if(m_info["tipo"] == "downingFile"){
        sendFile();
    }else if(m_info["tipo"] == "pingDataServer"){
        QMap<QString, QString> info;
        info["tipo"] = "pongDataServer";

        m_msg->sendMsg(info);
        m_controlMode = true;
    }

}


void DataHandler::handleNewChunck(QByteArray data){
    qDebug() << "DataHandler::handleNewChunck()" << data.size();
    m_writeFile->write(data);
    m_sumReceived += data.size();
    emit newChunck(m_info["code"], m_info["fName"], m_info["fSize"].toInt(), m_sumReceived + m_info["seek"].toInt());
}

void DataHandler::setDownloadPath(QString downloadPath){
    m_downloadPath = downloadPath;
}

void DataHandler::sendFile(){
    int bs = 100000;
    m_readFile = new QFile(m_info["fPath"]+"/"+m_info["fName"]);
    int startpoint = m_info["seek"].toInt();

    m_readFile->open(QIODevice::ReadOnly);
    int fileSize = m_readFile->size();

    //Send header info
    QMap<QString, QString> header;
    header["tipo"] = "upingFile";
    header["code"] = m_info["code"];
    header["fName"] = m_info["fName"];
    header["fSize"] = QString::number(fileSize);
    header["dataMode"] = "true";
    m_msg->sendMsg(header);

    QByteArray tmp;

    for(int i=startpoint; i<fileSize && m_socket->state() != QAbstractSocket::UnconnectedState; i+=bs){
        tmp = m_readFile->read(bs);
        m_msg->sendRawData(tmp);
        while( m_socket->bytesToWrite() && m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForBytesWritten();

        emit chunkSent(m_info["code"], m_info["fName"], tmp.size());
    }

    m_socket->flush();
    m_socket->close();
    emit allSent(m_info);


}
