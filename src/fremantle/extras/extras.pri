TEMPLATE = lib
INCLUDEPATH += $$PWD

win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release
CONFIG += qt plugin
QT += declarative network


HEADERS += mdatetimehelper.h
SOURCES += plugin.cpp mdatetimehelper.cpp

QML_FILES += \
    qmldir \
    constants.js\
    CountBubble.qml \
    DatePickerDialog.qml \
    InfoBanner.qml \
    style/InfoBannerStyle.qml \
    ListDelegate.qml \
    ListButton.qml \
    MoreIndicator.qml \
    NetPromoterScore.qml \
    PageIndicator.qml \
    RatingIndicator.qml \
    SystemBanner.qml \
    TimePickerDialog.qml \
    Tumbler.qml \
    TumblerNew.qml \
    TumblerDialogNew.qml \
    Tumbler.js \
    TumblerColumn.qml \
    TumblerIndexHelper.js \
    TumblerTemplate.qml \
    TumblerButton.qml \
    TumblerDialog.qml \
    style/ListButtonStyle.qml \
    style/NegativeButtonStyle.qml \
    style/PositiveButtonStyle.qml \
    style/TumblerButtonStyle.qml
