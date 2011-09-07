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

#include "mthemedaemon.h"

#include <themedaemon/mlocalthemedaemonclient.h>
#ifndef FORCE_LOCAL_THEME
# include <themedaemon/mremotethemedaemonclient.h>
#endif

static MThemeDaemon *self = 0;

MThemeDaemon* MThemeDaemon::instance()
{
    if (!self) {
        self = new MThemeDaemon();
    }
    return self;
}

MThemeDaemon::MThemeDaemon(QObject *parent)
    : MAbstractThemeDaemonClient(parent),
      m_themeDaemonClient(0)
{
#ifndef FORCE_LOCAL_THEME

    bool QuseRemoteThemeDaemon = qgetenv("M_FORCE_LOCAL_THEME").isEmpty();
#if defined Q_WS_MAC || defined Q_WS_WIN32 || defined FORCE_LOCAL_THEME
    useRemoteThemeDaemon = false;
#endif
    MRemoteThemeDaemonClient *remoteThemeDaemonClient = 0;
    if (useRemoteThemeDaemon)
        remoteThemeDaemonClient = new MRemoteThemeDaemonClient();

    if (remoteThemeDaemonClient && remoteThemeDaemonClient->isConnected()) {
        m_themeDaemonClient = remoteThemeDaemonClient;
    } else {
        if (remoteThemeDaemonClient)
            delete remoteThemeDaemonClient;
#else
    {
#endif
        m_themeDaemonClient = new MLocalThemeDaemonClient();
    }
}

MThemeDaemon::~MThemeDaemon()
{
    delete m_themeDaemonClient;
}

bool MThemeDaemon::requestTheme(const QString &newTheme)
{
    return m_themeDaemonClient->requestTheme(newTheme);
}

bool MThemeDaemon::requestValues(QDeclarativePropertyMap *map, QList<ThemeProperty> *updated)
{
    return m_themeDaemonClient->requestValues(map, updated);
}

QPixmap MThemeDaemon::requestPixmap(const QString &id, const QSize &requestedSize)
{
    return m_themeDaemonClient->requestPixmap(id, requestedSize);
}

#include "moc_mthemedaemon.cpp"
