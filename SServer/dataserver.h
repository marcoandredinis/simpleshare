#ifndef DATASERVER_H
#define DATASERVER_H

#include <QObject>
#include <QTcpServer>
#include "datahandler.h"

class DataServer : public QObject
{
    Q_OBJECT
public:
    explicit DataServer(QObject *parent = 0);

    
signals:
    
public slots:

private slots:
    void newConn();


private:
    void findPartner(QString code);
    QTcpServer *m_listen;
    QMap<QString, QList<DataHandler*> *> *m_downers;
    QMap<QString, QList<DataHandler*> *> *m_uppers;
};

#endif // DATASERVER_H
