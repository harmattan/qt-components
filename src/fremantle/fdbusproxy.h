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

#ifndef FDBUSPROXY_H
#define FDBUSPROXY_H

#include <QDBusError>
#include <QDBusObjectPath>
#include <QDBusInterface>

#include <QString>

class QDBusPendingCallWatcher;
class AsyncDBusInterface;

class FDBusProxy : public QObject
{

    // QtObject Stuff
    Q_OBJECT

    // Properties
public:
    Q_PROPERTY(bool started READ isStarted FINAL)

    // Constructors
public:
    explicit FDBusProxy(const QString& path, QObject *parent = 0);
    explicit FDBusProxy(const QString& path, const QString& signal_path, QObject *parent = 0);
    ~FDBusProxy();

    //Methods
public:
    virtual void start (QObject *requestor = 0) = 0;
    virtual void stop  (QObject *requestor = 0) = 0;

    // Signals
Q_SIGNALS:
    void valueChanged();

protected:
    // DBus Stuff
    QString            serviceName;
    QString            interfaceName;
    QString            memberName;

    QString            signalName;
    QString            signalMemberName;

    bool                     started;
    QDBusObjectPath          device;
    QDBusObjectPath          signal;

    // Used for Proxies Only
    AsyncDBusInterface*      proxy;               
    QDBusPendingCallWatcher* watcher;


protected Q_SLOTS:
    virtual void callback(QDBusPendingCallWatcher* pcw);

private:
    bool isStarted() const;
};

#endif /* ! FDBUSPROXY_H */
