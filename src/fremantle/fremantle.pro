include (../../qt-components.pri)

TARGETPATH = org/maemo/fremantle
TEMPLATE = lib
TARGET = $$qtLibraryTarget(fremantleplugin)
INCLUDEPATH += $$PWD $$PWD/indicators

DEFINES += THEME_DIR=\\\"\"$$THEME_DIR\"\\\"
force-local-theme: DEFINES+=FORCE_LOCAL_THEME

win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
CONFIG += qt plugin copy_native install_native
QT += declarative svg
!win32:!macx: QT += dbus

!win32:!embedded:!mac:!symbian {
    CONFIG += link_pkgconfig
}

mobility {
    QT += network
    MOBILITY += feedback systeminfo
    DEFINES += HAVE_SYSTEMINFO
}

maliit {
    DEFINES += HAVE_MALIIT_FRAMEWORK
    PKGCONFIG += maliit-1.0
}

shader {
    DEFINES += HAVE_SHADER
}

xdamage {
    DEFINES += HAVE_XDAMAGE
    PKGCONFIG += xdamage
}

xrandr:!mac {
    DEFINES += HAVE_XRANDR
    PKGCONFIG += xrandr
}

# Input
SOURCES += \
    mdeclarativeimageprovider.cpp \
    mdeclarativeinputcontext.cpp \
    mdeclarativescreen.cpp \
    msnapshot.cpp \
    mthemeplugin.cpp \
    mwindowstate.cpp \
    mx11wrapper.cpp \
    plugin.cpp \
    themedaemon/mabstractthemedaemonclient.cpp \
    themedaemon/mlocalthemedaemonclient.cpp \

HEADERS += \
    mdeclarativeimageprovider.h \
    mdeclarativeinputcontext.h \
    mdeclarativescreen.h \
    msnapshot.h \
    mthemeplugin.h \
    mwindowstate.h \
    mwindowstate_p.h \
    mx11wrapper.h \
    plugin.h \
    themedaemon/mabstractthemedaemonclient.h \
    themedaemon/mlocalthemedaemonclient.h \

QML_FILES = \
    qmldir \
    PageStack.js \
    PageStack.qml \
    UIConstants.js \
    Window.qml \
    style/WindowStyle.qml \
    SoftwareInputPanel.qml \
    style/Style.qml \

include(../../qml.pri)
