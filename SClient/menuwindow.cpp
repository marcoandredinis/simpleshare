#include "menuwindow.h"
#include "ui_menuwindow.h"

MenuWindow::MenuWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MenuWindow)
{
    ui->setupUi(this);
    setUpTables();

}



MenuWindow::~MenuWindow()
{
    emit closingMenu();
    delete ui;

}

void MenuWindow::setUpTables(){
    ui->tblMyCodes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblMyCodes->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tblDownloads->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblDownloads->setSelectionMode(QAbstractItemView::SingleSelection);
}


void MenuWindow::setNick(QString nick){
    m_nick = nick;
    setWindowTitle("Menu - "+ m_nick);
}

void MenuWindow::receivedFile(QString code, QString filename){
    ui->txtLog->append("File received: "+filename+", code "+ code);
}


void MenuWindow::resetInterface(){
    ui->lblCode->setText("");
    ui->label->setText("");

    while (ui->tblDownloads->rowCount() > 0)
        ui->tblDownloads->removeRow(0);


    while (ui->tblMyCodes->rowCount() > 0)
        ui->tblMyCodes->removeRow(0);
}


void MenuWindow::closeEvent(QCloseEvent *e){
    emit closingMenu();
}

void MenuWindow::newCodeFromFile(QString code, QString fname){
    QPushButton *pb = new QPushButton("Remove");
    ui->lblCode->setText("File: " + fname + ", code: " + code);
    int rowIndex = ui->tblMyCodes->rowCount();
    ui->tblMyCodes->insertRow(rowIndex);

    ui->tblMyCodes->setItem(rowIndex, 0, new QTableWidgetItem(code));
    ui->tblMyCodes->setItem(rowIndex, 1, new QTableWidgetItem(fname));
    ui->tblMyCodes->setCellWidget(rowIndex, 2, pb);


    connect(pb, &QPushButton::clicked, [this, code](){
        //qDebug() << "Rem code" << code;

        int row = -1;
        for(int i=0; i<ui->tblMyCodes->rowCount(); i++){
            if( ui->tblMyCodes->item(i, 0)->text() == code)
                row = i;
            //qDebug() << ui->tblMyCodes->item(i, 0)->text() << code;
        }

        if( row != -1){
            //qDebug() << "remove" << row;
            ui->tblMyCodes->removeRow(row);
        }
        emit remCode(code);
    });
}

void MenuWindow::appendToLog(QString txt){
    ui->txtLog->append(txt);
}



void MenuWindow::on_btnSelectFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open Document"),
                QDir::homePath(),
                tr("All files (*)") );

    ui->txtFileLocation->setText(filename);
}

void MenuWindow::on_btnDownload_clicked()
{
    QMap<QString, QString> msg;
    msg["tipo"] = "downCode";
    msg["code"] = ui->txtCode->text();
    msg["seek"] = QString::number(0);
    //msg["transferMode"] = "2";


    int rowIndex = ui->tblDownloads->rowCount();
    ui->tblDownloads->insertRow(rowIndex);

    ui->tblDownloads->setItem(rowIndex, 0, new QTableWidgetItem(msg["code"]));
    QProgressBar *pb = new QProgressBar;
    pb->setValue(0);
    ui->tblDownloads->setCellWidget(rowIndex, 1, pb);

    emit askDownload(msg);
}

void MenuWindow::on_btnUpload_clicked()
{
    QFileInfo f(ui->txtFileLocation->text());

    if( !f.exists() ){
        QMessageBox msgBox;
        msgBox.setText("File does not exist");
        msgBox.exec();
        return;
    }

    QMap<QString, QString> msg;
    msg["tipo"] = "upCode";
    msg["nick"] = m_nick;
    msg["fName"] = f.fileName();
    msg["fPath"] = f.path();
    msg["fSize"] = QString::number(f.size());

    emit askUpload(msg);
}

void MenuWindow::errorDownloading(QString code){
    QFileInfo fi(code+".qdownload");
    int received = fi.size();

    int row = -1;
    for(int i=0; i<ui->tblDownloads->rowCount(); i++){
        if( ui->tblDownloads->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;

    QProgressBar *pb = dynamic_cast<QProgressBar*>(ui->tblDownloads->cellWidget(row, 1));
    pb->setFormat("Paused");
    //pb->setValue(0);

    QPushButton *btnContinue = new QPushButton("Continue");
    ui->tblDownloads->setCellWidget(row, 2, btnContinue);

    connect(btnContinue, &QPushButton::clicked, [this, code, received, btnContinue](){
        QMap<QString, QString> msg;
        msg["tipo"] = "downCode";
        msg["code"] = code;
        msg["seek"] = QString::number(received);
        //msg["transferMode"] = "2";
        emit askDownload(msg);
        btnContinue->setEnabled(false);
    });
}

void MenuWindow::recoverDownload(QString code, int received){
    int rowIndex = ui->tblDownloads->rowCount();
    ui->tblDownloads->insertRow(rowIndex);

    ui->tblDownloads->setItem(rowIndex, 0, new QTableWidgetItem(code));

    QProgressBar *pb = new QProgressBar;
    ui->tblDownloads->setCellWidget(rowIndex, 1, pb);

    pb->setFormat("Unknown Status");
    pb->setValue(0);

    QPushButton *btnContinue = new QPushButton("Continue");
    ui->tblDownloads->setCellWidget(rowIndex, 2, btnContinue);

    connect(btnContinue, &QPushButton::clicked, [this, code, received, btnContinue](){
        QMap<QString, QString> msg;
        msg["tipo"] = "downCode";
        msg["code"] = code;
        msg["seek"] = QString::number(received);
        //msg["transferMode"] = "2";
        emit askDownload(msg);
        btnContinue->setEnabled(false);

    });
}

void MenuWindow::setDownloadPercent(QString code, int total, int received){
    int row = -1;
    for(int i=0; i<ui->tblDownloads->rowCount(); i++){
        if( ui->tblDownloads->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;

    int percent = static_cast<int>(((double)received/total)*100);
    QProgressBar *pb = dynamic_cast<QProgressBar*>(ui->tblDownloads->cellWidget(row, 1));
    pb->setFormat("%p%");
    pb->setValue(percent);

    //if( percent == 100){
    //    completedDownload(code);
    //}
}

void MenuWindow::completedDownload(QString code, QString downloadFolder){
    int row = -1;
    for(int i=0; i<ui->tblDownloads->rowCount(); i++){
        if( ui->tblDownloads->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;
    //qDebug() << "Completed download";

    QProgressBar *pb = dynamic_cast<QProgressBar*>(ui->tblDownloads->cellWidget(row, 1));
    pb->setFormat("Done");
    pb->setValue(100);


    QPushButton *btnOpenFolder = new QPushButton("Open Folder");
    ui->tblDownloads->setCellWidget(row, 2, btnOpenFolder);

    connect(btnOpenFolder, &QPushButton::clicked, [this, downloadFolder](){
        //QString path = QDir::toNativeSeparators(QApplication::applicationDirPath());
        QDesktopServices::openUrl(QUrl("file:///" + downloadFolder));
    });

}

void MenuWindow::askedCodeNotFound(QString code){
    int row = -1;
    for(int i=0; i<ui->tblDownloads->rowCount(); i++){
        if( ui->tblDownloads->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;
    QProgressBar *pb = dynamic_cast<QProgressBar*>(ui->tblDownloads->cellWidget(row, 1));
    pb->setFormat("Not Found");
}

void MenuWindow::askedCodeNotAvailable(QString code){
    int row = -1;
    for(int i=0; i<ui->tblDownloads->rowCount(); i++){
        if( ui->tblDownloads->item(i, 0)->text() == code)
            row = i;
    }

    if( row == -1) return;
    QProgressBar *pb = dynamic_cast<QProgressBar*>(ui->tblDownloads->cellWidget(row, 1));
    pb->setFormat("Not Available");


    QPushButton *btnTryAgain = new QPushButton;
    btnTryAgain->setText("Try again");

    ui->tblDownloads->setCellWidget(row, 2, btnTryAgain);

    connect(btnTryAgain, &QPushButton::clicked, [this](){
        QMap<QString, QString> msg;
        msg["tipo"] = "downCode";
        msg["code"] = ui->txtCode->text();
        msg["seek"] = QString::number(0);
        emit askDownload(msg);
    });
}


void MenuWindow::on_btnDownloadFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open Directory"),
                    QDir::homePath(),
                    QFileDialog::ShowDirsOnly
                    | QFileDialog::DontResolveSymlinks);


    m_path = dir;

    qDebug() << dir;

    if( m_path.size() > 0)
        emit downloadPathChanged(m_path);
}
