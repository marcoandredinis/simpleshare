#ifndef DATASERVER_H
#define DATASERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QDir>
#include "datahandler.h"
#include "sendfile.h"
#include "askfile.h"

class DataServer : public QObject
{
    Q_OBJECT
public:
    explicit DataServer(int port, QObject *parent = 0);
    
signals:
    void newFileReceived(QString code, QString downloadFolder);
    void newChunckReceived(QString code, QString fName, int fSize, int cSize);
    void errorTransfer(QString code, int fSize, int cSize);

    void errorConnectingAsk(QString code, QString seekPos);
    void errorConnectingSend(QString code, QString seekPos, QString nickDest);

    void serverStarted();
    
public slots:
    void uploadFile(QMap<QString, QString> info);
    void askFile(QMap<QString, QString> info);
    void newConnection();
    void start();
    void setDownloadPath(QString newPath);

private slots:


private:
    QTcpServer *m_listen;
    int m_port;
    QString m_downloadPath;
};

#endif // DATASERVER_H
