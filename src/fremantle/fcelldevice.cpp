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
#include <fcelldevice.h>
#include <fphoneservice.h>

#define DEVICE_SERVICE_NAME          "com.nokia.phone.net";
#define DEVICE_INTERFACE_NAME        "Phone.Net"
#define DEVICE_SIGNAL_INTERFACE_NAME "Phone.Net"
#define DEVICE_SIGNAL_MEMBER_NAME    "signal_strength_change"

#include <QDebug>

FCellDevice::FCellDevice(const QString& path, QObject *parent):
    FDBusProxy(path, path, parent),
    signalStrength(0),
    service(FPhoneService::instance())
{
    serviceName      = DEVICE_SERVICE_NAME;
    interfaceName    = DEVICE_INTERFACE_NAME;
    signalName       = DEVICE_SIGNAL_INTERFACE_NAME;
    signalMemberName = DEVICE_SIGNAL_MEMBER_NAME;

    QObject::connect (service, SIGNAL(valueChanged()), this,
                      SLOT(onServiceStateChanged()), Qt::QueuedConnection);
}

// Proxy members
void FCellDevice::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!proxy) {
        if (!started) {
            started = true;
            // Start and register to service
            service->start(this);
        }

        Q_ASSERT(service->isReady());
        proxy = new AsyncDBusInterface(
                    serviceName, device.path(),
                    interfaceName, CELL_BUS, this);

        // Listen to updates
        CELL_BUS.connect(
                    serviceName, signal.path(),
                    signalName, signalMemberName,
                    this, SLOT(onSignalStrength(int, int)));

        // Set initial value
        QMetaObject::invokeMethod(this, "setSignalStrength", Qt::QueuedConnection);
    }
}

void FCellDevice::stop(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (started && proxy) {

        // Disconnect from signal notifications
        CELL_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, signalMemberName,
                    this, SLOT(updated(int)));

        // If mce is disconnected, service is already started but offline, We wait for
        // a mce signal to wake up again
        if (service->isReady()) {
            // stop and unregister to mce service
            service->stop(this);
            started = false;
        }

        // Remove all connections
        delete proxy; proxy = 0;
        delete watcher; watcher = 0;
    }
}

// Target values
int FCellDevice::getSignalStrength()
{
    return signalStrength;
}

void FCellDevice::setSignalStrength()
{
#define DEVICE_GET_SIGNAL_STRENGTH "get_signal_strength"
    if(started) {
        watcher = new QDBusPendingCallWatcher(proxy->asyncCall(DEVICE_GET_SIGNAL_STRENGTH));
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(signalStrengthReply(QDBusPendingCallWatcher*)));
    }
#undef DEVICE_GET_SIGNAL_STRENGTH
}

// Signals
void FCellDevice::onSignalStrengthReply(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<int, int> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        onSignalStrengthChanged(reply.argumentAt<0>(), reply.argumentAt<1>());
    }
    else {
        qWarning() << reply.error().message();
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

void FCellDevice::onSignalStrengthChanged(int bars, int dbm)
{
#define MIN(a, b) (a > b ? b : a)

    Q_UNUSED(dbm);

    bars = MIN(bars, 100) + 20 / 21;
    if (signalStrength != bars) {
        signalStrength = bars;
        Q_EMIT signalStrengthChanged();
    }
#undef MIN
}

void FCellDevice::onServiceStateChanged()
{
    if(started) {
        stop();
        if (service->isReady()) {
            start();
        }
    }
}

#include "moc_fcelldevice.cpp"

