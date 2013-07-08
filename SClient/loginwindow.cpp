#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);


}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::closeEvent(QCloseEvent *e){
    emit closingLogin();
}


void LoginWindow::disableForm(QString reason){
    ui->txtName->setEnabled(false);
    ui->txtPass->setEnabled(false);
    ui->btnLogin->setEnabled(false);
    ui->btnRegister->setEnabled(false);
    ui->statusBar->showMessage(reason);
}

void LoginWindow::enableForm(QString reason){
    ui->txtName->setEnabled(true);
    ui->txtPass->setEnabled(true);
    ui->btnLogin->setEnabled(true);
    ui->btnRegister->setEnabled(true);
    ui->statusBar->showMessage(reason);
}

void LoginWindow::on_btnLogin_clicked(){
    QMap<QString, QString> msg;
    msg["tipo"] = "login";
    msg["nick"] = ui->txtName->text();
    msg["passe"] = ui->txtPass->text();

    disableForm("Sending login info...");

    emit login(msg);
}

void LoginWindow::on_btnRegister_clicked()
{
    emit showRegister();
}
