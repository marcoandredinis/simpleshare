#ifndef SENDFILE_H
#define SENDFILE_H

#include <QObject>
#include <QMap>
#include <QTcpSocket>
#include <QFile>
#include "msgsocket.h"

class SendFile : public QObject
{
    Q_OBJECT
public:
    explicit SendFile(QObject *parent = 0);
    
signals:
    void finished(QMap<QString, QString> info);
    void chunkSent(QString code, QString fName, int size);
    void errorConnecting(QString code, QString seekPos, QString nickDest);
    
public slots:
    void connectAndTransfer(QMap<QString, QString> info );
    void startTransfer();

private:
    QMap<QString, QString> m_info;
    QTcpSocket *m_socket;
    MsgSocket *m_msg;    
};

#endif // SENDFILE_H
