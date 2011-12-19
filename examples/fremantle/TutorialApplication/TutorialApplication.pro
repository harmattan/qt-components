TEMPLATE = app
QT += declarative
target.path = $$INSTALL_PREFIX/qt-components$$Q_COMPONENTS_VERSION/bin

include(../util/util.pri)

CONFIG -= app_bundle

# Input
SOURCES += main.cpp

OTHER_FILES += \
    qml/*.qml

RESOURCES = qml/qmlfiles.qrc

INSTALLS += target
