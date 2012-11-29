/*
 * Copyright (C) 2008  Wang Hoi <fearee@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
*/
#include <memcache.h>
#include <QtCore>
#include <libmemcached/memcached.h>

MemCache::MemCache()
{
    mc = memcached_create(NULL);
}

MemCache::~MemCache()
{
    memcached_free(mc);
}

QString MemCache::getErrorString(int err)
{
    return QString(memcached_strerror(mc, memcached_return(err)));
}

int MemCache::setServers(const QList<QByteArray> &servers)
{
    int i;
    memcached_return err;
    this->servers = servers;
    foreach(QByteArray server, servers) {
        err = MEMCACHED_FAILURE;
        if (server.startsWith('/')) {
            err = memcached_server_add_unix_socket(mc, (server + '\0').constData());
        } else {
            i = server.lastIndexOf(':');
            err = memcached_server_add(mc, (server.left(i) += '\0').constData(), (server.right(server.size() - i - 1) + '\0').toUInt());
        }
        if (err != MEMCACHED_SUCCESS) {
            return err;
        }
    }
    return 0;
}

QByteArray MemCache::get(const QString &key)
{
    char *buf;
    size_t value_length;
    QByteArray value;
    uint32_t flags = 0;
    memcached_return err;
    buf = memcached_get(mc, key.toUtf8().constData(), key.toUtf8().size(), &value_length, &flags, &err);
    if (err == MEMCACHED_SUCCESS)
        return QByteArray(buf, value_length);
    else
        return QByteArray();
}

int MemCache::set(const QString &key, const QByteArray &value)
{
    time_t expiration = 0;
    uint32_t flags = 0;
    memcached_return err;
    err = memcached_set(mc, key.toUtf8().constData(), key.toUtf8().size(), value.constData(), value.size(), expiration, flags);
    if (err == MEMCACHED_SUCCESS || err == MEMCACHED_BUFFERED)
        return 0;
    else
        return err;
}

int MemCache::remove(const QString &key)
{
    time_t expiration = 0;
    memcached_return err;
    err = memcached_delete(mc, key.toUtf8().constData(), key.toUtf8().size(), expiration);
    if (err == MEMCACHED_SUCCESS || err == MEMCACHED_BUFFERED)
        return 0;
    else
        return err;
}