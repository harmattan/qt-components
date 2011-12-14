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

#define DEVICE_SERVICE_NAME             "com.nokia.phone.net";
#define DEVICE_INTERFACE_NAME           "Phone.Net"
#define DEVICE_SIGNAL_INTERFACE_NAME    "Phone.Net"

#define DEVICE_SIGNAL_STRENGTH_NAME     "signal_strength_change"
#define DEVICE_REGISTRATION_STATUS_NAME "registration_status_change"
#define DEVICE_PROVIDER_NAME            "operator_name_change"
#define DEVICE_RADIO_MODE_NAME          "radio_access_technology_change"

#include <QDebug>

FCellDevice::FCellDevice(const QString& path, QObject *parent):
    FDBusProxy(path, path, parent),
    signalStrength(0),
    services(0),
    status(NETWORK_REG_STATUS_NOSERV),
    offline(true),
    provider(""),
    radioMode(0),
    service(FPhoneService::instance())
{
    serviceName      = DEVICE_SERVICE_NAME;
    interfaceName    = DEVICE_INTERFACE_NAME;
    signalName       = DEVICE_SIGNAL_INTERFACE_NAME;

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

        // Listen to signal strength update
        CELL_BUS.connect(
                    serviceName, signal.path(),
                    signalName, DEVICE_SIGNAL_STRENGTH_NAME,
                    this, SLOT(onSignalStrengthChanged(uchar, uchar)));

        // Liste to register status updates
        CELL_BUS.connect(
                    serviceName, signal.path(),
                    signalName, DEVICE_REGISTRATION_STATUS_NAME,
                    this, SLOT(onRegistrationStatusChanged(QDBusMessage)));

        //Listen to operator name changes
        CELL_BUS.connect(
                    serviceName, signal.path(),
                    signalName, DEVICE_PROVIDER_NAME,
                    this, SLOT(onProviderChanged(QString)));

        //Listen to radio mode changes
        CELL_BUS.connect(
                    serviceName, signal.path(),
                    signalName, DEVICE_RADIO_MODE_NAME,
                    this, SLOT(onRadioModeChanged(uchar)));

        // Set initial value
        QMetaObject::invokeMethod(this, "setSignalStrength", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "setRegistrationStatus", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "setRadioMode", Qt::QueuedConnection);
    }
}

void FCellDevice::stop(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (started && proxy) {

        // Disconnect from signal notifications
        CELL_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, DEVICE_SIGNAL_STRENGTH_NAME,
                    this, SLOT(onSignalStrengthChanged(uchar, uchar)));

        CELL_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, DEVICE_REGISTRATION_STATUS_NAME,
                    this, SLOT(onRegistrationStatusChanged(QDBusMessage)));

        CELL_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, DEVICE_PROVIDER_NAME,
                    this, SLOT(onProviderChanged(QString)));

        CELL_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, DEVICE_RADIO_MODE_NAME,
                    this, SLOT(onRadioModeChanged(uchar)));

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
int FCellDevice::getSignalStrength() const
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

int FCellDevice::getStatus() const
{
    return status;
}

void FCellDevice::setRegistrationStatus()
{
#define DEVICE_GET_REGISTRATION_STATUS "get_registration_status"
    if(started) {
        watcher = new QDBusPendingCallWatcher(proxy->asyncCall(DEVICE_GET_REGISTRATION_STATUS));
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(onRegistrationStatusReply(QDBusPendingCallWatcher*)));
    }
#undef DEVICE_GET_REGISTRATION_STATUS
}

bool FCellDevice::isOffline()
{
    return offline;
}

int FCellDevice::getServices() const
{
    return services;
}

QString FCellDevice::getProvider() const
{
    return provider;
}

void FCellDevice::setProvider(uint operator_code, uint country_code)
{
#define DEVICE_GET_OPERATOR_NAME "get_operator_name"
    if(started) {
        watcher = new QDBusPendingCallWatcher(proxy->asyncCall(DEVICE_GET_OPERATOR_NAME, QVariant::fromValue(uchar(0)), operator_code, country_code));
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(onProviderReply(QDBusPendingCallWatcher*)));
    }
#undef DEVICE_GET_OPERATOR_NAME
}

int FCellDevice::getRadioMode() const
{
    return radioMode;
}

void FCellDevice::setRadioMode()
{
#define DEVICE_GET_RADIO_MODE "get_radio_access_technology"
    if(started) {
        watcher = new QDBusPendingCallWatcher(proxy->asyncCall(DEVICE_GET_RADIO_MODE));
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(onRadioModeReply(QDBusPendingCallWatcher*)));
    }
#undef DEVICE_GET_RADIO_MODE
}

// Signals
void FCellDevice::onSignalStrengthReply(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<uchar, uchar> reply = *pcw;
    if (!reply.isError()) {
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

void FCellDevice::onSignalStrengthChanged(uchar bars, uchar dbm)
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

void FCellDevice::onRegistrationStatusReply(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<uchar, ushort, int, int, int, uchar, uchar> reply = *pcw;
    if (!reply.isError()) {
        uint operator_code = reply.argumentAt<3>();
        uint country_code  = reply.argumentAt<4>();
        // Get operator Name and Network services
        setProvider(operator_code, country_code);
        onRegistrationStatusChanged(reply.reply());
    }
    else {
        qWarning() << reply.error().message();
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

void FCellDevice::onRegistrationStatusChanged(QDBusMessage msg)
{
    int old_status = status;
    int new_status = msg.arguments().at(3).toInt();

#define TOGGLE_OFFLINE(v) if (offline == v) { offline = !offline; Q_EMIT offlineChanged(); }

    status   = new_status;
    services = msg.arguments().at(7).toInt();

    if (old_status != new_status) {
        switch (new_status) {
        case NETWORK_REG_STATUS_HOME:
        case NETWORK_REG_STATUS_ROAM:
        case NETWORK_REG_STATUS_ROAM_BLINK:
            if (old_status >= NETWORK_REG_STATUS_NOSERV) {
                Q_EMIT statusChanged();
            }
            TOGGLE_OFFLINE(true);
            break;

        case NETWORK_REG_STATUS_NOSERV_NOSIM:
            Q_EMIT statusChanged();
            TOGGLE_OFFLINE(false);
            break;

        default:
            if (old_status < NETWORK_REG_STATUS_NOSERV || old_status == NETWORK_REG_STATUS_NOSERV_NOSIM) {
                Q_EMIT statusChanged();
            }
            TOGGLE_OFFLINE(false);
            break;
        }

#undef TOGGLE_OFFLINE
    }
}

void FCellDevice::onProviderReply(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<QString> reply = *pcw;
    if (!reply.isError()) {
        onProviderChanged(reply.argumentAt<0>());
    }
    else {
        qWarning() << reply.error().message();
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

void FCellDevice::onProviderChanged(QString name)
{
    if (provider != name) {
        provider = name;
        Q_EMIT providerChanged();
    }
}

void FCellDevice::onRadioModeReply(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<uchar> reply = *pcw;
    if (!reply.isError()) {
        onRadioModeChanged(reply.argumentAt<0>());
    }
    else {
        qWarning() << reply.error().message();
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

void FCellDevice::onRadioModeChanged(uchar mode)
{
    if (radioMode != mode) {
        radioMode = mode;
        Q_EMIT radioModeChanged();
    }
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

