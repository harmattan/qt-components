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

#ifndef FORIENTATIONDEVICE_H
#define FORIENTATIONDEVICE_H

#include "fmceservice.h"

#define ORIENT_DEVICE   "/com/nokia/mce/device"
#define ORIENT_SIGNAL   "/com/nokia/mce/signal"
#define ORIENTATION_BUS QDBusConnection::systemBus()

class FOrientationDevice : public FDBusProxy
{
    Q_OBJECT

public:
    Q_PROPERTY(QString orientation READ getOrientation WRITE setOrientation NOTIFY valueChanged() FINAL)

Q_SIGNALS:
    void valueChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FOrientationDevice(const QString& path, const QString &signal_path, QObject *parent = 0);

public:
    QString getOrientation() const;
    void    setOrientation(const QString& orientation);

private:
    QString orientation;
    QString physicalOrientation;
    FMCEService *mce;

private Q_SLOTS:
    void onMCEStateChanged();
    void callback(QDBusPendingCallWatcher* watcher);
    void signalUpdated(QString orien, QString stand, QString face);
};

#endif /* ! FORIENTATIONDEVICE_H */
