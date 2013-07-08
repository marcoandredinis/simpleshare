#include "dataserver.h"

DataServer::DataServer(int port, QObject *parent) :
    QObject(parent)
{
    m_port = port;
    m_listen = new QTcpServer;
    connect(m_listen, &QTcpServer::newConnection, this, &DataServer::newConnection);

    //connect(m_listen, SIGNAL(), this, &DataServer::newConnection);
    m_downloadPath = QDir::homePath();


}


void DataServer::start(){
    bool listening = m_listen->listen(QHostAddress::Any, m_port);
    qDebug() << "[Log] DataServer@"<< m_port << listening;

    if( listening )
        emit serverStarted();
}


void DataServer::setDownloadPath(QString newPath){
    m_downloadPath = newPath;
}

void DataServer::newConnection(){
    qDebug() << "DataServer new connection";
    QTcpSocket *sTransf = m_listen->nextPendingConnection();
    DataHandler *dh = new DataHandler(sTransf, m_downloadPath);

    connect(dh, &DataHandler::newChunck, [this](QString code, QString fName, int fSize, int cSize){
        emit newChunckReceived(code, fName, fSize, cSize);
    });

    connect(dh, &DataHandler::finishedRec, [this](QString code, QString downloadFolder){
        emit newFileReceived(code, downloadFolder);
    });

    connect(dh, &DataHandler::errorTransfer, [this](QString code, int fSize, int cSize){
        emit errorTransfer(code, fSize, cSize);
    });

}


void DataServer::uploadFile(QMap<QString, QString> info){
    SendFile *sf = new SendFile;
    QThread *t1 = new QThread;

    sf->moveToThread(t1);

    connect(t1, &QThread::started, [this, info, sf](){
        sf->connectAndTransfer(info);
    });

    connect(sf, &SendFile::chunkSent, [this](QString code, QString fName, int fSize){
        qDebug() << "[Log] SendFile:" << code << "Chunck of " << fName << ", bytes written: " << fSize;
    });

    connect(sf, &SendFile::finished, [this](QMap<QString, QString> info){
        qDebug() << "[Log] File Sent!" << info["code"] << info["fName"];
    });

    connect(sf, &SendFile::errorConnecting, [this](QString code, QString seekPos, QString nickDest){
        emit errorConnectingSend(code, seekPos, nickDest);
    });

    t1->start();
}

void DataServer::askFile(QMap<QString, QString> info){
    AskFile *af = new AskFile;

    af->connectAndAsk(info, m_downloadPath);

    connect(af, &AskFile::newChunck, [this](QString code, QString fName, int fSize, int cSize){
        qDebug() << "[Log] RecFile:" << code << "Chunck of " << fName << ", bytes written: " << cSize << " of " << fSize;
        emit newChunckReceived(code, fName, fSize, cSize);
    });

    connect(af, &AskFile::finished, [this](QString code, QString downloadFolder){
        emit newFileReceived(code, downloadFolder);
    });

    connect(af, &AskFile::errorTransfer, [this](QString code, int fSize, int cSize){
        emit errorTransfer(code, fSize, cSize);
    });

    connect(af, &AskFile::errorConnecting, [this](QString code, QString seekPos){
        emit errorConnectingAsk(code, seekPos);
    });

}
