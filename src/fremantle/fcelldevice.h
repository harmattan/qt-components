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

enum registrationStatus 
{
    NETWORK_REG_STATUS_HOME = 0x00,
    NETWORK_REG_STATUS_ROAM,
    NETWORK_REG_STATUS_ROAM_BLINK,
    NETWORK_REG_STATUS_NOSERV,
    NETWORK_REG_STATUS_NOSERV_SEARCHING,
    NETWORK_REG_STATUS_NOSERV_NOTSEARCHING,
    NETWORK_REG_STATUS_NOSERV_NOSIM,
    NETWORK_REG_STATUS_POWER_OFF = 0x08,
    NETWORK_REG_STATUS_NSPS,
    NETWORK_REG_STATUS_NSPS_NO_COVERAGE,
    NETWORK_REG_STATUS_NOSERV_SIM_REJECTED_BY_NW
};

class FCellDevice : public FDBusProxy
{
    Q_OBJECT

public:
    Q_PROPERTY(int signalStrength READ getSignalStrength NOTIFY signalStrengthChanged() FINAL)
    Q_PROPERTY(int status READ getStatus NOTIFY statusChanged() FINAL)
    Q_PROPERTY(bool offline READ isOffline NOTIFY offlineChanged() FINAL)
    Q_PROPERTY(QString provider READ getProvider NOTIFY providerChanged() FINAL)
    Q_PROPERTY(int radioMode READ getRadioMode NOTIFY radioModeChanged() FINAL)

Q_SIGNALS:
    void signalStrengthChanged();
    void statusChanged();
    void offlineChanged();
    void providerChanged();
    void radioModeChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FCellDevice(const QString& path, QObject *parent = 0);

public:
    int getSignalStrength();
    int getStatus();
    bool isOffline();
    QString getProvider() const;
    int getRadioMode();

private:
    int signalStrength;
    uchar status;
    bool offline;
    QString provider;
    int radioMode;

    FService *service;

private Q_SLOTS:
    void setSignalStrength();
    void setRegistrationStatus();
    void setProvider(uint operator_code, uint country_code);
    void setRadioMode();


    void onSignalStrengthChanged(uchar bars, uchar dbm);
    void onSignalStrengthReply(QDBusPendingCallWatcher* watcher);
    void onRegistrationStatusChanged(QDBusMessage msg);
    void onRegistrationStatusReply(QDBusPendingCallWatcher* watcher);
    void onProviderChanged(QString name);
    void onProviderReply(QDBusPendingCallWatcher *watcher);
    void onRadioModeChanged(uchar mode);
    void onRadioModeReply(QDBusPendingCallWatcher *watcher);

    void onServiceStateChanged();
};

#endif /* ! FCELLDEVICE_H */
