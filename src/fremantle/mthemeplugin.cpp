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

#include <QApplication>
#include <QPointer>

#include "mthemeplugin.h"
#include "mthemedaemon.h"

#ifdef HAVE_GCONF
# include "mimsettings.h"
#endif


MThemePlugin::MThemePlugin(QDeclarativeItem *parent)
    : QObject(parent),
      m_inverted(false),
      m_name("")
    #ifdef HAVE_GCONF
    ,m_nameConf(new MImSettings ("/qtcomponents/themes/current"))
    #endif
{
    // Load default system theme
    MThemeDaemon::instance()->requestTheme(name());

#ifdef HAVE_GCONF
    QObject::connect(m_nameConf, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
#endif
}

MThemePlugin::~MThemePlugin()
{
    delete m_nameConf;
}

bool MThemePlugin::isInverted() const
{
    return m_inverted;
}

void MThemePlugin::setInverted(bool inverted)
{
    if (m_inverted != inverted) {
        m_inverted = inverted;
        emit invertedChanged();
    }
}

bool MThemePlugin::setName(const QString &newTheme)
{
    // If newTheme is empty, fallback to system theme
    QString tmpTheme = newTheme;
    if (tmpTheme.isEmpty()) {
#ifdef HAVE_GCONF
        tmpTheme = m_nameConf->value().toString();
#else
        tmpTheme = QString(DEFAULT_THEME);
#endif
    }
    if (MThemeDaemon::instance()->requestTheme(tmpTheme)) {
        // Notify about theme change
        qDebug() << "Theme changed to " << newTheme;
        m_name = newTheme;
        emit nameChanged();

        // This is a hack. Force components to update its images
        emit invertedChanged();
        return true;
    }
    // Theme hasn't changed
    return false;
}

QString MThemePlugin::name() const
{
    if (! m_name.isEmpty()) {
        return m_name;
    }
#ifdef HAVE_GCONF
    return m_nameConf->value().toString();
#else
    return QString(DEFAULT_THEME);
#endif
}

void MThemePlugin::onValueChanged()
{
    if (m_name.isEmpty()){
        setName("");
    }
}

#include "moc_mthemeplugin.cpp"
