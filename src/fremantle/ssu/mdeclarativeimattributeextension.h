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

#ifndef MDECLARATIVEIMATTRIBUTEEXTENSION_H
#define MDECLARATIVEIMATTRIBUTEEXTENSION_H

#include <QDeclarativeItem>
#ifdef HAVE_MALIIT_FRAMEWORK
# include <maliit/attributeextension.h>
# include <maliit/inputmethod.h>
# include <maliit/namespace.h>
#endif

class MDeclarativeIMAttributeExtension : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit MDeclarativeIMAttributeExtension(QDeclarativeItem *parent = 0);

    Q_PROPERTY(bool actionKeyHighlighted READ isActionKeyHighlighted WRITE setActionKeyHighlighted NOTIFY actionKeyHighlightedChanged);
    Q_PROPERTY(bool actionKeyEnabled READ isActionKeyEnabled WRITE setActionKeyEnabled NOTIFY actionKeyEnabledChanged);
    Q_PROPERTY(QString actionKeyLabel READ actionKeyLabel WRITE setActionKeyLabel NOTIFY actionKeyLabelChanged);
    Q_PROPERTY(QString actionKeyIcon READ actionKeyIcon WRITE setActionKeyIcon NOTIFY actionKeyIconChanged);

    bool isActionKeyHighlighted() const {
#ifdef HAVE_MALIIT_FRAMEWORK
        return m_extension->attributes().value("/keys/actionKey/highlighted").toBool();
#else
        return false;
#endif
    }

    void setActionKeyHighlighted( bool isHighlighted ) {
#ifdef HAVE_MALIIT_FRAMEWORK
        m_extension->setAttribute("/keys/actionKey/highlighted", QVariant(isHighlighted));
        emit actionKeyHighlightedChanged();
#else
        Q_UNUSED(isHighlighted);
#endif
    }

    bool isActionKeyEnabled() const {
#ifdef HAVE_MALIIT_FRAMEWORK
        return m_extension->attributes().value("/keys/actionKey/enabled").toBool();
#else
        return false;
#endif
    }

    void setActionKeyEnabled( bool isEnabled ) {
#ifdef HAVE_MALIIT_FRAMEWORK
        m_extension->setAttribute("/keys/actionKey/enabled", QVariant(isEnabled));
        emit actionKeyEnabledChanged();
#else
        Q_UNUSED(isEnabled);
#endif
    }

    QString actionKeyLabel() const {
#ifdef HAVE_MALIIT_FRAMEWORK
        return m_extension->attributes().value("/keys/actionKey/label").toString();
#else
        return QString();
#endif
    }

    void setActionKeyLabel( QString newLabel ) {
#ifdef HAVE_MALIIT_FRAMEWORK
        m_extension->setAttribute("/keys/actionKey/label", QVariant(newLabel));
        emit actionKeyLabelChanged();
#else
        Q_UNUSED(newLabel);
#endif
    }

    QString actionKeyIcon() const {
#ifdef HAVE_MALIIT_FRAMEWORK
        return m_extension->attributes().value("/keys/actionKey/icons").toString();
#else
        return QString();
#endif
    }

    void setActionKeyIcon( QString newIcon ) {
#ifdef HAVE_MALIIT_FRAMEWORK
        m_extension->setAttribute("/keys/actionKey/icon", QVariant(newIcon));
        emit actionKeyIconChanged();
#else
        Q_UNUSED(newIcon);
#endif
    }

    Q_INVOKABLE void registerInputElement(QDeclarativeItem *element) const {
#if defined(HAVE_MALIIT_FRAMEWORK)

#define CHECK_VER(v, V) v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) >= V
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));

        if (element && CHECK_VER(v, 0x040704))
            element->setProperty(Maliit::InputMethodQuery::attributeExtensionId, QVariant(m_extension->id()));
#undef CHECK_VER
#else
        Q_UNUSED(element);
#endif
    }

signals:
    void actionKeyHighlightedChanged();
    void actionKeyEnabledChanged();
    void actionKeyLabelChanged();
    void actionKeyIconChanged();

public slots:

private:
#ifdef HAVE_MALIIT_FRAMEWORK
    QScopedPointer<Maliit::AttributeExtension> m_extension;
#endif
};

#endif // MDECLARATIVEIMATTRIBUTEEXTENSION_H
