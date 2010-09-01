/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mdeclarativeicon.h"

#include "mthemebridge.h"

#include <QPixmap>
#include <QPainter>
#include <MWidgetStyle>
#include <mtheme.h>

MDeclarativeIcon::MDeclarativeIcon(QDeclarativeItem *parent) :
    QDeclarativeItem(parent), m_icon(0), m_pendingPixmap(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    MThemeBridge::instance()->registerIcon(this);
}

MDeclarativeIcon::~MDeclarativeIcon()
{
    MThemeBridge::instance()->unregisterIcon(this);
    setIconId(QLatin1String(""));
}

QString MDeclarativeIcon::iconId() const
{
    return m_iconId;
}

void MDeclarativeIcon::setIconId(const QString &iconId)
{
    if (m_iconId == iconId)
        return;

    m_iconId = iconId;
    refreshPixmap();
    emit iconIdChanged(m_iconId);
}

void MDeclarativeIcon::refreshPixmap()
{
    if (m_icon) {
        MTheme::instance()->releasePixmap(m_icon);
        m_icon = 0;
    }

    if (!m_iconId.isEmpty()) {
        // Request the pixmap represented by the given iconID
        // XXX The second QSize(0, 0) argument is the size we want the returned pixmap
        // to be. However, if we specify that, we do not have a way of knowing whether
        // that pixmap is yet to be loaded (size -1, -1) or actually ready.
        // This causes the blur effect in Home and Close titlebar buttons.
        // We have a patch that solves that but we are waiting on
        // http://meego.gitorious.org/meegotouch/libmeegotouch/merge_requests/342
        m_icon = MTheme::instance()->pixmap(m_iconId, QSize(0, 0));
    }

    checkPendingPixmap();
}

void MDeclarativeIcon::checkPendingPixmap()
{
    // In MeeGo the themeserver may run in a separate process. In that case MScalableImages
    // may be created without a proper pixmap, instead a gray 1x1 pixmap is provided.
    // We must account for that situation and then listen for the pixmapRequestFinished signal
    // in order to repaint the primitive.
    // This method is part of a helper infrastructure provided in MDeclarativePrimitive to
    // help subclasses handle that issue.

    if (hasPendingPixmap()) {
        if (!m_pendingPixmap) {
            // If not yet connected to MTheme, connect and wait for update
            connect(MTheme::instance(), SIGNAL(pixmapRequestsFinished()), SLOT(checkPendingPixmap()));
            m_pendingPixmap = 1;
        }
    } else {
        if (m_pendingPixmap) {
            // If still connected to MTheme, disconnect.
            disconnect(MTheme::instance(), SIGNAL(pixmapRequestsFinished()));
            m_pendingPixmap = 0;
        }
        update();
    }
}

bool MDeclarativeIcon::hasPendingPixmap()
{
    if (!m_icon) {
        setImplicitWidth(0);
        setImplicitHeight(0);
        return false;
    }

    if (m_icon->size() != QSize(-1, -1)) {
        setImplicitWidth(m_icon->width());
        setImplicitHeight(m_icon->height());
        return false;
    }

    return true;
}

void MDeclarativeIcon::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!m_icon)
        return;

    const bool oldSmooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap(boundingRect().toRect(), *m_icon);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
}
