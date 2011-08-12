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
#include <fsliderdevice.h>
#include <fhalservice.h>

#define HAL_SERVICE_NAME          "org.freedesktop.Hal"
#define HAL_DEVICE_INTERFACE_NAME "org.freedesktop.Hal.Device"
#define HAL_DEVICE_METHOD_NAME    "PropertyModified"

#include <QDebug>

FSliderDevice::FSliderDevice(const QString& path, QObject *parent):
    FDBusProxy(path, parent),
    open(false),
    hal(FHALService::instance())
{
    serviceName   = HAL_SERVICE_NAME;
    interfaceName = HAL_DEVICE_INTERFACE_NAME;
    memberName    = HAL_DEVICE_METHOD_NAME;

    QObject::connect (hal, SIGNAL(valueChanged()), this, SLOT(onHALStateChanged()), Qt::QueuedConnection);
}

void FSliderDevice::start(QObject *requestor)
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
                    interfaceName, SLIDER_BUS, this);

        // Listen to updates
        SLIDER_BUS.connect(
                    serviceName, device.path(),
                    interfaceName, memberName,
                    this, SLOT(updated()));

        // Set initial value
        QMetaObject::invokeMethod(this, "updated", Qt::QueuedConnection);

        qDebug("Start keyboard suppervision");
    }
}

void FSliderDevice::stop(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (started && proxy) {

        // Disconnect from signal notifications
        SLIDER_BUS.disconnect(
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

        qDebug("Stopped keyboard supervision");
    }
}

void FSliderDevice::onHALStateChanged()
{
    if(started) {
        stop();
        if (hal->isReady()) {
            start();
        }
    }
}

void FSliderDevice::updated()
{

#define HAL_GET_PROPERTY        "GetProperty"
#define HAL_GET_KEYBOARD_STATUS "button.state.value"

    watcher = new QDBusPendingCallWatcher(proxy->asyncCall(HAL_GET_PROPERTY, HAL_GET_KEYBOARD_STATUS));
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(callback(QDBusPendingCallWatcher*)));

#undef HAL_GET_PROPERTY
#undef HAL_GET_KEYBOARD_STATUS
}

/// Called when the GetProperty D-Bus call is done.
void FSliderDevice::callback(QDBusPendingCallWatcher* pcw)
{
    QDBusPendingReply<bool> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        if (open == reply.argumentAt<0>()) {
            open = !open;
            Q_EMIT valueChanged();
        }
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

bool FSliderDevice::isOpen() const {
    return open;
}

#include "moc_fsliderdevice.cpp"
