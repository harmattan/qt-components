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

#ifndef MLOCALTHEMEDAEMONCLIENT_P_H
#define MLOCALTHEMEDAEMONCLIENT_P_H

#ifdef HAVE_GCONF
# include "mimsettings.h"
#endif

class MLocalThemeDaemonClientPrivate
{
public:

    MLocalThemeDaemonClientPrivate(QObject *parent);
    virtual ~MLocalThemeDaemonClientPrivate();

#ifdef HAVE_GCONF
    MImSettings m_currentThemeConf;
#endif

    bool activateTheme(const QString& new_theme);
    /**
     * Reads the image \a id from the available directories specified
     * by m_imageDirNodes.
     */
    QImage readImage(const QString &id) const;

protected:

    MLocalThemeDaemonClient * q_ptr;

private:

    QString currentThemeName;
    QStringList themeInheritance;

    struct ImageDirNode
    {
        ImageDirNode(const QString &directory, const QStringList &suffixList);
        QString directory;
        QStringList suffixList;
    };

    QHash<QString, QString> m_filenameHash;
    QList<ImageDirNode> m_imageDirNodes;

    /**
     * Load a theme from cache
     */
    bool activateThemeFromCache(const QString &theme);

    /**
     * Save current theme to binary cache
     */
    bool saveThemeToBinaryCache(const QString &theme);

    /**
     * Get a hash recursively for each image available on rootDir
     */
    void buildHash(const QDir& rootDir, const QStringList& nameFilter);

    Q_DECLARE_PUBLIC(MLocalThemeDaemonClient)
};

#endif // MLOCALTHEMEDAEMONCLIENT_P_H
