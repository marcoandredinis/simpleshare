#ifndef CONTROLHANDLER_H
#define CONTROLHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QThread>
#include "msgsocket.h"
#include "sendfile.h"

class ControlHandler : public QObject
{
    Q_OBJECT
public:
    explicit ControlHandler(QList<QString> ips, QObject *parent = 0);
    
signals:
    void connectedToHost();
    void disconnectedFromHost(QString errorToString);
    void okLogin(QString nick);
    void badLogin(QString reason);
    void okRegistry(QString nick);
    void badRegistry(QString reason);
    void appendToLog(QString txt);
    void newCodeFromFile(QMap<QString, QString> info);

    void uploadFile(QMap<QString, QString> info);
    void askFile(QMap<QString, QString> info);

    void codeNotFound(QString code);
    void codeNotAvailable(QString code);

    
public slots:
    void sendMsg(QMap<QString, QString> info);
    void readMsg(QMap<QString, QString> info);
    void removeCodes(QList<QString> invalidCodes);
    void connectToHost(int pos);

private:
    MsgSocket *m_msg;
    QTcpSocket *m_socket;
    QString m_nick;
    QList<QString> m_ips;
    QString m_serverIP;
    
};

#endif // CONTROLHANDLER_H
