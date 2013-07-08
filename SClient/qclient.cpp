#include "qclient.h"

void mapPort(int port, int *status);
void unMapPort(int port);

QClient::QClient(QObject *parent) :
    QObject(parent)
{

    // new seed
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    m_ips = getServerIP();

    m_dataPort = 9000 + (qrand() % 10);

    m_chConnected = false;

    LoginWindow *lw = new LoginWindow;
    RegistryWindow *rw = new RegistryWindow;
    MenuWindow *mw = new MenuWindow;

    ControlHandler *ch = new ControlHandler(m_ips);
    DataServer *ds = new DataServer(m_dataPort);

    Settings *st = new Settings;

    m_serverIPPos = 0;
    ch->connectToHost(m_serverIPPos);

    QTimer *qt = new QTimer;
    qt->start(5000);

    connect(qt, &QTimer::timeout, [this, ch, qt](){
        if( !m_chConnected){
            m_serverIPPos++;
            ch->connectToHost(m_serverIPPos);
        }else
            qt->stop();
    });





    m_status = 0;

    connect(ds, &DataServer::serverStarted, [this](){
        //QtConcurrent::run(this, &QClient::mapPort);
        QtConcurrent::run(mapPort, m_dataPort, &m_status);
    });

    ds->start();
    connect(lw, &LoginWindow::login, [this, ch](QMap<QString, QString> info){
        info["dataPort"] = QString::number(m_dataPort);
        ch->sendMsg(info);
    });

    connect(rw, &RegistryWindow::registry, [this, ch](QMap<QString, QString> info){
        ch->sendMsg(info);
    });


    connect(rw, &RegistryWindow::cancel, [this, rw, lw](){
        rw->hide();
        lw->show();
    });

    connect(lw, &LoginWindow::showRegister, [this, lw, rw](){
        lw->hide();
        rw->show();
    });

    connect(ch, &ControlHandler::connectedToHost, [this, lw](){
        lw->enableForm("Connected, you can login now.");
        m_chConnected = true;
    });

    connect(ch, &ControlHandler::disconnectedFromHost, [this, lw, mw, st, qt](QString error){
        lw->disableForm("ServerError!");
        mw->hide();
        mw->resetInterface();

        //unMapPort(m_dataPort);

        //guardar settings
        st->save();

        lw->show();

        m_chConnected = false;
        qt->start();
    });

    connect(ch, &ControlHandler::okLogin, [this, lw, mw, st, ch, ds](QString nick){
        lw->disableForm("Login ok!");
        lw->hide();
        mw->setNick(nick);
        mw->show();

        st->load(nick);
        QList<QString> invalidCodes = st->getInvalidCodes();

        foreach(QString invalid, invalidCodes){
            QMap<QString, QString> info;
            info["tipo"] = "remCode";
            info["code"] = invalid;
            ch->sendMsg(info);
            qDebug() << "Codigo invalido" << info;
        }

        QMap<QString, QPair<QString, QByteArray> > validCodes = st->getValidCodes();
        foreach(QString code, validCodes.keys()){

            //codeX -> /home/quarter/texto.txt:hashvaue
            //queremos: codeX -> texto.txt
            QPair<QString, QByteArray> parNameHash = validCodes.value(code);

            QString fName = parNameHash.first.split("/").last();
            mw->newCodeFromFile(code, fName);
        }



        QList<QString> incompletedCodes = detectIncompleteFiles();
        qDebug() << "IncompletedCodes" << incompletedCodes;
        recoverDownloads(mw, incompletedCodes);

        //ds->start();
    });

    connect(ch, &ControlHandler::badLogin, [this, lw](QString reason){
        lw->enableForm("Failed! " + reason);
    });

    connect(ch, &ControlHandler::okRegistry, [this, rw, mw, st, ds](QString nick){
        rw->disableForm("Registry ok!");
        rw->hide();
        mw->setNick(nick);
        mw->show();

        QMap<QString, QPair<QString, QByteArray> > myFiles;
        st->init(nick, myFiles);

        //ds->start();
        
    });

    connect(ch, &ControlHandler::badRegistry, [this, rw](QString reason){
        rw->enableForm("Failed! " + reason);
    });

    //connect(ch, &ControlHandler::newCodeFromFile, mw, &MenuWindow::newCodeFromFile);
    connect(ch, &ControlHandler::newCodeFromFile, [this, mw, st](QMap<QString, QString> info){
        QString fPathName;
        fPathName.append(info["fPath"]);
        fPathName.append("/");
        fPathName.append(info["fName"]);

        st->addCode(fPathName, info["code"]);

        mw->newCodeFromFile(info["code"], info["fName"]);
    });


    connect(ch, &ControlHandler::appendToLog, mw, &MenuWindow::appendToLog);

    connect(mw, &MenuWindow::askDownload, [this, ch](QMap<QString, QString> info){
        ch->sendMsg(info);
        //m_activeTransfers.append(info["code"]);
        qDebug() << "asking file, timer.start";
        timer.start();

    });



    connect(mw, &MenuWindow::askUpload, [this, ch](QMap<QString, QString> info){

        //for( int i =0; i< 3000; i++){
        ch->sendMsg(info);
        //}
    });


    connect(ch, &ControlHandler::uploadFile, [this, ds](QMap<QString, QString> info){
        if( info["mode"] == "3"){
            int realPos = m_serverIPPos % m_ips.size();
            info["IP"] = m_ips.at(realPos);
            qDebug() << "===MODE 3===" << info;
        }
        ds->uploadFile(info);
    });


    connect(ch, &ControlHandler::askFile, ds, &DataServer::askFile);

    connect(ds, &DataServer::newChunckReceived, [this, mw](QString code, QString fName, int fSize, int cSize){
        mw->setDownloadPercent(code, fSize, cSize);
        m_activeTransfers.removeOne(code);
    });

    connect(ds, &DataServer::newFileReceived, [this, mw](QString code, QString downloadFolder){
        mw->completedDownload(code, downloadFolder);
        qDebug() << "file received" << timer.elapsed();
    });

    connect(ds, &DataServer::errorTransfer, [this, mw](QString code, int fSize, int cSize){
        mw->errorDownloading(code);
        qDebug() << "DataServer::errorTransfer" << code << fSize << cSize;
    });



    connect(ds, &DataServer::errorConnectingSend, [this, ch](QString code, QString seekPos, QString nickDest){
        QMap<QString, QString> msg;
        msg["tipo"] = "errorSendingFile";
        msg["code"] = code;
        msg["seek"] = seekPos;
        msg["DestNick"] = nickDest;
        ch->sendMsg(msg);
    });


    connect(ch, &ControlHandler::codeNotAvailable, mw, &MenuWindow::askedCodeNotAvailable);
    connect(ch, &ControlHandler::codeNotFound, mw, &MenuWindow::askedCodeNotFound);



    connect(mw, &MenuWindow::remCode, [this, ch, st](QString code){
        st->removeCode(code);
        QMap<QString, QString> info;
        info["tipo"] = "remCode";
        info["code"] = code;
        ch->sendMsg(info);
    });


    connect(mw, &MenuWindow::closingMenu, [this, st](){
        if( m_status == 1)
            QtConcurrent::run(unMapPort, m_dataPort);

        //guardar settings
        st->save();
    });


    connect(lw, &LoginWindow::closingLogin, [this](){
        if( m_status == 1)
            QtConcurrent::run(unMapPort, m_dataPort);
    });


    connect(rw, &RegistryWindow::closingRegistryWindow, [this](){
        if( m_status == 1)
            QtConcurrent::run(unMapPort, m_dataPort);
    });


    connect(mw, &MenuWindow::downloadPathChanged, [this, ds](QString newPath){
        ds->setDownloadPath(newPath);
    });



    lw->disableForm("Conecting to server...");
    lw->show();


}

QList<QString> QClient::getServerIP(){
    QList<QString> ips;
    QByteArray buffer;
    QFile *f = new QFile("server.cnf");
    if( f->exists() ){
        f->open(QIODevice::ReadOnly);
        buffer = f->readAll();
        QList<QByteArray> lines = buffer.split('\n');
        foreach (QByteArray line, lines) {
            if(line.size()>1)
                ips.append(QString(line));
        }

        f->close();
    }else{
        f->open(QIODevice::WriteOnly);
        f->write("127.0.0.1\n");
        f->flush();
        f->close();
        ips.append("127.0.0.1");
    }

    return ips;
}


void QClient::recoverDownloads(MenuWindow *mw, QList<QString> incompleted){
    foreach (QString entry, incompleted) {
        QList<QString> args = entry.split(":");
        QString code = args.at(0);
        int received = args.at(1).toInt();
        mw->recoverDownload(code, received);
    }
}

QList<QString> QClient::detectIncompleteFiles(){
    QDir incompleteFiles;
    incompleteFiles.setFilter(QDir::Files);

    QStringList filters;
    filters << "*.qdownload";
    incompleteFiles.setNameFilters(filters);

    QFileInfoList fList = incompleteFiles.entryInfoList();

    QList<QString> ret;

    foreach (QFileInfo fi, fList) {
        ret.append(fi.fileName().split(".").at(0) + ":"+QString::number(fi.size()));
    }


    return ret;
}

