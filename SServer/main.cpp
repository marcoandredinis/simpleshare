#include <QApplication>
#include "qserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QServer *qs = new QServer(QServer::Primario);
    //QServer *qs = new QServer(QServer::Secundario);
    
    return a.exec();
}
