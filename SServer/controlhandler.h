#ifndef CONTROLHANDLER_H
#define CONTROLHANDLER_H

#include <QTcpSocket>
#include <QObject>
#include <QMap>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "file.h"
#include "ligacao.h"
#include "msgsocket.h"

class ControlHandler : public QObject
{
    Q_OBJECT
public:
    explicit ControlHandler(QList<ControlHandler*> *lch, Ligacao *lbd, QTcpSocket *socket, QObject *parent = 0);
    QString getName();
    QTcpSocket *getSocket();
    int getDataPort();
    void sendMsg(QMap<QString, QString> msg);
    QString getIPPeer();
    QString getIPLocal();
    bool getDataServerAvailability();
    

signals:
    void onUser(QString);
    void offUser(QString);
    void addLog(QString);
    
public slots:
    void handleRead(QMap<QString, QString> info);
    QString createCode();

private:
    QTcpSocket *m_socket;
    MsgSocket *m_msg;
    QString m_name;
    QString m_password;
    bool m_dataServerAvailable;
    Ligacao *m_lbd;
    QList<ControlHandler*> *m_lch;
    int m_dataPort;
    QString m_localAdd;
    
};

#endif // CONTROLHANDLER_H
