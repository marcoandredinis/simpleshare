#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include <QFile>
#include "msgsocket.h"
#include "sendfile.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QTcpSocket *socket, QString downloadFPath, QObject *parent = 0);
    
signals:
    //void receivedChunk(QString code, QString fName, int size);
    void newChunck(QString code, QString filename, int fSize, int cSize);
    void errorTransfer(QString code, int fSize, int received);
    void finishedRec(QString code, QString downloadFolder);

    void chunkSent(QString code, QString fName, int fSize);
    void allSent(QMap<QString, QString> info);


    
public slots:
    void handleNewMsg(QMap<QString, QString> info);
    void handleNewChunck(QByteArray data);
    void setDownloadPath(QString downloadPath);

    void sendFile();

private:
    QTcpSocket *m_socket;
    MsgSocket *m_msg;
    QMap<QString, QString> m_info;

    QFile *m_writeFile;
    QFile *m_readFile;

    int m_sumReceived;

    bool m_controlMode;

    QString m_downloadPath;
    
};

#endif // DATAHANDLER_H
