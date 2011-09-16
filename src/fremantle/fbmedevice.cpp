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

#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

// Local classes
#include <asyncdbusinterface.h>
#include <fbmedevice.h>
#include <fhalservice.h>

#define HAL_SERVICE_NAME          "org.freedesktop.Hal"
#define HAL_DEVICE_INTERFACE_NAME "org.freedesktop.Hal.Device"
#define HAL_DEVICE_METHOD_NAME    "PropertyModified"

#include <QDebug>

FBMEDevice::FBMEDevice(const QString& path, QObject *parent):
    FDBusProxy(path, parent),
    level(0),
    charging(false),
    hal(FHALService::instance())
{
    serviceName   = HAL_SERVICE_NAME;
    interfaceName = HAL_DEVICE_INTERFACE_NAME;
    memberName    = HAL_DEVICE_METHOD_NAME;

    QObject::connect (hal, SIGNAL(valueChanged()), this, SLOT(onHALStateChanged()), Qt::QueuedConnection);
}

void FBMEDevice::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!proxy) {
        if (!started) {
            hal->start(this);
            // Start and register to hal service
            started = true;
        }

        proxy = new AsyncDBusInterface(
                    serviceName, device.path(),
                    interfaceName, BME_BUS, this);

        // Listen to updates
        BME_BUS.connect(
                    serviceName, device.path(),
                    interfaceName, memberName,
                    this, SLOT(updated(int, QList<FHALProperty>)));

        // Set initial value
        QMetaObject::invokeMethod(this, "updated", Qt::QueuedConnection);
    }
}

void FBMEDevice::stop(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (started && proxy) {

        // Disconnect from signal notifications
        BME_BUS.disconnect(
                    serviceName, device.path(),
                    interfaceName, memberName,
                    this, SLOT(updated()));


        // If mce is disconnected, service is already started but offline, We wait for
        // a hal to wake up again
        if (hal->isReady()) {
            // TODO: disconnect from hal supervision
            hal->stop(this);
            started = false;
        }

        // Remove all connections
        delete proxy; proxy = 0;
        delete watcher; watcher = 0;
    }
}

void FBMEDevice::onHALStateChanged()
{
    if(started) {
        stop();
        if (hal->isReady()) {
            start();
        }
    }
}

void FBMEDevice::updated(int n, QList<FHALProperty> updates)
{
#define BME_GET_PROPERTY          "GetProperty"
#define BME_BARS_KEY              "battery.charge_level.current"
#define BME_REPORTING_CURRENT_KEY "battery.reporting.current"
#define BME_IS_CHARGING_KEY       "battery.rechargeable.is_charging"

    if (!updates.length()) {
        updates << FHALProperty(BME_REPORTING_CURRENT_KEY);
    }
    for (int i = 0; i < updates.length(); ++i) {
        if (updates[i].name == BME_IS_CHARGING_KEY) {
            watcher = new QDBusPendingCallWatcher(proxy->asyncCall(BME_GET_PROPERTY, BME_IS_CHARGING_KEY));
            QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(chargingCallback(QDBusPendingCallWatcher*)));
        }
        if (updates[i].name == BME_REPORTING_CURRENT_KEY) {
            watcher = new QDBusPendingCallWatcher(proxy->asyncCall(BME_GET_PROPERTY, BME_BARS_KEY));
            QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(levelCallback(QDBusPendingCallWatcher*)));
        }
    }

#undef BME_IS_CHARGING_KEY
#undef BME_REPORTING_CURRENT_KEY
#undef BME_BARS_KEY
#undef BME_GET_PROPERTY

}

/// Called when the GetProperty D-Bus call is done.
void FBMEDevice::chargingCallback(QDBusPendingCallWatcher* pcw)
{
    QDBusPendingReply<bool> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        if (charging != reply.argumentAt<0>()) {
            charging = !charging;
            Q_EMIT chargingChanged();
        }
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

/// Called when the GetProperty D-Bus call is done.
void FBMEDevice::levelCallback(QDBusPendingCallWatcher* pcw)
{
    int new_value;
    QDBusPendingReply<int> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        new_value = reply.argumentAt<0>();
        if (level != new_value) {
            level = new_value;
            Q_EMIT levelChanged();
        }
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

int FBMEDevice::batteryLevel() const {
    return level;
}
bool FBMEDevice::isCharging() const {
    return charging;
}

#include "moc_fbmedevice.cpp"
