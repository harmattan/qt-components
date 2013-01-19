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

#ifndef FBMEDEVICE_H
#define FBMEDEVICE_H

#include "fhalservice.h"

#define BME_DEVICE "/org/freedesktop/Hal/devices/bme"
#define BME_BUS    QDBusConnection::systemBus()

class FBMEDevice : public FDBusProxy
{
    Q_OBJECT

public:
    Q_PROPERTY(int  level READ batteryLevel NOTIFY levelChanged() FINAL)
    Q_PROPERTY(bool charging READ isCharging NOTIFY chargingChanged() FINAL)

Q_SIGNALS:
    void levelChanged();
    void chargingChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FBMEDevice(const QString& path, QObject *parent = 0);

public:
    int batteryLevel() const;
    bool isCharging() const;

private:
    int level;
    bool charging;

    FService *hal;

private Q_SLOTS:
    void onHALStateChanged();
    void chargingCallback(QDBusPendingCallWatcher* watcher);
    void levelCallback(QDBusPendingCallWatcher* watcher);
    void updated(int n = 0, QList<FHALProperty> updates = QList<FHALProperty>());
};

#endif /* ! FBMEDEVICE_H */
