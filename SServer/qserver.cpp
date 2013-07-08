#include "qserver.h"

QServer::QServer(int mode, QObject *parent) :
    QObject(parent)
{
    m_mode = mode;
    if( mode == Primario )
        initPrim("../SServer/PISD");
    else
        initSec();
}

void QServer::initPrim(QString dbPath){
    //"randomize" seed
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());



    //Create objects
    ServerWindow *sw = new ServerWindow;
    Ligacao *bd = new Ligacao(dbPath);
    ControlServer *cs = new ControlServer(bd);
    DataServer *ds = new DataServer;


    //Set connections
    connect(bd, &Ligacao::addedFile,         sw, &ServerWindow::insereFicheiro);
    connect(cs, &ControlServer::newLog,      sw, &ServerWindow::addToLog);
    connect(cs, &ControlServer::onlineUser,  sw, &ServerWindow::onlineUser);
    connect(cs, &ControlServer::offlineUser, sw, &ServerWindow::offlineUser);

    //Do initial setup
    bd->emitAllAddedFiles();

    SpreadAPI *sPrim = new SpreadAPI("primario");

    connect(bd, &Ligacao::addedFile,[this, sPrim](File *f){
        QString msg = "addCode:"+f->getCode()+":"+f->getDono()+":"+f->getName()+":"+f->getPath()+":"+f->getExt()+":"+
                QString::number(f->getSize());

        //qDebug() << "Send to spread: " << msg;
        sPrim->sendMsg(msg.toUtf8());
    });

    connect(bd, &Ligacao::removedFile,[this, sPrim, sw](QString code){
        QString msg = "remCode:"+code;
        sw->removedCode(code);
        sPrim->sendMsg(msg.toUtf8());
        //qDebug() << "Send to spread: " << msg;
    });

    connect(bd, &Ligacao::addedUser,[this, sPrim](QString nome, QString email, QString password, QString path){
        QString msg = "addUser:"+nome+":"+email+":"+password+":"+path;
        sPrim->sendMsg(msg.toUtf8());
        //qDebug() << "Send to spread: " << msg;
    });

    sw->show();
}

void QServer::initSec(){
    SpreadAPI *sec = new SpreadAPI("secundario");
    Ligacao *bd = new Ligacao("../SServer/PISD2");

    connect(sec, &SpreadAPI::newMsgReceived, [this, bd, sec](QMap<QString, QString> info){
        if( m_mode == Secundario){
            if( info["tipo"] == "msg"){
                qDebug() << "Spread Msg" << info;
                QString contents = info["msg"];
                QList<QString> args = contents.split(":");

                if( args.at(0) == "addCode"){
                    File *f = new File(args.at(1), args.at(2), args.at(3), args.at(4), args.at(5), args.at(6).toInt());
                    bd->addFile(f);

                }else if( args.at(0) == "remCode"){
                    bd->remCode(args.at(1));

                }else if( args.at(0) == "addUser"){
                    bd->addUser(args.at(1), args.at(2), args.at(3), args.at(4));
                }else{
                    qDebug() << "Mesangem de estado" << contents;
                }
            }else if( info["tipo"] == "out"){
                //TODO: passar a primario
                qDebug() << "Vou ser o novo primario";
                bd->disconnectFromDB();
                initPrim("../SServer/PISD2");

            }else if( info["tipo"] == "in"){

            }
        }

    });

}
