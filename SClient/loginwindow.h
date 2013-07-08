#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

signals:
    void login(QMap<QString, QString> info);
    void showRegister();
    void closingLogin();

public slots:
    void disableForm(QString reason);
    void enableForm(QString reason);


private slots:
    void on_btnLogin_clicked();

    void on_btnRegister_clicked();
    void closeEvent(QCloseEvent *e);

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
