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

#include "fcelldevice.h"
#include "mcellinfo.h"

// dummy private class
class MCellInfoPrivate
{
    Q_DECLARE_PUBLIC(MCellInfo)

public:
    MCellInfoPrivate(MCellInfo *qq) : cell(CELL_DEVICE), q_ptr(qq) {}
    FCellDevice cell;

private:
    MCellInfo *q_ptr;
};


MCellInfo::MCellInfo(QObject *parent) :
    QObject(parent), d_ptr(new MCellInfoPrivate(this))
{
    Q_D(MBatteryInfo);

    QObject::connect(&d->cell, SIGNAL(signalStrengthChanged()), this, SIGNAL(signalStrengthChanged()));
    QObject::connect(&d->cell, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));
    QObject::connect(&d->cell, SIGNAL(providerChanged()), this, SIGNAL(providerChanged()));
    QObject::connect(&d->cell, SIGNAL(radioModeChanged()), this, SIGNAL(radioModeChanged()));

    //Start to listen to events
    d->cell.start();
}

MCellInfo::~MCellInfo()
{
    delete d_ptr;
}


QString MCellInfo::getStatus() const
{
    return d->cell.getStatus();
}

QString MCellInfo::getProvider() const
{
    return d->cell.getProvider();
}

QString MCellInfo::getRadioMode() const
{
    const Q_D(MCellInfo);

    switch(d->cell.getStatus()) {

    case NETWORK_REG_STATUS_HOME:
    case NETWORK_REG_STATUS_ROAM:
    case NETWORK_REG_STATUS_ROAM_BLINK:

#define GPRS (0x01 | 0x04)
#define HSPA (0x08 | 0x10)

        if (d->cell.getRadioMode() == 1)
            return d->cell.getServices() & GPRS ? "25g" : "gsm";
        if (d->cell.getRadioMode() == 2)
            return d->cell.getServices() & HSPA ? "35g" : "3g";

#undef GPRS
#undef HSPA

        break;

    case NETWORK_REG_STATUS_NOSERV:
    case NETWORK_REG_STATUS_SEARCHING:
    case NETWORK_REG_STATUS_NOTSEARCHING:
        return "no-gsm-connection";

    case NETWORK_REG_STATUS_NODIM:
        return "no-simcard";

    case NETWORK_REG_STATUS_POWEROFF:
        return "offline";
    }
    return "unknown";
}

int MCellInfo::getSignalStrength() const
{
    const Q_D(MCellInfo);
    return d->cell.signalStrength();
}


#include "moc_mcellinfo.cpp"
