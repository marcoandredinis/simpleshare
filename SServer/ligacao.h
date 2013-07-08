#ifndef LIGACAO_H
#define LIGACAO_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QList>
#include "file.h"

class Ligacao : public QObject
{
    Q_OBJECT

public:
    explicit Ligacao(QString fName, QObject *parent = 0);
    ~Ligacao();
    void addUser(QString nome, QString email, QString password, QString path);
    void addFile(File *f);

    void remCode(QString code);

    QString getNameFile(int id);
    QString getOwnerFile(QString code);
    QString getExtensaoFile(QString nome);
    int getSizeFile(QString nome);
    QString getPathFile(QString nome);
    QString getNameUser(int id);
    QString getEmailUser(QString nome);
    QString getPasseUser(QString nome);
    QString getPathUser(QString nome);
    QString getEnderecoUser(QString nome);
    QList<QString>* getFilesUser(QString nome);



    bool existFile(QString code);
    File * getFile(QString code);
    bool existUser(QString name);
    void emitAllAddedFiles();
    void disconnectFromDB();

public slots:
    bool existCode(QString code);


signals:
    void addedFile(File *f);
    void removedFile(QString code);
    void addedUser(QString nome, QString email, QString password, QString path);

private:
    QSqlDatabase db;
};

#endif // LIGACAO_H
