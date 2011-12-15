/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project.
**
** $QT_BEGIN_LICENSE:BMD$
** You may use this file under the terms of the BMD license as follows:
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

#include "mdeclarative.h"
#include "mbatteryinfo.h"
#include "mcellinfo.h"
#include "mnetworkinfo.h"

#include <QCoreApplication>
#include <QTime>
#include <QTimer>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QPixmapCache>

#include <QDebug>

static const int MINUTE_MS = 60*1000;

class MDeclarativePrivate
{
public:
    QTimer timer;
    MBatteryInfo batteryInfo;
    MCellInfo cellInfo;
    MNetworkInfo networkInfo;

    MDeclarativePrivate() {}
};

MDeclarative::MDeclarative(QObject *parent) :
    QObject(parent),
    d_ptr(new MDeclarativePrivate)
{
    Q_D(MDeclarative);
    d->timer.start(MINUTE_MS);
    connect(&d->timer, SIGNAL(timeout()), this, SIGNAL(currentTimeChanged()));

    QCoreApplication *application = QCoreApplication::instance();
    if (application)
        application->installEventFilter(this);
}

MDeclarative::~MDeclarative()
{
    d_ptr->timer.stop();
    delete d_ptr;
}

QString MDeclarative::currentTime()
{
    return QTime::currentTime().toString(QLatin1String("h:mm"));
}

MBatteryInfo *MDeclarative::batteryInfo()
{
    Q_D(MDeclarative);
    return &d->batteryInfo;
}

MCellInfo *MDeclarative::cellInfo()
{
    Q_D(MDeclarative);
    return &d->cellInfo;
}

MNetworkInfo *MDeclarative::networkInfo()
{
    Q_D(MDeclarative);
    return &d->networkInfo;
}

void MDeclarative::privateClearIconCaches()
{
    QPixmapCache::clear();
}

void MDeclarative::privateClearComponentCache()
{
    QDeclarativeContext *context = qobject_cast<QDeclarativeContext*>(this->parent());
    if (context) {
        context->engine()->clearComponentCache();
    }
}

bool MDeclarative::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(MDeclarative);

    if (obj == QCoreApplication::instance()) {
        if (event->type() == QEvent::ApplicationActivate) {
	    d->batteryInfo.start();
	    d->cellInfo.start();
            d->timer.start(MINUTE_MS);
            emit currentTimeChanged();
	    
        } else if (event->type() == QEvent::ApplicationDeactivate) {
            d->timer.stop();
	    d->cellInfo.stop();
	    d->batteryInfo.stop();
        }
    }
    return QObject::eventFilter(obj, event);
}
