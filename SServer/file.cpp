#include "file.h"

File::File(QString code, QString dono, QString name, QString path, QString ext, int size, QObject *parent) :
    QObject(parent)
{
    m_code = code;
    m_dono = dono;
    m_name = name;
    m_path = path;
    m_ext = ext;
    m_size = size;
}

QString File::getCode()         {   return m_code;          }
QString File::getDono()         {   return m_dono;          }
QString File::getName()         {   return m_name;          }
QString File::getPath()         {   return m_path;          }
QString File::getExt()          {   return m_ext;           }
int     File::getSize()         {   return m_size;          }

