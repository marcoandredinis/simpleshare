#ifndef SPREADRECMSG_H
#define SPREADRECMSG_H

#include <QObject>
#include <QDebug>
#include <sp.h>

class SpreadRecMsg : public QObject
{
    Q_OBJECT
public:
    explicit SpreadRecMsg(mailbox *mbox, QObject *parent = 0);
    
public slots:
    void waitForNewMsg();


signals:
    void receivedMsg(QMap<QString, QString> info);

public slots:

private:
    mailbox *m_mbox;
    
};

#endif // SPREADRECMSG_H
