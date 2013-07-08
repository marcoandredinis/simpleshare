#include "serverwindow.h"
#include "ui_serverwindow.h"

ServerWindow::ServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerWindow)
{
    ui->setupUi(this);
    setupTables();
}

ServerWindow::~ServerWindow()
{
    delete ui;
}


void ServerWindow::setupTables(){
    ui->tUtilizadores->setColumnCount(1);
    ui->tUtilizadores->setHorizontalHeaderItem(0, new QTableWidgetItem("Nome"));
    ui->tUtilizadores->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tUtilizadores->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tFicheiros->setColumnCount(4);
    ui->tFicheiros->setHorizontalHeaderItem(0, new QTableWidgetItem("Codigo"));
    ui->tFicheiros->setHorizontalHeaderItem(1, new QTableWidgetItem("Nome"));
    ui->tFicheiros->setHorizontalHeaderItem(2, new QTableWidgetItem("Dono"));
    ui->tFicheiros->setHorizontalHeaderItem(3, new QTableWidgetItem("Tamanho"));
    ui->tFicheiros->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tFicheiros->setSelectionMode(QAbstractItemView::SingleSelection);
}

void ServerWindow::addToLog(QString s){
    ui->log->append(s);
}

void ServerWindow::insereFicheiro(File *f){
    int row = ui->tFicheiros->rowCount();
    ui->tFicheiros->insertRow(row);
    ui->tFicheiros->setItem(row, 0, new QTableWidgetItem(f->getCode()));
    ui->tFicheiros->setItem(row, 1, new QTableWidgetItem(f->getName()));
    ui->tFicheiros->setItem(row, 2, new QTableWidgetItem(f->getDono()));
    ui->tFicheiros->setItem(row, 3, new QTableWidgetItem(QString::number(f->getSize())));
}

void ServerWindow::onlineUser(QString s){
    ui->log->append("Online " + s);
    int row = ui->tUtilizadores->rowCount();
    ui->tUtilizadores->insertRow(row);
    ui->tUtilizadores->setItem(row, 0, new QTableWidgetItem(s));
}

void ServerWindow::offlineUser(QString s){
    ui->log->append("Offline " + s);

    for(int i=0; i<ui->tUtilizadores->rowCount(); i++)
        if( ui->tUtilizadores->item(i, 0)->text() == s){
            ui->tUtilizadores->removeRow(i);
            return;
        }
}



void ServerWindow::removedCode(QString code){
    int row = -1;
    for(int i=0; i<ui->tFicheiros->rowCount(); i++){
        if( ui->tFicheiros->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;
    ui->tFicheiros->removeRow(row);
}
