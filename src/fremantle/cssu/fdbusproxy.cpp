/*
 * Copyright (C) 2011 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
*/

#include <asyncdbusinterface.h>

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

#include "fdbusproxy.h"

FDBusProxy::FDBusProxy(const QString& path, QObject *parent):
    QObject(parent),
    started(false),
    device(path),
    signal("/"),
    proxy(0),
    watcher(0)
{

}

FDBusProxy::FDBusProxy(const QString& path, const QString &signal_path, QObject *parent):
    QObject(parent),
    started(false),
    device(path),
    signal(signal_path),
    proxy(0),
    watcher(0)
{
}

FDBusProxy::~FDBusProxy()
{
    delete proxy;
    proxy = 0;
    delete watcher;
    watcher = 0;
}

bool FDBusProxy::isStarted() const
{
    return started;
}

void FDBusProxy::callback(QDBusPendingCallWatcher* pcw)
{
    Q_UNUSED(pcw);
    // Do nothing
}

#include "moc_fdbusproxy.cpp"
