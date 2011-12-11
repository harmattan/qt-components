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

    //Start to listen to events
    d->cell.start();
}

MCellInfo::~MCellInfo()
{
    delete d_ptr;
}


bool MCellInfo::active() const
{
    return false;
}

bool MCellInfo::offline() const
{
    return true;
}

QString MCellInfo::status() const
{
    return "no-gsm-connection";
}

QString MCellInfo::networkOperator() const
{
    return "";
}

QString MCellInfo::radioMode() const
{
    return "gsm";
}

int MCellInfo::signalStrength() const
{
    const Q_D(MCellInfo);
    return d->cell.signalStrength();
}


#include "moc_mcellinfo.cpp"
