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
#ifndef MEMCACHE_H
#define MEMCACHE_H

#include <QtCore>
#include <libmemcached/memcached.h>

class MemCache
{
public:
    MemCache();
    ~MemCache();

    QString getErrorString(int err);
    int setServers(const QList<QByteArray> &servers);

    bool exists(const QString &key);
    QByteArray get(const QString &key);
    int set(const QString &key, const QByteArray &value);
    int remove(const QString &key);

private:
    QList<QByteArray> servers;
    memcached_st *mc;
};

#endif // MEMCACHE_H