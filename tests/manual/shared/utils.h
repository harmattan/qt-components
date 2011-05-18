/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project on Qt Labs.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions contained
** in the Technology Preview License Agreement accompanying this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
****************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QStringList>

class FileAccess : public QObject
{
    Q_OBJECT

public:
    explicit FileAccess(QObject *parent = 0);

    Q_INVOKABLE QStringList qmlFileNames(const QString &path) const;
    Q_INVOKABLE QStringList qmlFilePaths(const QString &path) const;
    Q_INVOKABLE QStringList qmlPaths() const;
};

class Settings : public QObject
{
    Q_OBJECT
    Q_ENUMS(IndicatorIds)
public:

    enum IndicatorIds {
        IndicatorEMail = 2,
        IndicatorSecuredConnection = 6,
        IndicatorBluetooth = 12,
        IndicatorUsb = 28
    };

    explicit Settings(QObject *parent = 0);
    ~Settings();

    Q_INVOKABLE void setOrientation(int orientation);
    Q_INVOKABLE int orientation() const;
    Q_INVOKABLE void setIndicatorState(int indicatorId, bool on) const;
};

#endif // UTILS_H
