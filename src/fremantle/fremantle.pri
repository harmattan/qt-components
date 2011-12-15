TEMPLATE = lib
INCLUDEPATH += $$PWD $$PWD/indicators

DEFINES += THEME_DIR=\\\"\"$$THEME_DIR\"\\\"
DEFINES += CACHE_DIR=\\\"\"$$CACHE_DIR\"\\\"
DEFINES += I18N_DIR=\\\"\"$$I18N_DIR\"\\\"
DEFINES += DEFAULT_THEME=\\\"\"$$DEFAULT_THEME\"\\\"

force-local-theme: DEFINES+=FORCE_LOCAL_THEME

win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
CONFIG += qt plugin copy_native install_native
QT += declarative network opengl

maemo5 {
    SOURCES += \
	fcelldevice.cpp \
	fcellinfo.cpp \
	fservice.cpp \
        asyncdbusinterface.cpp \
        fbatteryinfo.cpp \
        fbmedevice.cpp \
        fdbusproxy.cpp \
        forientationdevice.cpp \
        fsliderdevice.cpp \

    HEADERS += \
	fcelldevice.h \
	fphoneservice.h \
	fservice.h \
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
	mcellinfo.cpp \
}

!win32:!embedded:!mac:!symbian {
    CONFIG += link_pkgconfig

}

mobility {
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
    plugin.cpp \
    mdeclarativescreen.cpp \
    mdeclarativeclipboard.cpp \
    msnapshot.cpp \
    minversemousearea.cpp \
    mdeclarative.cpp \
    mdeclarativeinputcontext.cpp \
    mdeclarativeimageprovider.cpp \
    mdeclarativeimplicitsizeitem.cpp \
    mdeclarativemaskeditem.cpp \
    mnetworkinfo.cpp \
    mscrolldecoratorsizer.cpp \
    mtexttranslator.cpp \
    mthemedaemon.cpp \
    mthemeplugin.cpp \
    mwindowstate.cpp \
    mx11wrapper.cpp \
    themedaemon/mabstractthemedaemonclient.cpp \
    themedaemon/mlocalthemedaemonclient.cpp \
    themedaemon/mlogicalvalues.cpp \
    themedaemon/msystemdirectories.cpp \
    themedaemon/mthemedaemonprotocol.cpp \
    mdeclarativemousefilter.cpp \
    mdeclarativeimattributeextension.cpp \
    mdeclarativeimobserver.cpp \
    shadereffectitem/shadereffectitem.cpp \
    shadereffectitem/shadereffectitemnull.cpp \
    shadereffectitem/3d/qmatrix4x4stack.cpp \
    shadereffectitem/3d/qglnamespace.cpp \
    shadereffectitem/3d/qglattributevalue.cpp \
    shadereffectitem/3d/qglattributedescription.cpp \
    shadereffectitem/3d/qcustomdataarray.cpp \
    shadereffectitem/3d/qcolor4ub.cpp \
    shadereffectitem/3d/qarray.cpp \
    shadereffectitem/utilities.cpp \
    shadereffectitem/shadereffectsource.cpp \
    shadereffectitem/shadereffectsourcenull.cpp \
    shadereffectitem/shadereffect.cpp \
    shadereffectitem/geometry.cpp

HEADERS += \
    mbatteryinfo.h \
    mcellinfo.h \
    mnetworkinfo.h \
    mnetworkinfo_p.h \
    mdeclarativescreen.h \
    mdeclarativeclipboard.h \
    mdialogstatus.h \
    msnapshot.h \
    mpagestatus.h \
    minversemousearea.h \
    mdeclarative.h \
    mdeclarativeinputcontext.h \
    mdeclarativeimageprovider.h \
    mdeclarativeimplicitsizeitem.h \
    mdeclarativemaskeditem.h \
    mscrolldecoratorsizer.h \
    mtexttranslator.h \
    mthemedaemon.h \
    mthemeplugin.h \
    mwindowstate.h \
    mwindowstate_p.h \
    mx11wrapper.h \
    themedaemon/mabstractthemedaemonclient.h \
    themedaemon/mlocalthemedaemonclient.h \
    themedaemon/mthemedaemonprotocol.h \
    mdeclarativemousefilter.h \
    mdeclarativeimattributeextension.h \
    mdeclarativeimobserver.h \
    shadereffectitem/shadereffectitem.h \
    shadereffectitem/shadereffectitemnull.h \
    shadereffectitem/3d/qt3dglobal.h \
    shadereffectitem/3d/qmatrix4x4stack_p.h \
    shadereffectitem/3d/qmatrix4x4stack.h \
    shadereffectitem/3d/qglnamespace.h \
    shadereffectitem/3d/qglattributevalue.h \
    shadereffectitem/3d/qglattributedescription.h \
    shadereffectitem/3d/qcustomdataarray.h \
    shadereffectitem/3d/qcolor4ub.h \
    shadereffectitem/3d/qarray.h \
    shadereffectitem/utilities.h \
    shadereffectitem/shadereffectsource.h \
    shadereffectitem/shadereffectsourcenull.h \
    shadereffectitem/shadereffect.h \
    shadereffectitem/geometry.h \
    shadereffectitem/glfunctions.h \
    i18n/mlocalewrapper.h \
    mpageorientation.h \
    mtoolbarvisibility.h

QML_FILES = \
        qmldir \
        ApplicationWindow.qml \
        style/ApplicationWindowStyle.qml \
        BusyIndicator.qml \
        style/BusyIndicatorStyle.qml \
        Button.qml \
        style/ButtonStyle.qml \
        style/GroupButtonStyle.qml \
        ButtonColumn.qml \
        ButtonRow.qml \
        ButtonGroup.js \
        BasicRow.qml \
        BasicRow.js \
        CheckBox.qml \
        CommonDialog.qml \
        style/CheckBoxStyle.qml \
        ContextMenu.qml \
        style/ContextMenuStyle.qml \
        Dialog.qml \
        Utils.js \
        EditBubble.js \
        EditBubble.qml \
        EditBubbleButton.qml \
        style/EditBubbleStyle.qml \
        style/EditBubbleButtonStyle.qml \
        style/DialogStyle.qml \
        style/QueryDialogStyle.qml \
        style/SelectionDialogStyle.qml \
        style/MenuStyle.qml \
	style/WindowStyle.qml \
	style/StatusBarStyle.qml \
        Fader.qml \
        Label.qml \
        style/LabelStyle.qml \
        AbstractMenu.qml \
        Menu.qml \
        MenuItem.qml \
        MenuLayout.qml \
        style/MenuItemStyle.qml \
        MouseAreaDebug.qml \
        NotificationBanner.qml \
        Page.qml \
        PageStack.qml \
        PageStack.js \
        PageStackWindow.qml \
	style/PageStackWindowStyle.qml \
        Popup.qml \
        QueryDialog.qml \
        SelectionDialog.qml \
        MultiSelectionDialog.qml \
        ProgressBar.qml \
        style/ProgressBarStyle.qml \
        RadioButton.qml \
        style/RadioButtonStyle.qml \
        ScrollDecorator.qml \
        style/ScrollDecoratorStyle.qml \
        ScrollDecoratorSizer.qml \
        SectionScroller.qml \
        style/SectionScrollerStyle.qml \
        SectionScroller.js \
        SectionScrollerLabel.qml \
        StatusBar.qml \
        TextArea.qml \
        style/TextAreaStyle.qml \
        TextField.qml \
        style/TextFieldStyle.qml \
        TabButton.qml \
        TabBarLayout.qml \
        style/TabButtonStyle.qml \
        style/Style.qml \
        TabGroup.qml \
        ToolBar.qml \
        ToolBarLayout.js \
        ToolBarLayout.qml \
        style/ToolBarStyle.qml \
        ToolButton.qml \
        style/ToolButtonStyle.qml \
        ToolItem.qml \
        ToolIcon.qml \
        ToolButtonRow.qml \
        style/ToolButtonRowStyle.qml \
        style/ToolItemStyle.qml \
        Sheet.qml \
        style/SheetStyle.qml \
        SheetButton.qml \
        style/SheetButtonStyle.qml \
        style/SheetButtonAccentStyle.qml \
        Slider.qml \
        style/SliderStyle.qml \
        SliderTemplate.qml \
        Switch.qml \
        style/SwitchStyle.qml \
        UIConstants.js \
        Window.qml \
        SipSimulator.qml \
        SoftwareInputPanel.qml \
        TextAreaHelper.js \ 
        MultiSelectionDialog.js \
        Magnifier.qml \
        Magnifier.js
