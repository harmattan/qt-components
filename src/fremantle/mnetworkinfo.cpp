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

#include <QTimer>
#include <QNetworkSession>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>

#include "mnetworkinfo.h"
#include "mnetworkinfo_p.h"


// dummy private class
MNetworkInfoPrivate::MNetworkInfoPrivate(MNetworkInfo *qq) : 
    q_ptr(qq),
    current(0)
{
    QObject::connect(
                &manager,
                SIGNAL(configurationAdded(QNetworkConfiguration)),
                this, SLOT(onConfigurationAdded(QNetworkConfiguration)));
    QObject::connect(
                &manager,
                SIGNAL(configurationRemoved(QNetworkConfiguration)),
                this, SLOT(onConfigurationRemoved(QNetworkConfiguration)));

    QTimer::singleShot(0, this, SLOT(updateConfigurations()));
}

void MNetworkInfoPrivate::updateConfigurations()
{
    Q_Q(MNetworkInfo);
    QList<QNetworkConfiguration> allConfigurations = manager.allConfigurations();

    while (!allConfigurations.isEmpty()) {
        QNetworkConfiguration conf = allConfigurations.takeFirst();
        onConfigurationAdded(conf);

        if (conf.state() == QNetworkConfiguration::Active) {
            current = sessions[manager.defaultConfiguration().identifier()];
            Q_EMIT(q->statusChanged());
            Q_EMIT(q->bearerChanged());
        }
    }
}

void MNetworkInfoPrivate::onConfigurationAdded(QNetworkConfiguration conf)
{
    QNetworkSession *session;

    session = new QNetworkSession(conf, this);
    sessions.insert(conf.identifier(), session);

    QObject::connect(
                session,
                SIGNAL(stateChanged(QNetworkSession::State)),
                this, SLOT(onStateChanged(QNetworkSession::State)));
}

void MNetworkInfoPrivate::onConfigurationRemoved(QNetworkConfiguration conf)
{
    sessions.remove(conf.identifier());
}

void MNetworkInfoPrivate::onStateChanged(QNetworkSession::State state)
{
    Q_Q(MNetworkInfo);
    QNetworkSession *session;

    session = qobject_cast<QNetworkSession *>(QObject::sender());

    switch (state) {
    case QNetworkSession::Connecting:
        current = session;
        Q_EMIT(q->statusChanged());
        Q_EMIT(q->bearerChanged());
        break;

    case QNetworkSession::Connected:
        Q_EMIT(q->statusChanged());
        if (current != session) {
            current = session;
            Q_EMIT(q->bearerChanged());
        }
        break;

    case QNetworkSession::Closing:
        if (current == session) {
            Q_EMIT(q->statusChanged());
        }
        break;

    case QNetworkSession::Disconnected:
        if (current == session) {
            current = NULL;
            Q_EMIT(q->statusChanged());
            Q_EMIT(q->bearerChanged());
        }
        break;

    case QNetworkSession::Roaming:
        break;
    case QNetworkSession::Invalid:
    case QNetworkSession::NotAvailable:
    default:
        break;
    }
}

MNetworkInfo::MNetworkInfo(QObject *parent) :
    QObject(parent), d_ptr(new MNetworkInfoPrivate(this))
{
}

MNetworkInfo::~MNetworkInfo()
{
    delete d_ptr;
}

// Properties
MNetworkInfo::Status MNetworkInfo::getStatus()
{
    Q_D(MNetworkInfo);

#define NETWORK_STATE(_c, _def) _c ? _c->state() : _def;

    QNetworkSession::State state = QNetworkSession::NotAvailable;
    state = NETWORK_STATE(d->current, state)

            switch(state) {
            case QNetworkSession::Connecting:
            return Connecting;
    case QNetworkSession::Connected:
        return Connected;
    case QNetworkSession::Closing:
        return Closing;
    case QNetworkSession::Disconnected:
    case QNetworkSession::Roaming:
    case QNetworkSession::NotAvailable:
    case QNetworkSession::Invalid:
    default:
        return Disconnected;
}

#undef NETWORK_STATE
}

QString MNetworkInfo::getBearer()
{
    Q_D(MNetworkInfo);

#define NETWORK_BEARER(_c, _def) _c ? _c->configuration().bearerType(): _def;

    QNetworkConfiguration::BearerType bearer = QNetworkConfiguration::BearerUnknown;
    bearer = NETWORK_BEARER(d->current, bearer)

            switch(bearer) {
            case QNetworkConfiguration::BearerWLAN:
            return "wlan";
    case QNetworkConfiguration::Bearer2G:
        return "25";
    case QNetworkConfiguration::BearerWCDMA:
        return "3g";
    case QNetworkConfiguration::BearerHSPA:
        return "35";
    case QNetworkConfiguration::BearerBluetooth:
        return "bluetooth";
    case QNetworkConfiguration::BearerCDMA2000:
    case QNetworkConfiguration::BearerWiMAX:
    case QNetworkConfiguration::BearerEthernet:
    case QNetworkConfiguration::BearerUnknown:
    default:
        return "unknown";
}

#undef NETWORK_BEARER
}

#include "moc_mnetworkinfo.cpp"
