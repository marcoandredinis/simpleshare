#ifndef QSERVER_H
#define QSERVER_H

#include "serverwindow.h"

#include "dataserver.h"
#include "controlserver.h"

#include "ligacao.h"

#include "spreadapi.h"

#include <QObject>
#include <QTime>

class QServer : public QObject
{
    Q_OBJECT
public:
    explicit QServer(int mode, QObject *parent = 0);
    void initPrim(QString dbPath);
    void initSec();
    enum{
        Primario,
        Secundario
    };
    
signals:
    
public slots:
    
private:
    int m_mode;
};

#endif // QSERVER_H
