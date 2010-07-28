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

#include "mdeclarativeprimitive.h"
#include "mstylewrapper.h"

#include <MTheme>
#include <MWidgetStyle>

MDeclarativePrimitive::MDeclarativePrimitive(QDeclarativeItem *parent) :
    QDeclarativeItem(parent), m_style(0), m_pendingPixmap(0)
{
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

MDeclarativePrimitive::~MDeclarativePrimitive()
{
    internalClearStyleData();
}

MStyleWrapper *MDeclarativePrimitive::style() const
{
    return m_style;
}

void MDeclarativePrimitive::setStyle(MStyleWrapper *style)
{
    if (style == m_style)
        return;

    if (m_style) {
        // Stop listening for mode changes in the old style
        m_style->disconnect(this);
        internalClearStyleData();
    }

    m_style = style;

    if (m_style) {
        // Listen for mode and/or styleType changes
        connect(m_style, SIGNAL(modeChanged(const QString &)), SLOT(updateStyleData()));
        updateStyleData();
    }
    update();
}

void MDeclarativePrimitive::internalClearStyleData()
{
    // Clear subclass-specific members
    clearStyleData();

    // Disconnect from MTheme in case we were still waiting for pixmaps
    if (m_pendingPixmap) {
        MTheme::instance()->disconnect(this);
        m_pendingPixmap = 0;
    }
}

void MDeclarativePrimitive::updateStyleData()
{
    // This check is to protect from subclass calls to this method
    // We never call updateStyleData ourselves if we have no style set
    if (!m_style)
        return;

    const MWidgetStyleContainer *styleContainer = m_style->styleContainer();

    if (styleContainer) {
        // Fill subclass-specific members with updated data from style
        fetchStyleData(*styleContainer);
        checkPendingPixmap();
    } else {
        internalClearStyleData();
        update();
    }
}

void MDeclarativePrimitive::checkPendingPixmap()
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
            MTheme::instance()->disconnect(this);
            m_pendingPixmap = 0;
        }
        update();
    }
}

void MDeclarativePrimitive::clearStyleData()
{

}

void MDeclarativePrimitive::fetchStyleData(const MWidgetStyleContainer &styleContainer)
{
    Q_UNUSED(styleContainer);
}

bool MDeclarativePrimitive::hasPendingPixmap()
{
    return false;
}
