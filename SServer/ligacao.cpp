#include "ligacao.h"
#include <QObject>
#include <QSqlQuery>
#include <QList>
#include <QDebug>
#include "file.h"

Ligacao::Ligacao(QString fName, QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fName);
    bool db_ok = db.open();
    qDebug() << "[Log] LigacaoBD:" << db_ok;

}

Ligacao::~Ligacao()
{

}

void Ligacao::remCode(QString code){
    QSqlQuery q(db);
    bool op;

    op = q.exec(QString("DELETE FROM Files WHERE code='%1';").arg(code));

    if( op ) emit removedFile(code);
}

void Ligacao::emitAllAddedFiles(){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT code, user, nome, extensao, size, path FROM Files;"));
    if(op){
        while(q.next()){
            QString i   = q.value(0).toString();
            int u       = q.value(1).toInt();
            QString n   = q.value(2).toString();
            QString e   = q.value(3).toString();
            int s       = q.value(4).toInt();
            QString p   = q.value(5).toString();

            QSqlQuery qq(db);
            op = qq.exec(QString("SELECT nome FROM Users WHERE id='%1';").arg(u));
            if(op){
                qq.next();
                QString un = qq.value(0).toString();
                emit addedFile(new File(i,un,n,e,p,s));
            }
        }
    }
}

void Ligacao::addUser(QString nome, QString email, QString password, QString path){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT MAX(id) FROM Users;"));
    if( !op ) return;
    int maxid;
    q.next();
    if(!q.value(0).isNull()){
        maxid = q.value(0).toInt();
    }
    else{
        maxid = 1;
    }

    QString sql = QString("INSERT INTO Users VALUES (%1,'%2','%3','%4','%5');").arg(maxid+1).arg(nome).arg(email).arg(password).arg(path);
    QSqlQuery qq(db);
    op = qq.exec(sql);
    //if(op) qDebug() << "merda";
    if( op ) emit addedUser(nome, email, password, path);
}

void Ligacao::addFile(File* f){
    QSqlQuery q(db);
    bool op;

    q.exec(QString("SELECT id FROM Users WHERE nome='%1';").arg(f->getDono()));
    q.next();
    QString id_u= q.value(0).toString();
    QString sql = QString("INSERT INTO Files VALUES ('%1','%2','%3','%4','%5','%6');").arg(f->getCode()).arg(id_u).arg(f->getName()).arg(f->getExt()).arg(f->getSize()).arg(f->getPath());

    QSqlQuery p(db);
    op = p.exec(sql);
    if( op ) emit addedFile(f);
}

QString Ligacao::getNameFile(int id){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT nome FROM Files WHERE id=%1;").arg(id));
    if(op){
        q.next();
        QString res = q.value(0).toString();
        return res;
    }
    return NULL;
}

QString Ligacao::getOwnerFile(QString code){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT u.nome FROM Users u, Files f WHERE f.code='%1' AND f.user=u.id;").arg(code));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getExtensaoFile(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT extensao FROM Files WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

int Ligacao::getSizeFile(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT size FROM Files WHERE nome='%1';").arg(nome));
    int res = 0;
    if(op){
        q.next();
        res = q.value(0).toInt();
    }
    return res;
}

QString Ligacao::getPathFile(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT path FROM Files WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getNameUser(int id){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT nome FROM Users WHERE id='%1';").arg(id));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getPasseUser(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT password FROM Users WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getEmailUser(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT email FROM Users WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getPathUser(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT path FROM Users WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QString Ligacao::getEnderecoUser(QString nome){
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT endereco FROM Users WHERE nome='%1';").arg(nome));
    QString res = NULL;
    if(op){
        q.next();
        res = q.value(0).toString();
    }
    return res;
}

QList<QString>* Ligacao::getFilesUser(QString nome){
    QSqlQuery q(db);
    q.exec(QString("SELECT f.nome FROM Files f, Users u WHERE u.nome='%1' AND u.id=f.utilizador;").arg(nome));
    QList<QString> *res = new QList<QString>();
    while(q.next()){
        res->append(q.value(0).toString());
    }
    return res;
}


bool Ligacao::existFile(QString code){
    bool res = false;
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT nome FROM Files WHERE code='%1';").arg(code));
    if(op){
        if(q.next())
            res = true;
    }
    return res;
}



File * Ligacao::getFile(QString code){
    File *f = NULL;
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT code, user, nome, extensao, size, path FROM Files WHERE code='%1';").arg(code));
    if(op){
        q.next();
        QString i = q.value(0).toString();
        int u = q.value(1).toInt();
        QString n = q.value(2).toString();
        QString e = q.value(3).toString();
        int s = q.value(4).toInt();
        QString p = q.value(5).toString();
        op = q.exec(QString("SELECT nome FROM Users WHERE id='%1';").arg(u));
        if(op){
            q.next();
            QString un = q.value(0).toString();
            f = new File(i,un,n,p,e,s);
        }
    }
    return f;
}

bool Ligacao::existUser(QString name){
    bool res = false;
    QSqlQuery q(db);

    bool op = q.exec(QString("SELECT count(*) FROM Users WHERE nome='%1';").arg(name));
    if(op){
        q.next();
        if(q.value(0).toInt()>0)
            res = true;
    }
    return res;
}

bool Ligacao::existCode(QString code){
    bool res = false;
    QSqlQuery q(db);
    bool op = q.exec(QString("SELECT count(*) FROM Files WHERE code='%1';").arg(code));
    if(op){
        q.next();
        if(q.value(0).toInt()>0)
            res = true;
    }
    return res;
}


void Ligacao::disconnectFromDB(){
    db.close();
}
