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
#include <QWindowStateChangeEvent>

#include "mdeclarativescreen.h"
#include "mdeclarativeinputcontext.h"
#include "mwindowstate.h"
#include <QDesktopWidget>
#include <math.h>

#ifdef HAVE_CONTEXTSUBSCRIBER
# include "contextproperty.h"
# include "contextpropertyinfo.h"
# include "mservicelistener.h"
#else
# if !defined(Q_WS_MAEMO_5) && defined(HAVE_SENSORS)
# include <qorientationsensor.h>
  QTM_USE_NAMESPACE
# endif
#endif

#ifdef Q_WS_X11
  // These includes conflict with some of Qt's types, so should be kept last
# include <QX11Info>
# include <X11/Xatom.h>
# include <X11/Xlib.h>
#endif

#ifdef HAVE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#if !defined(HAVE_CONTEXTSUBSCRIBER)  && defined(Q_WS_MAEMO_5)
# include "fsliderdevice.h"
# include "forientationdevice.h"
#endif

//Stuff for minimize/maximize on N900
#ifdef Q_WS_MAEMO_5
# include <QDBusConnection>
# include <QDBusMessage>
#endif


static const qreal CATEGORY_SMALL_LIMIT  = 3.2;
static const qreal CATEGORY_MEDIUM_LIMIT = 4.5;
static const qreal CATEGORY_LARGE_LIMIT  = 7.0;
static const qreal DENSITY_SMALL_LIMIT   = 140.0;
static const qreal DENSITY_MEDIUM_LIMIT  = 180.0;
static const qreal DENSITY_LARGE_LIMIT   = 270.0;

class MDeclarativeScreenPrivate
{
public:
    MDeclarativeScreenPrivate(MDeclarativeScreen *qq);
    ~MDeclarativeScreenPrivate();

    void updateX11OrientationAngleProperty();
    void initContextSubscriber();
    void initMobilityBackends();

    void _q_isCoveredChanged();
    void _q_updateOrientationAngle();
    void _q_updateIsTvConnected();
    void _q_windowAnimationChanged();

    qreal dpi() const;
    int rotation() const;

    MDeclarativeScreen::Orientation physicalOrientation() const;

    MDeclarativeScreen *q;

    MDeclarativeScreen::Orientation orientation;
    MDeclarativeScreen::Orientation finalOrientation;
    MDeclarativeScreen::Orientations allowedOrientations;
    MDeclarativeScreen::Orientations allowedOrientationsBackup;

    bool isCovered;
    bool keyboardOpen;
    bool isTvConnected;

    QPointer<QWidget> topLevelWidget;

    QCoreApplication::EventFilter oldEventFilter;

    QSize displaySize;
    QSize screenSize;

    bool allowSwipe;
    void setMinimized(bool);
    bool isMinimized() const;
    bool isRemoteScreenPresent() const;
    QString topEdgeValue() const;

#ifdef Q_WS_X11
    WId windowId;
#endif

#ifdef HAVE_CONTEXTSUBSCRIBER
    ContextProperty topEdgeProperty;
    ContextProperty remoteTopEdgeProperty;
    ContextProperty isCoveredProperty;
    ContextProperty keyboardOpenProperty;
    ContextProperty videoRouteProperty;
    MServiceListener remoteTopEdgeListener;
#else
# ifdef Q_WS_MAEMO_5
    FSliderDevice k;
    FOrientationDevice o;
# else
#  ifdef HAVE_SENSORS
    QOrientationSensor orientationSensor;
#  endif
# endif
#endif

#ifdef Q_OS_BLACKBERRY

#endif

private:
    bool minimized;
};

static MDeclarativeScreen *self = 0;
static MDeclarativeScreenPrivate *gScreenPrivate = 0;

MDeclarativeScreen* MDeclarativeScreen::instance()
{
    if (!self)
        self = new MDeclarativeScreen();
    return self;
}

#ifdef Q_WS_X11
// This writes the orientation angle of into the X11 window property,
// which makes OS dialogs coming on top follow the app orientation
static void writeX11OrientationAngleProperty(WId id, int angle)
{
    Display *display = QX11Info::display();

    if (!display)
        return;

    Atom orientationAngleAtom = XInternAtom(display, "_MEEGOTOUCH_ORIENTATION_ANGLE", False);

    XChangeProperty(display, id, orientationAngleAtom, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char*)&angle, 1);
}
#endif

bool x11EventFilter(void *message, long *result)
{
#ifdef Q_WS_X11
    XEvent *event = (XEvent *)message;

    Q_UNUSED(result);

    if (event->type == MapNotify) {
        XMapEvent * xevent = (XMapEvent*) event;
        gScreenPrivate->windowId = xevent->window;
        writeX11OrientationAngleProperty(xevent->window, gScreenPrivate->rotation());
    }
#endif

    if (gScreenPrivate->oldEventFilter) {
        return gScreenPrivate->oldEventFilter(message, result);
    } else
        return false;
}

MDeclarativeScreenPrivate::MDeclarativeScreenPrivate(MDeclarativeScreen *qq)
    : q(qq)
    , orientation(MDeclarativeScreen::Portrait)
    , finalOrientation(MDeclarativeScreen::Portrait)
    , allowedOrientations(MDeclarativeScreen::Landscape | MDeclarativeScreen::Portrait)
    , isCovered(false)
    , keyboardOpen(false)
    , isTvConnected(false)
    , topLevelWidget(0)
    , oldEventFilter(0)
    , allowSwipe(true)
#ifdef Q_WS_X11
    , windowId(0)
#endif
#ifdef HAVE_CONTEXTSUBSCRIBER
    , topEdgeProperty("Screen.TopEdge")
    , remoteTopEdgeProperty("RemoteScreen.TopEdge")
    , isCoveredProperty("Screen.IsCovered")
    , keyboardOpenProperty("/maemo/InternalKeyboard/Open")
    , videoRouteProperty("com.nokia.policy.video_route")
    , remoteTopEdgeListener(remoteTopEdgeProperty.info()->providerDBusType()
    , remoteTopEdgeProperty.info()->providerDBusName())
#else
# ifdef Q_WS_MAEMO_5
    , k(SLIDER_DEVICE)
    , o(ORIENT_DEVICE, ORIENT_SIGNAL)
# else 
#  ifdef HAVE_SENSORS
    , orientationSensor(0)
#  endif
# endif
#endif
    , minimized(false)
{
    // TODO: Could use QDesktopWidget, but what about on host PC?

    QDesktopWidget* screen = qApp->desktop();

    if (screen) {
        displaySize = screen->screenGeometry().size();
        qDebug() << "MDeclarativeScreen" << "size:" << displaySize;
    }

    screenSize = QSize(displaySize.width(), displaySize.height());

    oldEventFilter = QCoreApplication::instance()->setEventFilter(x11EventFilter);
    //Q_ASSERT(gScreenPrivate == 0);
    gScreenPrivate = this;
}

MDeclarativeScreenPrivate::~MDeclarativeScreenPrivate()
{
}

void MDeclarativeScreenPrivate::initContextSubscriber()
{
#ifdef HAVE_CONTEXTSUBSCRIBER
    //waiting for properties to synchronize
    topEdgeProperty.waitForSubscription();
    isCoveredProperty.waitForSubscription();
    keyboardOpenProperty.waitForSubscription();
    videoRouteProperty.waitForSubscription();
    remoteTopEdgeListener.startListening(true);

    QObject::connect(&topEdgeProperty, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateOrientationAngle()));
    QObject::connect(&remoteTopEdgeProperty, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateOrientationAngle()));
    QObject::connect(&keyboardOpenProperty, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateOrientationAngle()));
    QObject::connect(&isCoveredProperty, SIGNAL(valueChanged()),
                     q, SLOT(_q_isCoveredChanged()));
    QObject::connect(&videoRouteProperty, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateIsTvConnected()));
    QObject::connect(&remoteTopEdgeListener, SIGNAL(nameAppeared()),
                     q, SLOT(_q_updateOrientationAngle()));
    QObject::connect(&remoteTopEdgeListener, SIGNAL(nameDisappeared()),
                     q, SLOT(_q_updateOrientationAngle()));
#else 
# ifdef Q_WS_MAEMO_5
    QObject::connect(&k, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateOrientationAngle()));
    QObject::connect(&o, SIGNAL(valueChanged()),
                     q, SLOT(_q_updateOrientationAngle()));

    // Start to listen to events by default
    o.start(q);
    k.start(q);
# endif 
#endif

    //initiating the variables to current orientation
    _q_updateOrientationAngle();
    _q_isCoveredChanged();
    updateX11OrientationAngleProperty();
    _q_updateIsTvConnected();

    QObject::connect(MWindowState::instance(), SIGNAL(animatingChanged()),
                     q, SLOT(_q_windowAnimationChanged()));
}

void MDeclarativeScreenPrivate::initMobilityBackends()
{
#if !defined (Q_WS_MAEMO_5) && defined(HAVE_SENSORS)
    qDebug() << "MDeclarativeScreen" << "orientation sensor init";
    orientationSensor.connectToBackend();
    orientationSensor.start();
    if (!orientationSensor.isActive()) {
        qWarning("OrientationSensor didn't start!");
    }
    qDebug() << "MDeclarativeScreen" << "orientation sensor init - active ?" << orientationSensor.isActive();
    //Connect to updateOrientationAngle slot
    QObject::connect(&orientationSensor, SIGNAL(readingChanged()),
                     q, SLOT(_q_updateOrientationAngle()));
    qDebug() << "MDeclarativeScreen" << "orientation sensor signal connected";
#endif
    return;
}

void MDeclarativeScreenPrivate::updateX11OrientationAngleProperty()
{
#ifdef Q_WS_X11
    if (!windowId)
        return;

    writeX11OrientationAngleProperty(windowId, q->rotation());
#endif
}

void MDeclarativeScreenPrivate::_q_isCoveredChanged()
{
#ifdef HAVE_CONTEXTSUBSCRIBER
    bool covered = isCoveredProperty.value().toBool();

    if (isCovered != covered) {
        qDebug() << "MDeclarativeScreenPrivate" << "Covered:" << covered;

        isCovered = covered;
        emit q->coveredChanged();
    }
#endif
}

void MDeclarativeScreenPrivate::_q_updateIsTvConnected()
{
#ifdef HAVE_CONTEXTSUBSCRIBER
    QString value = videoRouteProperty.value().toString();

    isTvConnected = (value == "tvout" ||
                     value == "builtinandtvout");
    _q_updateOrientationAngle();
#endif
}

qreal MDeclarativeScreenPrivate::dpi() const
{
    static qreal dpi = 0;
#if defined(Q_WS_X11) && defined(HAVE_XRANDR)
    if (!dpi) {
        int physicalHeight = 0;
        XRRScreenResources *sr;
        sr = XRRGetScreenResources(QX11Info::display(), RootWindow(QX11Info::display(), 0));
        if (sr->noutput) {
            XRROutputInfo *output = XRRGetOutputInfo(QX11Info::display(),sr,sr->outputs[0]);
            if (output->crtc) {
               physicalHeight = output->mm_height;
            }
            XRRFreeOutputInfo(output);
        }
        XRRFreeScreenResources(sr);
        dpi = QDesktopWidget().screenGeometry().height() / (physicalHeight / 25.4);
    }
#else
    if (!dpi)
        dpi = QDesktopWidget().physicalDpiX();
#endif
    return dpi;
}

int MDeclarativeScreenPrivate::rotation() const
{
    int angle = 0;

    switch (orientation) {
    case MDeclarativeScreen::Landscape:
        angle = 270;
        break;
    case MDeclarativeScreen::Portrait:
    case MDeclarativeScreen::Default:  //handle default as portrait
        angle = 0;
        break;
    case MDeclarativeScreen::LandscapeInverted:
        angle = 180;
        break;
    case MDeclarativeScreen::PortraitInverted:
        angle = 90;
        break;
    default:
        qCritical() << "MDeclarativeScreen hast invalid orientation set.";
    }
    return angle;
}

MDeclarativeScreen::Orientation MDeclarativeScreenPrivate::physicalOrientation() const {
    MDeclarativeScreen::Orientation o = MDeclarativeScreen::Default;

#if defined(HAVE_CONTEXTSUBSCRIBER) || defined (Q_WS_MAEMO_5) || defined(HAVE_SENSORS)
    QString topEdge = topEdgeValue();

    if (topEdge == "top") {
        o = MDeclarativeScreen::Portrait;
    } else if (topEdge == "left") {
        o = MDeclarativeScreen::Landscape;
    } else if (topEdge == "right") {
        o = MDeclarativeScreen::PortraitInverted;
    } else if (topEdge == "bottom") {
        o = MDeclarativeScreen::LandscapeInverted;
    }
#endif
    return o;
}

void MDeclarativeScreenPrivate::_q_updateOrientationAngle()
{
    MDeclarativeScreen::Orientation newOrientation = MDeclarativeScreen::Default;
    //qDebug() << "MDeclarativeScreen" << "_q_updateOrientationAngle";

#if defined(HAVE_CONTEXTSUBSCRIBER) || defined (Q_WS_MAEMO_5) || defined(HAVE_SENSORS)
    QString edge = topEdgeValue();
#endif

#ifdef HAVE_CONTEXTSUBSCRIBER
    bool open = keyboardOpenProperty.value().toBool();
#else
# ifdef Q_WS_MAEMO_5
    bool open = k.isOpen();
# else
#  ifdef HAVE_SENSORS
    bool open = false;
#  endif
# endif
#endif

#if defined(HAVE_CONTEXTSUBSCRIBER) || defined (Q_WS_MAEMO_5) || defined(HAVE_SENSORS) 
    //HW Keyboard open or TV connected causes a switch to landscape, but only if this is allowed
    qDebug() << "MDeclarativeScreen" << "edge:" << edge;
    if ((open || isTvConnected) && allowedOrientations & MDeclarativeScreen::Landscape) {
        newOrientation = MDeclarativeScreen::Landscape;
    } else if (edge == "top" && (allowedOrientations & MDeclarativeScreen::Portrait)) {
        newOrientation = MDeclarativeScreen::Portrait;
    } else if (edge == "left" && (allowedOrientations & MDeclarativeScreen::Landscape)) {
        newOrientation = MDeclarativeScreen::Landscape;
    } else if (edge == "right" && (allowedOrientations & MDeclarativeScreen::PortraitInverted)) {
        newOrientation = MDeclarativeScreen::PortraitInverted;
    } else if (edge == "bottom" && (allowedOrientations & MDeclarativeScreen::LandscapeInverted)) {
        newOrientation = MDeclarativeScreen::LandscapeInverted;
    }

    if (open != keyboardOpen) {
        keyboardOpen = open;
        emit q->keyboardOpenChanged();
    }
#endif

    //only set the new orientation if it is a valid one
    if(newOrientation != MDeclarativeScreen::Default) {
        q->setOrientation(newOrientation);
    }
}

void MDeclarativeScreenPrivate::_q_windowAnimationChanged()
{
    if (!MWindowState::instance()->animating() && finalOrientation != orientation)
        q->setOrientation(finalOrientation);
}

void MDeclarativeScreenPrivate::setMinimized(bool m) {
    if(minimized == m)
        return;

    minimized = m;
    emit q->minimizedChanged();
}

bool MDeclarativeScreenPrivate::isMinimized() const {
    return minimized;
}

bool MDeclarativeScreenPrivate::isRemoteScreenPresent() const {
    bool present = false;
#ifdef HAVE_CONTEXTSUBSCRIBER
    QString remoteTopEdge = remoteTopEdgeProperty.value().toString();
    bool remoteTopEdgePresent = (remoteTopEdgeListener.isServicePresent() == MServiceListener::Present);
    present = remoteTopEdgePresent && !remoteTopEdge.isEmpty();
#endif
    return present;
}

QString MDeclarativeScreenPrivate::topEdgeValue() const {
    QString top; // Empty string simulates MDeclarativeScreen::Default orientation
#ifdef HAVE_CONTEXTSUBSCRIBER
    QString topEdge = topEdgeProperty.value().toString();
    QString remoteTopEdge = remoteTopEdgeProperty.value().toString();
    top = isRemoteScreenPresent() ? remoteTopEdge : topEdge;
#else
# ifdef Q_WS_MAEMO_5
    top = o.getOrientation();
# else
#  ifdef HAVE_SENSORS
    qDebug() << "MDeclarativeScreen" << "orientationSensor active:" << orientationSensor.isActive();
    if (!orientationSensor.isActive()) {
        return top;
    }
    switch (orientationSensor.reading()->orientation()) {
       case QOrientationReading::TopUp:     top = "top"; break;
       case QOrientationReading::TopDown:   top = "bottom"; break;
       case QOrientationReading::LeftUp:    top = "left"; break;
       case QOrientationReading::RightUp:   top = "right"; break;
       case QOrientationReading::FaceUp:    top = "up"; break;
       case QOrientationReading::FaceDown:  top = "down"; break;
       case QOrientationReading::Undefined: top = "Undefined"; break;
       default: top = "Invalid enum value";
    }
#  endif
# endif
#endif
    return top;
}

MDeclarativeScreen::MDeclarativeScreen(QDeclarativeItem *parent)
        : QObject(parent),
        d(new MDeclarativeScreenPrivate(this))
{
    d->initMobilityBackends();
    d->initContextSubscriber();
    qApp->installEventFilter(this);
}

MDeclarativeScreen::~MDeclarativeScreen()
{
    delete d;
}



//    //qDebug() << "MDeclarativeScreen" << "event type" << e;
//
//    //if(e->type() == QEvent::ActivationChange) {
//    if(e->type() == QEvent::ApplicationActivate || e->type() == QEvent::ApplicationDeactivate) {
//        //if(e->type() == QEvent::WindowActivate || e->type() == QEvent::WindowDeactivate) {
//
//        //d->topLevelWidget = qobject_cast<QWidget*>(o);
//        //if(d->topLevelWidget && d->topLevelWidget->parent() == NULL) { //it's a toplevelwidget
//        if(1) {
//            d->setMinimized(e->type() == QEvent::ApplicationDeactivate);
//            //if(!d->topLevelWidget->isActiveWindow()) {
//            if(e->type() == QEvent::ApplicationDeactivate) {
//            //qDebug() << "MDeclarativeScreen" << "state event" << e;
//            //qDebug() << "MDeclarativeScreen" << "state event type" << e->type();
//	        //Stop keyboard and orientation watcher




bool MDeclarativeScreen::eventFilter(QObject *o, QEvent *e) {

// On BB10 with QBB_USE_OPENGL set, even touching the screen sends the ActivationChange event
// but ApplicationActivate/ApplicationDeactivate are sent correctly only when the application window
// gets/looses focus (or it the screen is turned on/off)
#ifdef Q_OS_BLACKBERRY
    if(e->type() == QEvent::ApplicationActivate || e->type() == QEvent::ApplicationDeactivate) {
        if(1) { // looks like the event can't come from foreign window on BB10
            d->setMinimized(e->type() == QEvent::ApplicationDeactivate);
            if(e->type() == QEvent::ApplicationDeactivate) {
#else
    if(e->type() == QEvent::ActivationChange) {
        d->topLevelWidget = qobject_cast<QWidget*>(o);
        if(d->topLevelWidget && d->topLevelWidget->parent() == NULL) { //it's a toplevelwidget
            d->setMinimized(!d->topLevelWidget->isActiveWindow());
            if(d->isMinimized()) {
	        //Stop keyboard and orientation watcher
#endif

#ifdef Q_WS_MAEMO_5
	        d->k.stop(this); 
		d->o.stop(this);
#else
# ifdef HAVE_SENSORS
        qDebug() << "MDeclarativeScreen" << "orientationSensor stop";
		d->orientationSensor.stop();
# endif
#endif

#ifdef Q_OS_BLACKBERRY // don't switch to landscape when minimized on BB10
bool landscapeMinimized = false;
#else
bool landscapeMinimized = true;
#endif
                if(landscapeMinimized){
                    qDebug() << "MDeclarativeScreen" << "landscape minimized";
                    //minimized apps are forced to landscape
                    d->allowedOrientationsBackup = d->allowedOrientations;
                    //set allowedOrientations manually, because
                    //setAllowedOrientations() will not work while
                    //minimized. For Fremantle, force to Landscape
                    d->allowedOrientations = Landscape;
                    //setOrientation(Landscape);
                }
            } else {
	        //Start watchers.
#ifdef Q_WS_MAEMO_5
	      d->k.start(this); d->o.start(this);
#else
# ifdef HAVE_SENSORS
          qDebug() << "MDeclarativeScreen" << "orientationSensor start";
	      d->orientationSensor.start();
# endif
#endif
                if(d->allowedOrientationsBackup != Default) {
                    setAllowedOrientations(d->allowedOrientationsBackup);
                    //if the current sensor's value is allowed, switch to it

                    if(d->physicalOrientation() & allowedOrientations())
                        setOrientation(d->physicalOrientation());
                }
            }
        } else {
            qCritical() << "State change event from foreign window";
        }
    }
    return QObject::eventFilter(o, e);
}

void MDeclarativeScreen::setOrientation(Orientation o)
{
    d->finalOrientation = o;

    if (d->orientation == o || MWindowState::instance()->animating())
        return;

    Orientation newOrientation = Default;
    //if keyboard is open always set landscape and ignore allowed orientations
#if defined(HAVE_CONTEXTSUBSCRIBER) || defined (Q_WS_MAEMO_5)
# ifdef HAVE_CONTEXT_SUBSCRIBER
    if(d->keyboardOpenProperty.value().toBool()) {
# else
    if(d->k.isOpen()) {
        newOrientation = Landscape;
    } else {
# endif
#endif
        if (!(d->allowedOrientations & o))
            return;
        newOrientation = o;
#if defined(HAVE_CONTEXTSUBSCRIBER) || defined (Q_WS_MAEMO_5)
    }
#endif
    d->orientation = newOrientation;
    d->updateX11OrientationAngleProperty();

#ifndef USE_DEPRECATED_SCREEN_WIDTH_HEIGHT
    // Update screen width / height properties
    if (newOrientation == MDeclarativeScreen::Landscape ||
            newOrientation == MDeclarativeScreen::LandscapeInverted) {
        d->screenSize.setWidth(d->displaySize.width());
        d->screenSize.setHeight(d->displaySize.height());
    } else {
        d->screenSize.setWidth(d->displaySize.height());
        d->screenSize.setHeight(d->displaySize.width());
    }

    emit widthChanged();
    emit heightChanged();
#endif

    MDeclarativeInputContext::setKeyboardOrientation(o);
    emit currentOrientationChanged();
}

MDeclarativeScreen::Orientation MDeclarativeScreen::currentOrientation() const
{
    return d->orientation;
}

void MDeclarativeScreen::setAllowedOrientations(Orientations orientation) {
    if (d->allowedOrientations == orientation
        || d->isMinimized()) //fixed landscape when minimized, no change possible!
        return;

    d->allowedOrientations = orientation;

#ifdef Q_WS_MAEMO_5
    // This code starts or stops accelerometes
    uint i, flags;
    for (i = 0, flags = 0; i < sizeof(Orientations); i++) {
      flags += ((orientation & (1 << i)) != 0);
    }
    flags > 1 ? d->o.start(this): d->o.stop(this);
#endif

    //qDebug() << "MDeclarativeScreen" << "setAllowedOrientations";
    //qDebug() << "MDeclarativeScreen" << "orient. physical:" << d->physicalOrientation();
    //qDebug() << "MDeclarativeScreen" << "orient. current:" << d->orientation;
    // Check if physical orientation fits allowed orientations
    if(d->physicalOrientation() != d->orientation) {
        //qDebug() << "MDeclarativeScreen" << "al. orient." << "mismatch with physical";
        if(d->physicalOrientation() & d->allowedOrientations) {
            //qDebug() << "MDeclarativeScreen" << "al. orient." << "phys. orient. allowed, setting" ;
            setOrientation(d->physicalOrientation());
        }
    }

    // Check if current orientation still fits allowed
    if(!(d->orientation & d->allowedOrientations)) {
        if(d->allowedOrientations & MDeclarativeScreen::Portrait) {
            setOrientation(MDeclarativeScreen::Portrait);
            return;
        } else if(d->allowedOrientations & MDeclarativeScreen::Landscape) {
            setOrientation(MDeclarativeScreen::Landscape);
            return;
        } else if(d->allowedOrientations & MDeclarativeScreen::LandscapeInverted) {
            setOrientation(MDeclarativeScreen::LandscapeInverted);
            return;
        } else if(d->allowedOrientations & MDeclarativeScreen::PortraitInverted) {
            setOrientation(MDeclarativeScreen::PortraitInverted);
            return;
        }
    }
    emit allowedOrientationsChanged();
}

MDeclarativeScreen::Orientations MDeclarativeScreen::allowedOrientations() const {
    return d->allowedOrientations;
}

QString MDeclarativeScreen::orientationString() const
{
    const char *s = 0;
    switch (d->orientation) {
    case Portrait:
        s = "Portrait";
        break;
    case PortraitInverted:
        s = "PortraitInverted";
        break;
    case Landscape:
        s = "Landscape";
        break;
    case LandscapeInverted:
        s = "LandscapeInverted";
        break;
    default:
        qCritical() << "MDeclarativeScreen has invalid orientation set.";
        break;
    }
    return QString::fromLatin1(s);
}

int MDeclarativeScreen::rotation() const
{
    return d->rotation();
}

bool MDeclarativeScreen::isCovered() const
{
    return d->isCovered;
}

bool MDeclarativeScreen::isKeyboardOpen() const
{
    return d->keyboardOpen;
}

bool MDeclarativeScreen::isMinimized() const
{
    return d->isMinimized();
}

void MDeclarativeScreen::setMinimized(bool minimized)
{
    if(minimized == d->isMinimized())
        return;

    if(d->topLevelWidget) {
#ifdef Q_WS_MAEMO_5
      if (minimized) {
        QDBusMessage m = QDBusMessage::createSignal("/", "com.nokia.hildon_desktop", "exit_app_view");
	QDBusConnection::sessionBus().send(m);
      }
      else {
	d->topLevelWidget->activateWindow();
      }
#else
        d->topLevelWidget->setWindowState(minimized ? Qt::WindowMinimized : Qt::WindowFullScreen);
#endif
        d->setMinimized(minimized);
    } else {
        qCritical() << "No top level widget set";
    }
}

int MDeclarativeScreen::width() const
{
#ifdef USE_DEPRECATED_SCREEN_WIDTH_HEIGHT
    qWarning() << "The semantics of screen.width property is deprecated, see QTCOMPONENTS-521. Please use screen.displayWidth to query native screen width.";
    return d->displaySize.width();
#else
    //Enable these after applications have been changed
    return d->screenSize.width();
#endif
}

int MDeclarativeScreen::height() const
{
#ifdef USE_DEPRECATED_SCREEN_WIDTH_HEIGHT
    qWarning() << "The semantics of screen.height property is deprecated, see QTCOMPONENTS-521. Please use screen.displayHeight to query native screen height.";
    return d->displaySize.height();
#else
    // Enable these after applications have been changed
    return d->screenSize.height();
#endif
}

int MDeclarativeScreen::displayWidth() const
{
    return d->displaySize.width();
}

int MDeclarativeScreen::displayHeight() const
{
    return d->displaySize.height();
}

MWindowState * MDeclarativeScreen::windowState() const
{
    qWarning() << "Warning: screen.windowState() is deprecated, use platformWindow property instead";
    return MWindowState::instance();
}

int MDeclarativeScreen::dpi() const {
    return d->dpi();
}

MDeclarativeScreen::DisplayCategory MDeclarativeScreen::displayCategory() const {
    const int w = QApplication::desktop()->screenGeometry().width();
    const int h = QApplication::desktop()->screenGeometry().height();
    const qreal diagonal = sqrt(static_cast<qreal>(w * w + h * h)) / dpi();
    if (diagonal < CATEGORY_SMALL_LIMIT)
        return Small;
    else if (diagonal < CATEGORY_MEDIUM_LIMIT)
        return Normal;
    else if (diagonal < CATEGORY_LARGE_LIMIT)
        return Large;
    else
        return ExtraLarge;
}

MDeclarativeScreen::Density MDeclarativeScreen::density() const {
    if (dpi() < DENSITY_SMALL_LIMIT)
        return Low;
    else if (dpi() < DENSITY_MEDIUM_LIMIT)
        return Medium;
    else if (dpi() < DENSITY_LARGE_LIMIT)
        return High;
    else
        return ExtraHigh;
}

bool MDeclarativeScreen::allowSwipe() const
{
    return d->allowSwipe;
}

void MDeclarativeScreen::setAllowSwipe(bool enabled)
{
    if (enabled != d->allowSwipe) {
#ifdef Q_WS_X11
        QWidget * activeWindow = QApplication::activeWindow();
        if(!activeWindow) {
            return;
        }
#endif
        d->allowSwipe = enabled;
        emit allowSwipeChanged();
    }
}


#include "moc_mdeclarativescreen.cpp"
