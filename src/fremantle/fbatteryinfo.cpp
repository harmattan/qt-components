/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "fbmedevice.h"
#include "mbatteryinfo.h"

class MBatteryInfoPrivate
{
    Q_DECLARE_PUBLIC(MBatteryInfo)

public:
    MBatteryInfoPrivate(MBatteryInfo *qq) : bme(BME_DEVICE), q_ptr(qq) {}
    FBMEDevice bme;

private:
    MBatteryInfo *q_ptr;
};

MBatteryInfo::MBatteryInfo(QObject *parent) :
    QObject(parent), d_ptr(new MBatteryInfoPrivate(this))
{
    Q_D(MBatteryInfo);

    QObject::connect(&d->bme, SIGNAL(levelChanged()), this, SIGNAL(batteryLevelChanged()));
    QObject::connect(&d->bme, SIGNAL(chargingChanged()), this, SIGNAL(chargingChanged()));

    //Start to listen to events
    d->bme.start();
}

MBatteryInfo::~MBatteryInfo()
{
    delete d_ptr;
}

int MBatteryInfo::batteryLevel() const
{
    const Q_D(MBatteryInfo);
    return d->bme.batteryLevel();
}

bool MBatteryInfo::charging() const
{
    const Q_D(MBatteryInfo);
    return d->bme.isCharging();
}

bool MBatteryInfo::powerSaveModeEnabled() const
{
    return false;
}

#include "moc_mbatteryinfo.cpp"
