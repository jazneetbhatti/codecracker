#ifndef QUERYINTERFACE_H
#define QUERYINTERFACE_H

#include <QtCore/QtCore>

class MemCache;

class QueryInterface
{
public:
    explicit QueryInterface();
    ~QueryInterface();
    QByteArray query(QString queryString, QString key);

private:
    MemCache *m_cache;
};

#endif // QUERYINTERFACE_H