#include "controlhandler.h"

ControlHandler::ControlHandler(QList<ControlHandler*> *lch, Ligacao *lbd, QTcpSocket *socket, QObject *parent) :
    QObject(parent)
{
    m_lch = lch;
    m_lbd = lbd;
    m_name = "NNclient";
    m_socket = socket;
    m_dataServerAvailable = false;

    connect(m_socket, &QTcpSocket::disconnected, [this](){
        emit offUser(m_name);
    });

    m_msg = new MsgSocket(m_socket);

    connect(m_msg, &MsgSocket::newMessage, this, &ControlHandler::handleRead);

}

QString ControlHandler::getName(){ return m_name; }

QTcpSocket *ControlHandler::getSocket(){ return m_socket; }

void ControlHandler::sendMsg(QMap<QString, QString> msg){ m_msg->sendMsg(msg); }

int ControlHandler::getDataPort(){ return m_dataPort; }

QString ControlHandler::getIPPeer(){ return m_socket->peerAddress().toString(); }

QString ControlHandler::getIPLocal(){
    //return m_socket->localAddress().toString();
    //return m_localAdd;
    return "ipServer";
}


bool ControlHandler::getDataServerAvailability(){ return m_dataServerAvailable; }



void ControlHandler::handleRead(QMap<QString, QString> info){
    qDebug() << "[Log] ControlHandler::handleRead();" << info;
    QMap<QString, QString> msg;

    if( info["tipo"] == "login"){
        m_name = info["nick"];
        m_password = info["passe"];
        m_dataPort = info["dataPort"].toInt();

        //Response
        msg["tipo"] = "loginStatus";
        msg["nick"] = m_name;


        int loginOK = 0;

        if( m_lbd->existUser(m_name) ){
            if( m_lbd->getPasseUser(m_name) == m_password )
                loginOK = 1;
            else
                loginOK = -1;
        }

        if( loginOK == 1){
            msg["value"] = "accepted";

            //enviar ping para o servidor de dados do cliente
            QTcpSocket *testDataServer = new QTcpSocket;
            MsgSocket *msgTestDS = new MsgSocket(testDataServer);



            connect(testDataServer, &QTcpSocket::connected, [this, msgTestDS, testDataServer](){
                QMap<QString, QString> info;
                info["tipo"] = "pingDataServer";

                msgTestDS->sendMsg(info);
            });


            connect(msgTestDS, &MsgSocket::newMessage, [this, testDataServer](QMap<QString, QString> info){
                if( info["tipo"] == "pongDataServer"){
                    m_dataServerAvailable = true;
                    //qDebug() << "Pong!";
                }

                testDataServer->close();
            });

            //testDataServer->connectToHost(QHostAddress(m_socket->peerAddress().toString()), m_dataPort);

            QTimer *qt = new QTimer;
            qt->start(10000);

            connect(qt, &QTimer::timeout, [this, qt, testDataServer](){
                if( !m_dataServerAvailable && testDataServer->state() != QTcpSocket::ConnectingState){
                    testDataServer->connectToHost(m_socket->peerAddress().toString(), m_dataPort);
                }
                qt->stop();
            });

            testDataServer->connectToHost(m_socket->peerAddress().toString(), m_dataPort);
            qDebug() << "Trying to connect to " << m_socket->peerAddress().toString() <<  m_dataPort;


            emit onUser(m_name);
        }else if( loginOK == 0){
            msg["value"] = "denied";
            msg["reason"] = "Inexistent user";
        }else if( loginOK == -1){
            msg["value"] = "denied";
            msg["reason"] = "Bad Password";
        }

        m_msg->sendMsg(msg);


    }else if( info["tipo"] == "register"){
        //Response
        m_name = info["nick"];
        m_password = info["passe"];

        msg["tipo"] = "registryStatus";
        msg["nick"] = m_name;

        if( !m_lbd->existUser(m_name) ){
            m_lbd->addUser(info["nick"],info["email"],info["password"],info["path"]);
            msg["value"] = "accepted";
            emit onUser(m_name);
        }
        else{
            msg["value"] = "denied";
            msg["reason"] = "Chosen name is not available";
        }

        m_msg->sendMsg(msg);
    }
    else if( info["tipo"] == "upCode"){

        QString code = createCode();


        QString fName = info["fName"];
        QString nick = info["nick"];
        QString fPath = info["fPath"];
        int fSize = info["fSize"].toInt();
        QString ext = "";
        if( fName.contains(".")){
            ext += fName.right(fName.length() - 1 - fName.lastIndexOf("."));
        }
        //QString code, QString dono, QString name, QString path, QString ext, int size
        File *f = new File(code, nick, fName, fPath, ext, fSize);
        m_lbd->addFile(f);

        QMap<QString, QString> msg;
        msg["tipo"] = "code";
        msg["code"] = code;
        msg["fName"] = fName;
        msg["fPath"] = fPath;
        m_msg->sendMsg(msg);


    }else if( info["tipo"] == "downCode"){
        QMap<QString, QString> msg;
        msg["tipo"] = "downTransfer";
        msg["code"] = info["code"];
        msg["seek"] = info["seek"];


        if( !m_lbd->existFile(info["code"]) ){
            msg["status"] = "not found";
        }else{
            File *f = m_lbd->getFile(info["code"]);
            bool available = false;

            ControlHandler *chSource = NULL;

            for(int i=0; i<m_lch->size() && !available; i++)
                if( m_lch->at(i)->getName() == f->getDono() ){
                    available = true;
                    chSource = m_lch->at(i);
                }

            if(!available){
                msg["status"] = "not available";
            }else{
                int mode;



                if( m_dataServerAvailable ){
                    mode = 1;
                }else if( chSource->getDataServerAvailability() ){
                    mode = 2;
                }else{
                    mode = 3;
                }
                mode = 3;

                qDebug() << "TransferMode" << mode;


                msg["status"] = "available";
                msg["fName"] = f->getName();
                msg["fPath"] = f->getPath();
                msg["sName"] = chSource->getName();

                msg["mode"] = QString::number(mode);

                if( mode == 1){
                    QMap<QString, QString> msgTW;
                    msgTW["tipo"] = "upFile";
                    msgTW["code"] = info["code"];
                    msgTW["destNick"] = m_name;
                    msgTW["IP"] = getIPPeer();
                    msgTW["DataPort"] = QString::number(getDataPort());
                    msgTW["fName"] = f->getName();
                    msgTW["fPath"] = f->getPath();
                    msgTW["seek"] = info["seek"];
                    chSource->sendMsg(msgTW);

                }else if( mode == 2){
                    msg["IP"] = chSource->getIPPeer();
                    msg["DataPort"] = QString::number(chSource->getDataPort());

                }else if( mode == 3){
                    msg["IP"] = m_socket->localAddress().toString();
                    msg["DataPort"] = "9999";

                    QMap<QString, QString> msgTW;
                    msgTW["tipo"] = "upFile";
                    msgTW["code"] = info["code"];
                    msgTW["mode"] = "3";
                    msgTW["destNick"] = m_name;
                    msgTW["IP"] = m_localAdd;
                    msgTW["DataPort"] = "9999";
                    msgTW["fName"] = f->getName();
                    msgTW["fPath"] = f->getPath();
                    msgTW["seek"] = info["seek"];

                    chSource->sendMsg(msgTW);
                }
            }
        }
        m_msg->sendMsg(msg);

    }else if( info["tipo"] == "remCode"){
        m_lbd->remCode(info["code"]);
    }
}

QString ControlHandler::createCode(){
    QString c = "";
    int n;
    char temp;
    do{
        for(int i = 0; i<5; i++){
            do{
                n = qrand() % ((90) - 48) + 48;
            } while(n>57 && n <=65);

            temp = static_cast<char>(n);
            c += (char)temp;
        }

        if(m_lbd->existCode(c))
            qDebug() << "conflito";
    }while(m_lbd->existCode(c));
    qDebug() << "[Log] code" << c;
    return c;
}
