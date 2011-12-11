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

#ifndef FCELLDEVICE_H
#define FCELLDEVICE_H

#include "fphoneservice.h"

#define CELL_DEVICE   "/com/nokia/phone/net"
#define CELL_BUS      QDBusConnection::systemBus()

class FCellDevice : public FDBusProxy
{
    Q_OBJECT

public:
    Q_PROPERTY(int signalStrength READ getSignalStrength NOTIFY signalStrengthChanged() FINAL)

Q_SIGNALS:
    void signalStrengthChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FCellDevice(const QString& path, QObject *parent = 0);

public:
    int getSignalStrength();

private:
    int signalStrength;
    FService *service;

private Q_SLOTS:
    void setSignalStrength();

    void onSignalStrengthChanged(int bars, int dbm);
    void onSignalStrengthReply(QDBusPendingCallWatcher* watcher);
    void onServiceStateChanged();
};

#endif /* ! FCELLDEVICE_H */
