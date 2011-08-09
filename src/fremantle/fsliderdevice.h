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

#ifndef FSLIDERDEVICE_H
#define FSLIDERDEVICE_H

#include <QDBusError>
#include <QDBusObjectPath>
#include <QDBusInterface>

#include <QMap>
#include <QString>

#define SLIDER_BUS QDBusConnection::systemBus()

class QDBusPendingCallWatcher;
class AsyncDBusInterface;

class FSliderDevice : public QObject
{
    Q_OBJECT

public:
  Q_PROPERTY(bool open READ isOpen NOTIFY isOpenChanged FINAL)

public:
  explicit FSliderDevice(const QString& path, QObject *parent = 0);
    ~FSliderDevice();

    bool isOpen() const;

Q_SIGNALS:
    void isOpenChanged();

private Q_SLOTS:
    void updated();
    void callback(QDBusPendingCallWatcher* pcw);

private:
    bool open;

    // DBus Stuff
    static const QString     serviceName;
    static const QString     interfaceName;
    QDBusObjectPath          device;

    AsyncDBusInterface*      proxy;               

    QDBusPendingCallWatcher* watcher;

};

#endif /* ! FSLIDERDEVICE_H */
