#ifndef ASKFILE_H
#define ASKFILE_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QMap>
#include "msgsocket.h"

class AskFile : public QObject
{
    Q_OBJECT
public:
    explicit AskFile(QObject *parent = 0);

signals:
    void finished(QString code, QString filename);
    void newChunck(QString code, QString filename, int fSize, int cSize);
    void errorTransfer(QString code, int fSize, int cSize);
    void errorConnecting(QString code, QString seekPos);

public slots:
    void connectAndAsk(QMap<QString, QString> info, QString downloadPath);

private slots:
    void handleNewMsg(QMap<QString, QString> info);
    void handleNewChunck(QByteArray data);
    void handleDC();

private:
    QFile *m_writeFile;
    //bool m_headerMode;
    int m_sumReceived;
    QTcpSocket *m_socket;
    MsgSocket *m_msg;
    QMap<QString, QString> m_info;
    QString m_downloadPath;
};

#endif // ASKFILE_H
