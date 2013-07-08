#ifndef REGISTRYWINDOW_H
#define REGISTRYWINDOW_H

#include <QMainWindow>
#include <QMap>
namespace Ui {
class RegistryWindow;
}

class RegistryWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RegistryWindow(QWidget *parent = 0);
    ~RegistryWindow();
    
public slots:
    void disableForm(QString reason);
    void enableForm(QString reason);

signals:
    void registry(QMap<QString, QString> info);
    void cancel();
    void closingRegistryWindow();

private slots:
    void on_confirmButton_clicked();
    void on_cancelButton_clicked();
    void closeEvent(QCloseEvent *e);

private:
    Ui::RegistryWindow *ui;
};

#endif // REGISTRYWINDOW_H
