include (../../../qt-components.pri)

TARGETPATH = org/maemo/extras
TEMPLATE = lib
TARGET = $$qtLibraryTarget(fremantleextrasplugin)
INCLUDEPATH += $$PWD

win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release
CONFIG += qt plugin
QT += declarative

HEADERS += \

SOURCES += \

QML_FILES += \

include(../../../qml.pri)
