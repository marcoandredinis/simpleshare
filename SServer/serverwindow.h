#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include "file.h"

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ServerWindow(QWidget *parent = 0);
    ~ServerWindow();
    
public slots:
    void insereFicheiro(File *f);
    void addToLog(QString s);
    void onlineUser(QString s);
    void offlineUser(QString s);

    void removedCode(QString code);

private:
    void setupTables();
    Ui::ServerWindow *ui;
};

#endif // SERVERWINDOW_H
