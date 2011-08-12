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
#include <forientationdevice.h>
#include <fmceservice.h>

#define MCE_SERVICE_NAME          "com.nokia.mce";
#define MCE_INTERFACE_NAME        "com.nokia.mce.request"
#define MCE_SIGNAL_INTERFACE_NAME "com.nokia.mce.signal"
#define MCE_SIGNAL_MEMBER_NAME    "sig_device_orientation_ind"

#include <QDebug>

FOrientationDevice::FOrientationDevice(const QString& path, const QString &signal_path, QObject *parent):
    FDBusProxy(path, signal_path, parent),
    orientation("Undefined"),
    physicalOrientation("Undefined"),
    mce(FMCEService::instance())
{
    serviceName      = MCE_SERVICE_NAME;
    interfaceName    = MCE_INTERFACE_NAME;

    signalName       = MCE_SIGNAL_INTERFACE_NAME;
    signalMemberName = MCE_SIGNAL_MEMBER_NAME;

    QObject::connect (mce, SIGNAL(valueChanged()), this, SLOT(onMCEStateChanged()), Qt::QueuedConnection);
}

void FOrientationDevice::start(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (!proxy) {
        if (!started) {
            started = true;
            // Start and register to mce service
            mce->start(this);
        }

        Q_ASSERT(mce->isReady());
        proxy = new AsyncDBusInterface(
                    serviceName, device.path(),
                    interfaceName, ORIENTATION_BUS, this);

        // Listen to updates
        ORIENTATION_BUS.connect(
                    serviceName, signal.path(),
                    signalName, signalMemberName,
                    this, SLOT(signalUpdated(QString, QString, QString)));

        // Update orientation and enable if required accelerometers
        setOrientation(orientation == "Default" ? "Undefined": orientation);

        qDebug("Started orientation");
    }
}

void FOrientationDevice::stop(QObject *requestor)
{
    Q_UNUSED(requestor);

    if (started && proxy) {

        // Disconnect from signal notifications
        ORIENTATION_BUS.disconnect(
                    serviceName, signal.path(),
                    signalName, signalMemberName,
                    this, SLOT(signalUpdated(QString, QString, QString)));

        // Disable accelerometers if running
        setOrientation(orientation == "Undefined" ? "Default": orientation);

        // If mce is disconnected, service is already started but offline, We wait for
        // a mce signal to wake up again
        if (mce->isReady()) {
            // stop and unregister to mce service
            mce->stop(this);
            started = false;
        }

        // Remove all connections
        delete proxy; proxy = 0;
        delete watcher; watcher = 0;

        qDebug("Stopped orientation");
    }
}

void FOrientationDevice::onMCEStateChanged()
{
    if(started) {
        stop();
        if (mce->isReady()) {
            start();
        }
    }
}

void FOrientationDevice::signalUpdated(QString orient, QString state, QString face)
{
    Q_UNUSED(state)

#define UPDATE_ORIENTATION(_S, _C, _T, _F)  (_S[_S.length() -1] == _C ? _T: _F)

    switch(orient[0].toAscii()) {
    case 'l': physicalOrientation = UPDATE_ORIENTATION(orient, ')', "bottom", "top"); break;
    case 'p': physicalOrientation = UPDATE_ORIENTATION(orient, ')', "right", "left"); break;
    default:  physicalOrientation = UPDATE_ORIENTATION(face, 'p', "up", "down"); break;
    }

#undef UPDATE_ORIENTATION

    qDebug() << physicalOrientation;
    Q_EMIT valueChanged();
}

QString FOrientationDevice::getOrientation() const
{
    return orientation[0] == 'U' ? physicalOrientation: orientation;
}


void FOrientationDevice::setOrientation(const QString &new_orientation)
{

#define MCE_ACCELEROMETER_ENABLE_REQ   "req_accelerometer_enable"
#define MCE_ACCELEROMETER_DISABLE_REQ  "req_accelerometer_disable"

    if(started) {
        if (new_orientation == "Undefined") {
            // Enable accelerometer
            watcher = new QDBusPendingCallWatcher(proxy->asyncCall(MCE_ACCELEROMETER_ENABLE_REQ));
            QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(callback(QDBusPendingCallWatcher*)));
        }
        else {
            // Disable accelerometer
            proxy->call(QDBus::NoBlock, MCE_ACCELEROMETER_DISABLE_REQ);
        }
        orientation = new_orientation;
    }
}

void FOrientationDevice::callback(QDBusPendingCallWatcher *pcw)
{
    QDBusPendingReply<QString, QString, QString> reply = *pcw;
    if (!reply.isError()) {
        // Currently, a True value in reply indicated that keyboard is closed
        signalUpdated(reply.argumentAt<0>(), reply.argumentAt<1>(), reply.argumentAt<2>());
    }
    else {
        qWarning() << reply.error().message();
    }
    if (watcher == pcw) {
        watcher = 0;
    }
    pcw->deleteLater();
}

#include "moc_forientationdevice.cpp"

