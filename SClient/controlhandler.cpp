#include "controlhandler.h"

ControlHandler::ControlHandler(QList<QString> ips, QObject *parent) :
    QObject(parent)
{
    m_ips = ips;
    m_socket = new QTcpSocket;
    connect(m_socket, &QTcpSocket::connected, [this](){
        emit connectedToHost();
    });

    connect(m_socket, &QTcpSocket::disconnected, [this](){
        emit disconnectedFromHost(m_socket->errorString());
    });


    m_msg = new MsgSocket(m_socket);

    connect(m_msg, &MsgSocket::newMessage, this, &ControlHandler::readMsg);


    //m_socket->connectToHost(QHostAddress::LocalHost, 8888);

    //m_socket->connectToHost(QHostAddress("213.22.74.154"), 8888);
}

void ControlHandler::connectToHost(int pos){
    int realPos = pos % m_ips.size();
    m_serverIP = m_ips.at(realPos);
    m_socket->connectToHost(QHostAddress(m_serverIP), 8888);
    qDebug() << "Connecting";
}


void ControlHandler::readMsg(QMap<QString, QString> info){
    qDebug() << "ControlHandler::readyRead();" << info;

    if( info["tipo"] == "loginStatus" ){
        if( info["value"] == "accepted"){
            m_nick = info["nick"];
            emit okLogin(info["nick"]);
        }else if( info["tipo"] == "loginStatus"){
            emit badLogin(info["reason"]);
        }

    }else if( info["tipo"] == "registryStatus" ){
        if( info["value"] == "accepted"){
            m_nick = info["nick"];
            emit okRegistry(info["nick"]);
        }else if( info["tipo"] == "registryStatus"){
            emit badRegistry(info["reason"]);
        }

    }else if( info["tipo"] == "code"){
        emit newCodeFromFile(info);

    }else if( info["tipo"] == "downTransfer"){

        if( info["status"] == "available"){

            //Processing
            if( info["mode"] == "1"){
                //qDebug() << "[Log] Processing";

            } else if( info["mode"] == "2"){
                qDebug() << "[Log] Requesting file to host";
                emit askFile(info);
            } else if( info["mode"] == "3"){
                qDebug() << "[Log] Requesting file to server";
                info["IP"] = m_serverIP;
                emit askFile(info);
            }

        }else if(info["status"] == "not found"){
            //emit appendToLog("Code " + info["code"] +" not found.");
            emit codeNotFound(info["code"]);
        }else if(info["status"] == "not available"){
            //emit appendToLog("Owner of code " + info["code"]  + " not available.");
            emit codeNotAvailable(info["code"]);
        }
    }else if( info["tipo"] == "upFile"){
        emit uploadFile(info);
    }
    else if( info["tipo"] == "confirm"){
        QMap<QString, QString> resp;
        resp["tipo"] = "sync";
        resp["code"] = info["code"];
        resp["seek"] = info["seek"];
        //resp["transferMode"] = info["transferMode"];
        sendMsg(resp);
    }

}


void ControlHandler::sendMsg(QMap<QString, QString> info){
    qDebug() << "ControlHandler::sendMsg();" << info;
    m_msg->sendMsg(info);
}

void ControlHandler::removeCodes(QList<QString> invalidCodes){
    foreach (QString code, invalidCodes) {
        QMap<QString, QString> info;
        info["tipo"] = "removeCode";
        info["owner"] = m_nick;
        info["code"] = code;
        sendMsg(info);
    }
}
