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

#include "fsliderdevice.h"

const QString FSliderDevice::serviceName   = "org.freedesktop.Hal";
const QString FSliderDevice::interfaceName = "org.freedesktop.Hal.Device";

FSliderDevice::FSliderDevice(const QString& path, QObject *parent):
    QObject(parent),
    open(false),
    device(path),
    proxy(0),
    watcher(0)
{
    SLIDER_BUS.connect(
                serviceName, path, interfaceName, "PropertyModified",
                this, SLOT(updated()));

    proxy = new AsyncDBusInterface(serviceName, path,
                                   interfaceName, SLIDER_BUS, this);

    // Set initial value
    QMetaObject::invokeMethod(this, "updated", Qt::QueuedConnection);
}

FSliderDevice::~FSliderDevice() {
    SLIDER_BUS.disconnect(
                serviceName, device.path(), interfaceName, "PropertyModified",
                this, SLOT(updated()));

    delete proxy;
    proxy = 0;
    delete watcher;
    watcher = 0;
}

/// Called when the GetProperty D-Bus call is done.
void FSliderDevice::callback(QDBusPendingCallWatcher* pcw)
{
    QDBusPendingReply<bool> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        if (open == reply.argumentAt<0>()) {
            open = !open;
            Q_EMIT isOpenChanged();
        }
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

/// Connected to the D-Bus signal Update from FSlider /
/// proxy. Check if the change is relevant, and if so, signal
/// that the device is connected.
void FSliderDevice::updated()
{
    watcher = new QDBusPendingCallWatcher(
                proxy->asyncCall("GetProperty", "button.state.value"));

    QObject::connect(watcher,
                     SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this,
                     SLOT(callback(QDBusPendingCallWatcher*)));
}

bool FSliderDevice::isOpen() const {
    return open;
}

#include "moc_fsliderdevice.cpp"
