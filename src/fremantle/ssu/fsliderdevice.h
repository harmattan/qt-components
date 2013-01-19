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

#ifndef FSLIDERDEVICE_H
#define FSLIDERDEVICE_H

#include "fhalservice.h"

#define SLIDER_DEVICE "/org/freedesktop/Hal/devices/platform_slide"
#define SLIDER_BUS    QDBusConnection::systemBus()

class FSliderDevice : public FDBusProxy
{
    Q_OBJECT

public:
    Q_PROPERTY(bool open READ isOpen NOTIFY valueChanged() FINAL)

Q_SIGNALS:
    void valueChanged();

public:
    virtual void start (QObject *requestor = 0);
    virtual void stop  (QObject *requestor = 0);

public:
    explicit FSliderDevice(const QString& path, QObject *parent = 0);

public:
    bool isOpen() const;

private:
    bool open;
    FService *hal;

private Q_SLOTS:
    void onHALStateChanged();
    void callback(QDBusPendingCallWatcher* watcher);
    void updated();
};

#endif /* ! FSLIDERDEVICE_H */
