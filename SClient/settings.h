#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QDataStream>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QIODevice>
#include <QCryptographicHash>
#include <QDebug>
#include <QList>
#include <QString>
#include <QPair>

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);

public slots:
    void load(QString user);
    void save();
    void init(QString user, QMap<QString, QPair<QString, QByteArray> > mapFiles);
    QList<QString> getInvalidCodes();
    QMap<QString, QPair<QString, QByteArray> > getValidCodes();
    void addCode(QString fPathName, QString code);
    void removeCode(QString code);

    
signals:
    
public slots:



private:
    QList<QString> m_invalidCodes;
    QString m_fName;
    QFile *m_file;
    QString m_user;
    QMap<QString, QPair<QString, QByteArray> > m_mapFiles;
    
};

#endif // SETTINGS_H
