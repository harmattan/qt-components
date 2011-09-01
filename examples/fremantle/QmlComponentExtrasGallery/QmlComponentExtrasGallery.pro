TEMPLATE = app
QT += declarative
target.path = $$INSTALL_PREFIX/qt-components/bin

include(../util/util.pri)

CONFIG -= app_bundle

# Input
SOURCES += main.cpp

OTHER_FILES += \
    qml/*.qml \
    qml/*.js

RESOURCES = qml/qmlfiles.qrc

INSTALLS += target
