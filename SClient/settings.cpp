#include "settings.h"

Settings::Settings(QObject *parent) :
    QObject(parent)
{
    m_fName = "user.cnf";
    m_file = new QFile(m_fName);

}


void Settings::init(QString user, QMap<QString, QPair<QString, QByteArray> > mapFiles){

    m_user = user;
    m_mapFiles = mapFiles;

    save();
}



void Settings::load(QString user){
    m_user = user;
    if( !m_file->exists() ){
        m_mapFiles.clear();
        init(m_user, m_mapFiles);
    }



    QByteArray buffer;
    m_invalidCodes.clear();
    m_file->open(QIODevice::ReadOnly);



    buffer = m_file->readAll();
    m_file->close();

    QDataStream in(&buffer, QIODevice::ReadOnly);
    in >> m_user;
    in >> m_mapFiles;

    qDebug() << "Settings:load();" << m_user << m_mapFiles;

}

QList<QString> Settings::getInvalidCodes(){
    for(QString code: m_mapFiles.keys()){
        QPair<QString, QByteArray> value = m_mapFiles.value(code);
        QString fPathName = value.first;
        QByteArray hashArr = value.second;

        QFile *f = new QFile(fPathName);

        if( !f->exists() ){
            m_invalidCodes.append(code);

        }else{
            f->open(QIODevice::ReadOnly);

            int bs = 1000000;
            QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Md5);
            for(int i=0; i< f->size(); i+=1000000){
                hash->addData(f->read(bs));
            }


            QByteArray realHash = hash->result();

            if( realHash == hashArr){
                qDebug() << "Ficheiro OK";
            }else{
                m_mapFiles.remove(code);
                m_invalidCodes.append(code);
            }
        }



    }
    return m_invalidCodes;
}

QMap<QString, QPair<QString, QByteArray> > Settings::getValidCodes(){ return m_mapFiles; }

void Settings::save(){
    m_file->open(QIODevice::WriteOnly);
    QByteArray arr;
    QDataStream out(&arr, QIODevice::WriteOnly);
    out << m_user;
    out << m_mapFiles;

    qDebug() << "Settings::save();" << m_user << m_mapFiles;

    m_file->write(arr);

    m_file->flush();
    m_file->close();
}

void Settings::removeCode(QString code){
    m_mapFiles.remove(code);
}


void Settings::addCode(QString fPathName, QString code){
    //code -> pathName:hash
    //codeX -> /home/quarter/texto.txt:83914UHYAUSD893749UHAD

    QFile *f = new QFile(fPathName);

    if( !f->exists() )
        return;

    f->open(QIODevice::ReadOnly);

    int bs = 1000000;
    QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Md5);
    for(int i=0; i< f->size(); i+=1000000){
        hash->addData(f->read(bs));
    }


    QByteArray realHash = hash->result();
    QPair<QString, QByteArray> parNameHash;
    parNameHash.first = fPathName;
    parNameHash.second = realHash;

    m_mapFiles.insert(code, parNameHash);
    qDebug() << "Inserting " << code << "-> " << parNameHash;
}
