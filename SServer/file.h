#ifndef FILE_H
#define FILE_H

#include <QObject>
#include <QTcpSocket>
#include <QTime>

class File : public QObject
{
    Q_OBJECT
public:
    explicit File(QString code, QString dono, QString name, QString path, QString ext, int size, QObject *parent = 0);
    QString getCode();
    QString getDono();
    QString getName();
    QString getPath();
    QString getExt();
    int getSize();


signals:

public slots:


private:
    QString m_code;
    QString m_dono;
    QString m_name;
    QString m_path;
    QString m_ext;
    int m_size;
};

#endif // FILE_H
