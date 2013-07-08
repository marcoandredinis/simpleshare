#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "msgsocket.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(QTcpSocket *sSource, QObject *parent = 0);
    
    enum ReadWrite{
        Undifined,
        ReadRequest,
        WriteRequest
    };

signals:
    void newUp(QString code, DataHandler *dh);
    void newDown(QString code, DataHandler *dh);
    
public slots:
    void receiveRequest(QMap<QString, QString> info);
    void receiveChunck(QByteArray b);
    int getType();
    QString getCode();
    QMap<QString, QString> getInfo();
    MsgSocket* getMsgSocket();
    QTcpSocket *getSocket();
    void startTransfer(DataHandler *dhR);

private slots:

private:
    QTcpSocket *m_socket;
    MsgSocket *m_msg;
    MsgSocket *m_msgReceptor;
    QMap<QString, QString> m_info;
    QList<QByteArray> *m_lbff;
    int m_type;
    bool m_transfering;

};

#endif // DATAHANDLER_H
