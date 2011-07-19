include (../../qt-components.pri)

TARGETPATH = org/maemo/fremantle
TEMPLATE = lib
TARGET = $$qtLibraryTarget(fremantleplugin)
INCLUDEPATH += $$PWD $$PWD/indicators

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
    plugin.cpp \

HEADERS += \
    plugin.h \

QML_FILES = \
	qmldir

include(../../qml.pri)
