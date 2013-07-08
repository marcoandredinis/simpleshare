#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>
#include <QProgressBar>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QTime>

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MenuWindow(QWidget *parent = 0);
    ~MenuWindow();
    void setNick(QString nick);

signals:
    void askUpload(QMap<QString, QString> msg);
    void downloadPathChanged(QString path);
    void askDownload(QMap<QString, QString> msg);
    void remCode(QString code);
    void closingMenu();

public slots:
    void receivedFile(QString code, QString filename);
    void newCodeFromFile(QString code, QString fname);
    void appendToLog(QString txt);
    void setDownloadPercent(QString code, int total, int received);
    void completedDownload(QString code, QString downloadFolder);
    void askedCodeNotFound(QString code);
    void askedCodeNotAvailable(QString code);
    void recoverDownload(QString code, int received);
    void errorDownloading(QString code);
    void closeEvent(QCloseEvent *e);
    void resetInterface();

    
private slots:
    void setUpTables();
    void on_btnSelectFile_clicked();
    void on_btnDownload_clicked();
    void on_btnUpload_clicked();
    void on_btnDownloadFolder_clicked();

private:
    Ui::MenuWindow *ui;
    QString m_nick;
    QString m_path;
};

#endif // MENUWINDOW_H
