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

#include <QCoreApplication>
#include <QDate>
#include <QTime>
#include <QDir>

#include "mlocalthemedaemonclient.h"
#include "mlocalthemedaemonclient_p.h"

#ifdef HAVE_GCONF
# include "mimsettings.h"
#endif

#include <QDebug>

namespace {
const QString FALLBACK_THEME     = "base";
const unsigned int CACHE_VERSION = 1;
}

#define THEME_CACHE_DIR \
    MSystemDirectories::systemThemeCacheDirectory() + QDir::separator() + "QtComponents"

const QSettings *themeFile(const QString &theme)
{
    // Determine whether this is a m theme:
    // step 1: we need to have index.theme file
    const QString themeIndexFileName = MSystemDirectories::systemThemeDirectory() + QDir::separator() + theme + QDir::separator() + "index.theme";
    if (!QFile::exists(themeIndexFileName)) {
        return NULL;
    }
    // step 2: it needs to be a valid ini file
    const QSettings *themeIndexFile = new QSettings(themeIndexFileName, QSettings::IniFormat);
    if (themeIndexFile->status() != QSettings::NoError) {
        delete themeIndexFile;
        return NULL;
    }
    // step 3: we need to have X-MeeGoTouch-Metatheme group in index.theme
    if ((!themeIndexFile->childGroups().contains(QString("X-MeeGoTouch-Metatheme"))) ) {
        delete themeIndexFile;
        return NULL;
    }
    return themeIndexFile;
}

MLocalThemeDaemonClientPrivate::MLocalThemeDaemonClientPrivate() 
    : currentThemeName(""),
      themeInheritance(0),
      m_filenameHash(),
      m_imageDirNodes()
{
    m_imageDirNodes.append(ImageDirNode("icons" , QStringList() << ".svg" << ".png" << ".jpg"));
    m_imageDirNodes.append(ImageDirNode(QLatin1String("images") + QDir::separator() + QLatin1String("theme"), QStringList() << ".png" << ".jpg"));
    m_imageDirNodes.append(ImageDirNode(QLatin1String("images") + QDir::separator() + QLatin1String("backgrounds"), QStringList() << ".png" << ".jpg"));
}

MLocalThemeDaemonClientPrivate::~MLocalThemeDaemonClientPrivate()
{
}

bool MLocalThemeDaemonClientPrivate::activateTheme(const QString &newTheme)
{
    QString tmpTheme = newTheme;
    QDir dir(MSystemDirectories::systemThemeDirectory() + QDir::separator() + newTheme);

    if (!currentThemeName.isEmpty() && (currentThemeName == newTheme)) {
        return false;
    }
    if (!dir.exists()) {
        qDebug() << "Theme" << newTheme << "does not exist! Not changing theme";
        if (!currentTheme().isEmpty()) {
            return false;
        }
        tmpTheme = FALLBACK_THEME;
        qDebug() << "Fallback to theme Base";
    }

    // Change the theme
    //1: Try to load from cache
    if (activateThemeFromCache(tmpTheme)) {
        qDebug() << "Preloading theme" << tmpTheme << "from cache";
        currentThemeName = newTheme;
        return true;
    }
    // 2: find out the inheritance chain for the new theme
    QStringList newThemeInheritanceChain;

    while (true) {
        const QSettings *themeIndexFile = themeFile(tmpTheme);
        if (!themeIndexFile) {
            qDebug() << "Theme" << tmpTheme << "does not exist! Invalid inheritance";
            return false;
        }

        newThemeInheritanceChain.prepend(tmpTheme);
        QString parentTheme = themeIndexFile->value("X-MeeGoTouch-Metatheme/X-Inherits", "").toString();
        delete themeIndexFile;

        if (parentTheme.isEmpty()) {
            break;
        }
        tmpTheme = parentTheme;

        // check that there is no cyclic dependencies
        foreach(const QString & themeName, newThemeInheritanceChain) {
            if (tmpTheme == themeName) {
                qDebug() << "Cyclic dependency in theme:" << newTheme;
                return false;
            }
        }
    }
    themeInheritance = newThemeInheritanceChain;

    QString linkToCurrentTheme = THEME_CACHE_DIR + QDir::separator() + "default";
    QFile::remove(linkToCurrentTheme);
    QFile::link(MSystemDirectories::systemThemeDirectory() + QDir::separator() + themeInheritance.last(), linkToCurrentTheme);

    // include the path to theme inheritance chain
    for (int i = 0; i != themeInheritance.count(); ++i) {
        themeInheritance[i] = MSystemDirectories::systemThemeDirectory() + QDir::separator() + themeInheritance[i];
        qDebug() << "Looking for assets in" << themeInheritance[i];

        // Update Hashes
        themeInheritance[i] = themeInheritance[i] + QDir::separator() + "meegotouch" + QDir::separator();
        buildHash(themeInheritance[i] + "icons", QStringList() << "*.svg" << "*.png" << "*.jpg");
        buildHash(themeInheritance[i] + "images" + QDir::separator() + "theme", QStringList() << "*.png" << "*.jpg");
        buildHash(themeInheritance[i] + "images" + QDir::separator() + "backgrounds", QStringList() << "*.png" << "*.jpg");
    }
    saveThemeToBinaryCache(newTheme);
    currentThemeName = newTheme;
    return true;
}

bool MLocalThemeDaemonClientPrivate::activateThemeFromCache(const QString &theme)
{
    const QString cacheFileName = THEME_CACHE_DIR + QDir::separator() + theme + QDir::separator() + "theme.cache";


    if (QFile::exists(cacheFileName)) {
        QFile file(cacheFileName);
        if (file.open(QFile::ReadOnly)) {
            QDataStream stream(&file);
            uint version;
            stream >> version;
            if (version != CACHE_VERSION) {
                // will be replaced with up to date version
                file.close();
                return false;
            }

            uint timestamp;
            stream >> timestamp;
            if (timestamp != QFileInfo(cacheFileName).lastModified().toTime_t()) {
                // will be replaced with up to date version
                file.close();
                return false;
            }

            // Read cache contents
            stream >> m_filenameHash;
            file.close();
            return true;
        } else {
            qDebug() << "[MLogicalValuesPrivate]" << "Failed to load values from cache" << cacheFileName;
        }
    }
    return false;
}

bool MLocalThemeDaemonClientPrivate::saveThemeToBinaryCache(const QString &theme)
{
    const QString cacheFileName = THEME_CACHE_DIR + QDir::separator() + theme + QDir::separator() + "theme.cache";

    QFile file(cacheFileName);
    if (!file.open(QFile::WriteOnly)) {
        //Maybe it failed because the directory doesn't exist
        QDir().mkpath(QFileInfo(cacheFileName).absolutePath());
        if (!file.open(QFile::WriteOnly)) {
            qDebug() << "[MLogicalValuesPrivate]" << "Failed to save cache file for" << theme << "to" << cacheFileName;
            return false;
        }
    }

    QDataStream stream(&file);
    stream << CACHE_VERSION;
    stream << QFileInfo(cacheFileName).lastModified().toTime_t();
    //QDateTime(QDate::currentDate(), QTime::currentTime(), Qt::LocalTime).toTime_t();
    stream << m_filenameHash;
    qDebug() <<  QFileInfo(cacheFileName).lastModified().toTime_t();
    file.close();
    return true;
}

QImage MLocalThemeDaemonClientPrivate::readImage(const QString &id) const
{
    foreach (const ImageDirNode &imageDirNode, m_imageDirNodes) {
        foreach (const QString &suffix, imageDirNode.suffixList) {

            QString imageFilePathString = m_filenameHash.value(id + suffix);
            if (!imageFilePathString.isNull()) {
                imageFilePathString.append(QDir::separator() + id + suffix);
            }

            QImage image(imageFilePathString);
            if (!image.isNull()) {
                return image;
            }
        }
    }

    return QImage();
}

void MLocalThemeDaemonClientPrivate::buildHash(const QDir& rootDir, const QStringList& nameFilter)
{
    QDir rDir = rootDir;
    rDir.setNameFilters(nameFilter);
    QStringList files = rDir.entryList(QDir::Files);
    foreach (const QString filename, files) {
        m_filenameHash.insert(filename, rootDir.absolutePath());
    }

    QStringList dirList = rootDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(const QString nextDirString, dirList){
        QDir nextDir(rootDir.absolutePath() + QDir::separator() + nextDirString);
        buildHash(nextDir, nameFilter);
    }
}

MLocalThemeDaemonClientPrivate::ImageDirNode::ImageDirNode(const QString &directory, const QStringList &suffixList) :
    directory(directory),
    suffixList(suffixList)
{
}

MLocalThemeDaemonClient::MLocalThemeDaemonClient(QObject *parent) :
    MAbstractThemeDaemonClient(parent),
    d_ptr(new MLocalThemeDaemonClientPrivate),
    m_pixmapCache()
{
    Q_D(MLocalThemeDaemonClient);
    d->q_ptr = this;

#ifdef HAVE_GCONF
    d->activateTheme(DEFAULT_THEME);
#else
    d->activateTheme(DEFAULT_THEME);
#endif

}

MLocalThemeDaemonClient::~MLocalThemeDaemonClient()
{
    delete d_ptr;
}

QPixmap MLocalThemeDaemonClient::requestPixmap(const QString &id, const QSize &requestedSize)
{
    QPixmap pixmap;
    QSize size = requestedSize;

    Q_D(MLocalThemeDaemonClient);

    if (size.width() < 1) {
        size.rwidth() = 0;
    }
    if (size.height() < 1) {
        size.rheight() = 0;
    }

    const PixmapIdentifier pixmapId(id, size);
    pixmap = m_pixmapCache.value(pixmapId);
    if (pixmap.isNull()) {
        // The pixmap is not cached yet. Decode the image and
        // store it into the cache as pixmap.
        const QImage image = d->readImage(id);
        if (!image.isNull()) {
            pixmap = QPixmap::fromImage(image);
            if (requestedSize.isValid() && (pixmap.size() != requestedSize)) {
                pixmap = pixmap.scaled(requestedSize);
            }

            m_pixmapCache.insert(pixmapId, pixmap);
        }
    }
    return pixmap;
}

QString MLocalThemeDaemonClient::findFileRecursively(const QDir& rootDir, const QString& name)
{
    QStringList files = rootDir.entryList(QStringList(name));
    if(files.length() > 0)
        return rootDir.filePath(files[0]);

    QStringList dirList = rootDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(const QString nextDirString, dirList){
        QDir nextDir(rootDir.absolutePath() + QDir::separator() + nextDirString);
        QString nextFile = findFileRecursively(nextDir, name);
        if(!nextFile.isNull())
            return nextFile;
    }

    return QString();
}

MLocalThemeDaemonClient::PixmapIdentifier::PixmapIdentifier() :
    imageId(), size()
{
}

MLocalThemeDaemonClient::PixmapIdentifier::PixmapIdentifier(const QString &imageId, const QSize &size) :
    imageId(imageId), size(size)
{
}

bool MLocalThemeDaemonClient::PixmapIdentifier::operator==(const PixmapIdentifier &other) const
{
    return imageId == other.imageId && size == other.size;
}

bool MLocalThemeDaemonClient::PixmapIdentifier::operator!=(const PixmapIdentifier &other) const
{
    return imageId != other.imageId || size != other.size;
}

uint qHash(const MLocalThemeDaemonClient::PixmapIdentifier &id)
{
    using ::qHash;

    const uint idHash     = qHash(id.imageId);
    const uint widthHash  = qHash(id.size.width());
    const uint heightHash = qHash(id.size.height());

    // Twiddle the bits a little, taking a cue from Qt's own qHash() overloads
    return idHash ^ (widthHash << 8) ^ (widthHash >> 24) ^ (heightHash << 24) ^ (heightHash >> 8);
}
