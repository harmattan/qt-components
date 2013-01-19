/***************************************************************************
**
** Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mlogicalvalues.h"
#include "mlogicalvalues_p.h"

#include "msystemdirectories.h"

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextCodec>
#include <QDateTime>
#include <QCoreApplication>

#include <QVariant>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QUrl>

#include <QDebug>

namespace {
const unsigned int CACHE_VERSION = 1;
}

#define THEME_CACHE_DIR(_X)			\
    MSystemDirectories::systemThemeCacheDirectory() + QDir::separator() + "QtComponents" + QDir::separator() + _X + QDir::separator() + "constants" + QDir::separator()

#define THEME_INDEX_DIR(_X)			\
    MSystemDirectories::systemThemeDirectory() + QDir::separator() + _X + QDir::separator() + QLatin1String("meegotouch")

MLogicalValues::MLogicalValues() :
    d_ptr(new MLogicalValuesPrivate)
{
}

MLogicalValues::~MLogicalValues()
{
    delete d_ptr;
}

bool MLogicalValuesPrivate::parse(const QFileInfo &fileInfo, Groups &groups)
{
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QByteArray group = "General";
    groups.insert("General", Values());

    while (!file.atEnd()) {
        QByteArray line = file.readLine().trimmed();
        // skip comments
        if (line.startsWith("[")) {
            // parse group header
            int index = line.indexOf("]", 1);
            if (index == -1) {
                qWarning() << "[MLogicalValues]" << "Error occurred when parsing .ini file:" << line;
                file.close();
                return false;
            }
            // this will be the currently active group
            group = line.mid(1, index - 1);
        } else {

            // key/value pair
            QByteArray key, value;
            QByteArray *target = &key;

            // stores the last 'good' character
            int truncation = 0;
            // go through whole line
            for (int i = 0; i < line.length(); i++) {
                QChar character = line.at(i);
                if (character == ';') {
                    break;
                } else if (character == '=') {
                    // remove trailing whitespaces
                    target->truncate(truncation);
                    // start to parse value
                    target = &value;
                    truncation = 0;
                } else {
                    if (target->isEmpty() && character.isSpace()) {
                        // do not add whitespaces at the beginning
                    } else {
                        (*target) += character;
                        if (!character.isSpace())
                            truncation = target->length();
                    }
                }
            }
            // remove trailing whitespaces
            target->truncate(truncation);

            // consistency check
            if (!line.startsWith(';') && line.length() > 0) {
                if (key.isEmpty() || value.isEmpty()) {
                    qWarning() << "[MLogicalValues]" << "Error occurred when parsing .ini file:" << line;
                    file.close();
                    return false;
                }
                // store
                Values &values = groups[group];

                if (!values.contains(key)) {
                    QVariant v(value.constData());
                    QString s = v.toString();
                    int pos;

                    if (value.length()) {
                        if (QValidator::Acceptable == QIntValidator().validate(s, pos)) {
                            v.convert(QVariant::Int);
                        }
                        else if (QValidator::Acceptable == QDoubleValidator().validate(s, pos)) {
                            v.convert(QVariant::Double);
                        }
                        else {
                            QUrl url(s);
                            if (url.isValid() && (url.scheme() != "")) {
                                v.convert(QVariant::Url);
                            }
                        }
                    }
                    values.insert(key, v);
                }
            }
        }
    }

    file.close();
    return true;
}

bool MLogicalValuesPrivate::loadFromBinaryCache(const QString &theme, const QFileInfo &fileInfo, Groups &groups) {

    const QString cacheFileName = THEME_CACHE_DIR(theme) + fileInfo.fileName();

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
            if (timestamp != fileInfo.lastModified().toTime_t()) {
                // will be replaced with up to date version
                file.close();
                return false;
            }
            stream >> groups;
            file.close();
            return true;
        } else {
            qDebug() << "[MLogicalValuesPrivate]" << "Failed to load values from cache" << cacheFileName;
        }
    }

    return false;
}

bool MLogicalValuesPrivate::saveToBinaryCache(const QString &theme, const QFileInfo &fileInfo, const Groups &groups) const {
    const QString cacheFileName = THEME_CACHE_DIR(theme) + fileInfo.fileName();

    QFile file(cacheFileName);
    if (!file.open(QFile::WriteOnly)) {
        //Maybe it failed because the directory doesn't exist
        QDir().mkpath(QFileInfo(cacheFileName).absolutePath());
        if (!file.open(QFile::WriteOnly)) {
            qDebug() << "[MLogicalValuesPrivate]" << "Failed to save cache file for" << fileInfo.fileName() << "to" << cacheFileName;
            return false;
        }
    }

    QDataStream stream(&file);
    stream << CACHE_VERSION;
    stream << fileInfo.lastModified().toTime_t();
    stream << groups;

    file.close();
    return true;
}

void MLogicalValuesPrivate::mergeGroups(const Groups &groups)
{
    Groups::const_iterator i = groups.constBegin();
    while (i != groups.constEnd()) {
        Values &values = data[i.key()];
        Values::const_iterator j = i.value().constBegin();
        while (j != i.value().constEnd()) {
            values.insert(j.key(), j.value());
            ++j;
        }
        ++i;
    }
}

bool MLogicalValues::append(const QString &fileName, const QString &theme)
{
    Q_D(MLogicalValues);

    // make sure that the file exists
    if (!QFile(fileName).exists())
        return false;

    Groups groups;
    QFileInfo fileInfo(fileName);

    if (theme.isEmpty() || !d->loadFromBinaryCache(theme, fileInfo, groups)) {
        if (!d->parse(fileInfo, groups)) {
            return false;
        }
        d->saveToBinaryCache(theme, fileInfo, groups);
    }

    d->timestamps << fileInfo.lastModified().toTime_t();
    d->mergeGroups(groups);

    return true;
}

void MLogicalValues::load(const QStringList &themeInheritanceChain, const QString &locale)
{
    Q_D(MLogicalValues);

    d->data.clear();
    d->timestamps.clear();

    //TODO: Handle locales. Keep in mind the case when a third party add
    //a constants file only on meegotuch and not for every locale
    Q_UNUSED (locale);

    foreach(QString theme, themeInheritanceChain) {
        QDir dir(THEME_INDEX_DIR(theme));
        dir.setNameFilters(QStringList("*.ini"));
        foreach (QString file, dir.entryList(QDir::Files)) {
            append(dir.canonicalPath() + QDir::separator() + file, theme);
        }
    }
}

bool MLogicalValues::findKey(const QByteArray &key, QByteArray &group, QVariant &value) const
{
    Q_D(const MLogicalValues);

    // search from every group
    for (Groups::const_iterator iterator = d->data.begin(); iterator != d->data.end(); iterator++) {
        // get values from this group
        const Values &values = iterator.value();

        // check if this group contains the key
        if (values.contains(key)) {
            group = iterator.key();
            value = values.value(key);
            return true;
        }
    }

    return false;
}

bool MLogicalValues::value(const QByteArray &group, const QByteArray &key, QVariant &value) const
{
    Q_D(const MLogicalValues);
    if (!d->data.contains(group)) {
        qWarning() << "[MLogicalValues]" << "No such group:" << group;
        return false;
    }

    const Values &values = d->data[group];

    if (!values.contains(key)) {
        qWarning() << "[MLogicalValues]" << "No such key:" << group << '/' << key;
        return false;
    }

    value = values.value(key);
    return true;
}

QList<QByteArray> MLogicalValues::groups() const
{
    QList<QByteArray> keys;
    Q_D(const MLogicalValues);

    // search from every group
    for (Groups::const_iterator iterator = d->data.begin(); iterator != d->data.end(); iterator++) {
        keys << iterator.key();
    }
    return keys;
}

Groups MLogicalValues::data() const
{
    Q_D(const MLogicalValues);
    return d->data;
}

QHash<QByteArray, QVariant> MLogicalValues::values(const QByteArray &group) const
{
    const QHash<QByteArray, QVariant> empty;
    Q_D(const MLogicalValues);

    if (!d->data.contains(group)) {
        qWarning() << "[MLogicalValues]" << "No such group:" << group;
        return empty;
    }
    return d->data[group];
}

QList<uint> MLogicalValues::timestamps() const
{
    Q_D(const MLogicalValues);
    return d->timestamps;
}

