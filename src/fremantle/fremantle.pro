include (../../qt-components.pri)

TARGETPATH = org/maemo/fremantle
TEMPLATE = lib
TARGET = $$qtLibraryTarget(fremantleplugin)
INCLUDEPATH += $$PWD $$PWD/indicators

DEFINES += THEME_DIR=\\\"\"$$THEME_DIR\"\\\"
DEFINES += CACHE_DIR=\\\"\"$$CACHE_DIR\"\\\"
DEFINES += DEFAULT_THEME=\\\"\"$$DEFAULT_THEME\"\\\"

force-local-theme: DEFINES+=FORCE_LOCAL_THEME

win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
CONFIG += qt plugin copy_native install_native
QT += declarative svg opengl

maemo5 {
    SOURCES += \
        asyncdbusinterface.cpp \
        fbatteryinfo.cpp \
        fbmedevice.cpp \
        fdbusproxy.cpp \
        fhalservice.cpp \
        fmceservice.cpp \
        forientationdevice.cpp \
        fsliderdevice.cpp \

    HEADERS += \
        asyncdbusinterface.h \
        fbmedevice.h \
        fdbusproxy.h \
        fhalservice.h \
        fmceservice.h \
        forientationdevice.h \
        fsliderdevice.h \

    QT += dbus
    DEFINES += HAVE_DBUS  
}
else {
    SOURCES += \
        mbatteryinfo.cpp \
}

!win32:!embedded:!mac:!symbian {
    CONFIG += link_pkgconfig

}

mobility {
    QT += network
    MOBILITY += sensors systeminfo
    DEFINES += HAVE_SENSORS HAVE_SYSTEMINFO
}

maliit {
    DEFINES += HAVE_MALIIT_FRAMEWORK
    PKGCONFIG += maliit-1.0
}

shader {
    DEFINES += HAVE_SHADER
}

gconf {
    DEFINES += HAVE_GCONF
    PKGCONFIG += gconf-2.0
    
    in  = $$PWD/themedaemon/qtcomponents.schemas.in
    out = $$OUT_PWD/qtcomponents.schemas

    command  = "sed -e \"s:@DEFAULT_THEME@:$$DEFAULT_THEME:g\""
    command += $$in > $$out

    system(mkdir -p $$dirname(out))
    system($$command)
    system(chmod --reference=$$in $$out)

    schemas.files = $$OUT_PWD/qtcomponents.schemas
    schemas.path  = $$INSTALL_SYSCONFDIR/gconf/schemas

    HEADERS  += mimsettings.h
    SOURCES  += mimsettings.cpp
    INSTALLS += schemas
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
    mdeclarativeimattributeextension.cpp \
    mdeclarativeimobserver.cpp \
    mdeclarativeimplicitsizeitem.cpp \
    mdeclarativeinputcontext.cpp \
    mdeclarativemaskeditem.cpp \
    mdeclarativemousefilter.cpp \
    mdeclarativescreen.cpp \
    minversemousearea.cpp \
    mscrolldecoratorsizer.cpp \
    msnapshot.cpp \
    mtexttranslator.cpp \
    mthemedaemon.cpp \
    mthemeplugin.cpp \
    mwindowstate.cpp \
    mx11wrapper.cpp \
    plugin.cpp \
    mdeclarative.cpp \
    themedaemon/mabstractthemedaemonclient.cpp \
    themedaemon/mlocalthemedaemonclient.cpp \
    themedaemon/mlogicalvalues.cpp \
    themedaemon/msystemdirectories.cpp \
    shadereffectitem/3d/qarray.cpp \
    shadereffectitem/3d/qcolor4ub.cpp \
    shadereffectitem/3d/qcustomdataarray.cpp \
    shadereffectitem/3d/qglattributedescription.cpp \
    shadereffectitem/3d/qglattributevalue.cpp \
    shadereffectitem/3d/qglnamespace.cpp \
    shadereffectitem/3d/qmatrix4x4stack.cpp \
    shadereffectitem/geometry.cpp \
    shadereffectitem/shadereffect.cpp \
    shadereffectitem/shadereffectitem.cpp \
    shadereffectitem/shadereffectitemnull.cpp \
    shadereffectitem/shadereffectsource.cpp \
    shadereffectitem/shadereffectsourcenull.cpp \
    shadereffectitem/utilities.cpp \

HEADERS += \
    mbatteryinfo.h \
    mdeclarativeimageprovider.h \
    mdeclarativeimattributeextension.h \
    mdeclarativeimobserver.h \
    mdeclarativeimplicitsizeitem.h \
    mdeclarativeinputcontext.h \
    mdeclarativemaskeditem.h \
    mdeclarativemousefilter.h \
    mdeclarativescreen.h \
    minversemousearea.h \
    mscrolldecoratorsizer.h \
    msnapshot.h \
    mtexttranslator.h \
    mthemedaemon.h \
    mthemeplugin.h \
    mwindowstate.h \
    mwindowstate_p.h \
    mx11wrapper.h \
    plugin.h \
    mdeclarative.h \
    themedaemon/mabstractthemedaemonclient.h \
    themedaemon/mlocalthemedaemonclient.h \
    themedaemon/mlocalthemedaemonclient_p.h \
    themedaemon/msystemdirectories.h \
    themedaemon/mlogicalvalues.h \
    themedaemon/mlogicalvalues_p.h \
    shadereffectitem/3d/qarray.h \
    shadereffectitem/3d/qcolor4ub.h \
    shadereffectitem/3d/qcustomdataarray.h \
    shadereffectitem/3d/qglattributedescription.h \
    shadereffectitem/3d/qglattributevalue.h \
    shadereffectitem/3d/qglnamespace.h \
    shadereffectitem/3d/qmatrix4x4stack.h \
    shadereffectitem/3d/qmatrix4x4stack_p.h \
    shadereffectitem/3d/qt3dglobal.h \
    shadereffectitem/geometry.h \
    shadereffectitem/glfunctions.h \
    shadereffectitem/shadereffect.h \
    shadereffectitem/shadereffectitem.h \
    shadereffectitem/shadereffectitemnull.h \
    shadereffectitem/shadereffectsource.h \
    shadereffectitem/shadereffectsourcenull.h \
    shadereffectitem/utilities.h \

QML_FILES = \
    qmldir \
    EditBubble.js \
    EditBubble.qml \
    style/EditBubbleStyle.qml \
    Magnifier.js \
    Magnifier.qml \
    Utils.js \
    Fader.qml \
    Popup.qml \
    Dialog.qml \
    style/DialogStyle.qml \
    CommonDialog.qml \
    SelectionDialog.qml \
    style/SelectionDialogStyle.qml \
    QueryDialog.qml \
    style/QueryDialogStyle.qml \
    MultiSelectionDialog.js \
    MultiSelectionDialog.qml \
    ScrollDecorator.qml \
    style/ScrollDecoratorStyle.qml \
    AbstractMenu.qml \
    Menu.qml \
    style/MenuStyle.qml \
    MenuItem.qml \
    style/MenuItemStyle.qml \
    MenuLayout.qml \
    ContextMenu.qml \
    style/ContextMenuStyle.qml \
    Label.qml \
    style/LabelStyle.qml \
    Button.qml \
    ButtonRow.qml \
    ButtonColumn.qml \
    ButtonGroup.js \
    style/ButtonStyle.qml \
    CheckBox.qml \
    style/CheckBoxStyle.qml \
    RadioButton.qml \
    style/RadioButtonStyle.qml \
    ToolBarLayout.qml \
    ToolBarLayout.js\
    style/ToolButtonStyle.qml \
    ToolButton.qml \
    TextArea.qml \
    style/TextAreaStyle.qml \
    BusyIndicator.qml \
    style/BusyIndicatorStyle.qml \
    TextAreaHelper.js \
    TextField.qml \
    style/TextFieldStyle.qml \
    ToolButtonRow.qml \
    style/ToolButtonRowStyle.qml \
    style/ToolItemStyle.qml \
    ToolIcon.qml \
    ToolBar.qml \
    TabGroup.qml \
    style/ToolBarStyle.qml \
    StatusBar.qml \
    style/StatusBarStyle.qml \
    ProgressBar.qml \
    style/ProgressBarStyle.qml \
    SliderTemplate.qml \
    Slider.qml \
    style/SliderStyle.qml \
    TabButton.qml \
    style/TabButtonStyle.qml \
    SectionScroller.js \
    SectionScrollerLabel.qml \
    SectionScroller.qml \
    style/SectionScrollerStyle.qml \
    Sheet.qml \
    style/SheetStyle.qml \
    SheetButton.qml \
    style/SheetButtonAccentStyle.qml \
    style/SheetButtonStyle.qml \
    Page.qml \
    PageStack.js \
    PageStack.qml \
    UIConstants.js \
    Window.qml \
    style/WindowStyle.qml \
    SoftwareInputPanel.qml \
    style/Style.qml \
    PageStackWindow.qml \
    style/PageStackWindowStyle.qml \
    Switch.qml \
    style/SwitchStyle.qml \

include(../../qml.pri)
