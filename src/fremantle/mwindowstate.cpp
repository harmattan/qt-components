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

#include <QWidget>
#include <QApplication>
#include "mwindowstate.h"
#include "mwindowstate_p.h"

#ifdef Q_WS_MAEMO_5
#define ACTIVE_APP_ATOM "_MB_CURRENT_APP_WINDOW"
#else
#define ACTIVE_APP_ATOM "_NET_ACTIVE_WINDOW"
#endif

MWindowStatePrivate * MWindowStatePrivate::instance = NULL;

bool (*MWindowStatePrivate::origEventFilter)(void*, long int*) = NULL;

MWindowStatePrivate::MWindowStatePrivate()
    : viewMode(MWindowState::Fullsize)
    , focus(MWindowStatePrivate::FENone)
    , visible(false)
    , active(false)
    , fullyObscured(false)
    , visibleInSwitcherPropertySet(false)
    , animating(false)
    , eventMaskSet(false)
{
    MWindowStatePrivate::instance = this;
}

MWindowState::~MWindowState()
{
    delete d_ptr;
}

MWindowStatePrivate::~MWindowStatePrivate()
{
#ifdef Q_WS_X11
    if (qApp) {
        qApp->setEventFilter(origEventFilter);
    }
#endif
}

#ifdef Q_WS_X11
namespace
{
const unsigned int VisibleChangedDelay = 1000;
}

static int handleXError(Display *, XErrorEvent *)
{
    return 0;
}

Window MWindowStatePrivate::effectiveWinId(Window winIdFromEvent)
{
    QWidget * window = QWidget::find(winIdFromEvent);
    if (window) {
        return window->effectiveWinId();
    } else {
        return 0;
    }
}

void MWindowStatePrivate::initVisibilityWatcher()
{
    origEventFilter = QApplication::instance()->setEventFilter(eventFilter);
}

void MWindowStatePrivate::initVisibleChangedTimer()
{
    Q_Q(MWindowState);

    visibleChangedTimer.connect(&visibleChangedTimer, SIGNAL(timeout()),
                                q, SLOT(_q_doVisibleChangedNotVisible()));
    visibleChangedTimer.setInterval(VisibleChangedDelay);
    visibleChangedTimer.setSingleShot(true);
}

void MWindowStatePrivate::appendEventMask(Window win)
{
    XWindowAttributes existingAttributes;
    XSetWindowAttributes newAttributes;
    Status status;

    status = MX11Wrapper::XGetWindowAttributes(QX11Info::display(), win, &existingAttributes);
    if (status == 0) {
        qFatal("MWindow: XGetWindowAttributes() failed!");
    }

    newAttributes.event_mask = existingAttributes.your_event_mask |
                               PropertyChangeMask |
                               FocusChangeMask;

    MX11Wrapper::XChangeWindowAttributes(QX11Info::display(), win, CWEventMask, &newAttributes);
}

bool MWindowStatePrivate::eventFilter(void *message, long int *result)
{
    Q_UNUSED(result);

    XEvent *event = reinterpret_cast<XEvent*>(message);
    if (event->type == PropertyNotify) {
        XPropertyEvent *xevent = reinterpret_cast<XPropertyEvent *>(event);
        instance->handleXPropertyEvent(xevent);
        return false;

    } else if (event->type == FocusIn || event->type == FocusOut) {
        XFocusChangeEvent *xevent = reinterpret_cast<XFocusChangeEvent *>(event);
        instance->handleXFocusChangeEvent(xevent);
        return false;
    }

    if (origEventFilter) {
        return origEventFilter(message, result);
    }
    else {
        return false;
    }
}

void MWindowStatePrivate::handleXPropertyEvent(XPropertyEvent *xevent)
{
    static WId topLevelWindow = 0;

    if (xevent->state == PropertyNewValue) {

        static unsigned long rootWindow = QX11Info::appRootWindow();
        static Atom switcherAtom     = MX11Wrapper::XInternAtom(
                    QX11Info::display(), ACTIVE_APP_ATOM, true);

        if (xevent->atom == switcherAtom && xevent->window == rootWindow) {
            // Read value of the property. Should be 1 or 0.

            Atom type;
            int  format, status;
            WId activeWindow = 0;
            unsigned long  n, extra;
            unsigned char *data = 0;

            status = MX11Wrapper::XGetWindowProperty(
                        QX11Info::display(), xevent->window, switcherAtom,
                        0L, 16L, 0, XA_WINDOW, &type, &format, &n, &extra, &data);

            if (status == Success && type == XA_WINDOW && format == 32 && n == 1 && data != NULL) {
                // On fremantle we get a 0xfffffff window when we are in task switcher
                // so logic is like this:
                //
                // 1. rooWindow equals to AppWindow?
                // 1.1 yes -> we are on fullscreen mode
                // 1.2 no  -> equals to 0xfffffff ?
                // 1.2.1 yes -> in task mode
                // 1.2.2 no  -> in desktop or other app
                //
                activeWindow =((WId*)data)[0];
                if (!topLevelWindow) {
                    topLevelWindow = activeWindow;
                }
                visibleInSwitcherPropertySet = (activeWindow == 0xFFFFFFFF);
                fullyObscured = (activeWindow != topLevelWindow);

                // Visible in switcher property was added
                // => window is visible as a thumbnail
                if (visibleInSwitcherPropertySet) {
                    doViewModeChanged(MWindowState::Thumbnail);
                    doVisibleChanged(true);

                // Visible in switcher property was removed and
                // fullyObscured is already set => window is not
                // visible
                } else if (fullyObscured) {
                    doVisibleChanged(false);

                // Visible in switcher property was removed and
                // fullyObscured is not set => window visible and fullsize
                } else {
                    doViewModeChanged(MWindowState::Fullsize);
                    doVisibleChanged(true);
                }

                MX11Wrapper::XFree(data);
            }
        }
    }
}

void MWindowStatePrivate::handleXFocusChangeEvent(XFocusChangeEvent *xevent)
{
    Window winId = effectiveWinId(xevent->window);

    // FREMANTLE: remove the first_focus stuff becouse MB WM doesn't handle Visibility
    // events
    if (xevent->window == winId) {
        if (xevent->mode == NotifyNormal) {
            if (xevent->type == FocusIn) {
                focus = FEFocusIn;
                doActiveChanged(true);
            } else {
                focus = FEFocusOut;
                doActiveChanged(false);
            }
        }
    }
}

void MWindowStatePrivate::doActiveChanged(bool newActive)
{
    Q_Q(MWindowState);

    if (active != newActive) {
        active = newActive;
        emit q->activeChanged();
    }
}

void MWindowStatePrivate::_q_doVisibleChangedNotVisible()
{
    Q_Q(MWindowState);

    // Check that window is fully obscured and thumbnail is
    // not being seen in the switcher
    if (!visibleInSwitcherPropertySet && fullyObscured) {
        visible = false;
        emit q->visibleChanged();
    }
}

void MWindowStatePrivate::doViewModeChanged(MWindowState::ViewMode newViewMode)
{
    Q_Q(MWindowState);

    if (viewMode != newViewMode) {
        viewMode = newViewMode;
        emit q->viewModeChanged();
    }
}

void MWindowStatePrivate::doVisibleChanged(bool newVisible)
{
    Q_Q(MWindowState);

    // Emit visibleChanged() immediately if we are becaming
    // visible. Otherwise we need to wait a bit due to possible race
    // between _MEEGOTOUCH_VISIBLE_IN_SWITCHER and X11's visibility
    // events (that race condition causes oscillation).

    if (visible != newVisible) {
        if (newVisible) {
            visibleChangedTimer.stop();
            visible = true;
            emit q->visibleChanged();
            // Set the window active if it was before
            if (focus == FEFocusIn) {
                doActiveChanged(true);
            }
        } else {
            visibleChangedTimer.start();
            // Set the window not active
            doActiveChanged(false);
        }
    }
}

void MWindowState::_q_doVisibleChangedNotVisible()
{
    Q_D(MWindowState);
    d->_q_doVisibleChangedNotVisible();
}

#endif // Q_WS_X11

static MWindowState *self = 0;

MWindowState* MWindowState::instance()
{
    if (!self)
        self = new MWindowState();
    return self;
}

MWindowState::MWindowState(QObject *parent) :
    QObject(parent),
    d_ptr(new MWindowStatePrivate)
{
    Q_D(MWindowState);
    d->q_ptr = this;

#ifdef Q_WS_X11
    d->initVisibilityWatcher();
    d->initVisibleChangedTimer();
#endif // Q_WS_X11
}

MWindowState::ViewMode MWindowState::viewMode() const
{
    Q_D(const MWindowState);
    return d->viewMode;
}

QString MWindowState::viewModeString() const
{
    Q_D(const MWindowState);

    const char *s = 0;
    switch (d->viewMode) {
    case Fullsize:
        s = "Fullsize";
        break;
    case Thumbnail:
        s = "Thumbnail";
        break;
    }

    return QString::fromLatin1(s);
}

bool MWindowState::visible() const
{
    Q_D(const MWindowState);
    return d->visible;
}

bool MWindowState::active() const
{
    Q_D(const MWindowState);
    return d->active;
}

bool MWindowState::animating() const
{
    Q_D(const MWindowState);
    return d->animating;
}

void MWindowState::setAnimating(bool animatingStatus)
{
    Q_D(MWindowState);
    if (animatingStatus != d->animating) {
        d->animating = animatingStatus;
        emit animatingChanged();
    }
}
