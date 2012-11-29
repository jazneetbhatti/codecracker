#include <queryinterface.h>
#include <memcache.h>

#include <QtCore/QtCore>
#include <QtSql/QtSql>

QueryInterface::QueryInterface()
{
    m_cache = new MemCache();
    QList<QByteArray> serverList;
    serverList << QByteArray("127.0.0.1:");
    m_cache->setServers(serverList);
}

QueryInterface::~QueryInterface()
{
    delete m_cache;
}

QByteArray QueryInterface::query(QString queryString, QString keyString)
{
    QByteArray result;
    if (queryString.isEmpty())
        return QByteArray();
    result = m_cache->get(keyString);
    if (!result.isNull())
    {
        return result;
    }
    else
    {
        QSqlQuery query(queryString);
        query.next();
        result = query.value(0).toByteArray();
        int failure = m_cache->set(keyString, result);
        if (failure)
            qDebug() << "memcached error while setting," << m_cache->getErrorString(failure);
        return result;
    }
}