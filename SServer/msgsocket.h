#ifndef MSGSOCKET_H
#define MSGSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

class MsgSocket : public QObject
{
    Q_OBJECT
public:
    explicit MsgSocket(QTcpSocket *sock, QObject *parent = 0);
    void sendMsg(QMap<QString, QString> source);
    void sendRawData(QByteArray data);

signals:
    void newMessage(QMap<QString, QString> msg);
    void newRawChunck(QByteArray data);

public slots:
    void readyRead();
    void readRaw();


private:
    bool m_dataMode;
    QTcpSocket *m_socket;
    QByteArray *m_buffer;
    int m_msgSize;
};

#endif // MSGSOCKET_H
