#include "registrywindow.h"
#include "ui_registrywindow.h"

RegistryWindow::RegistryWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RegistryWindow)
{
    ui->setupUi(this);
}

RegistryWindow::~RegistryWindow()
{
    delete ui;
}

void RegistryWindow::closeEvent(QCloseEvent *e){
    emit closingRegistryWindow();
}

void RegistryWindow::disableForm(QString reason){
    ui->confirmButton->setEnabled(false);
    ui->cancelButton->setEnabled(false);
    ui->statusbar->showMessage(reason);
}

void RegistryWindow::enableForm(QString reason){
    ui->confirmButton->setEnabled(true);
    ui->cancelButton->setEnabled(true);
    ui->statusbar->showMessage(reason);
}

void RegistryWindow::on_confirmButton_clicked()
{
    QMap<QString, QString> msg;
    msg["tipo"] = "register";
    msg["nick"] = ui->txtName->text();
    msg["password"] = ui->txtPassword->text();
    msg["email"] = ui->txtEmail->text();
    msg["path"] = ui->txtPath->text();

    disableForm("Sending registry info...");

    emit registry(msg);
}

void RegistryWindow::on_cancelButton_clicked()
{
    //cena para voltar ao login
    emit cancel();
}
