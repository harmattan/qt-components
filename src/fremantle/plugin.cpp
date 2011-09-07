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

#include <QtDeclarative>
#include <QApplication>

//Add here type headers
#include "mdeclarativeimageprovider.h"
#include "mdeclarativeimattributeextension.h"
#include "mdeclarativeimobserver.h"
#include "mdeclarativeimplicitsizeitem.h"
#include "mdeclarativeinputcontext.h"
#include "mdeclarativemaskeditem.h"
#include "mdeclarativemousefilter.h"
#include "mdeclarativescreen.h"
#include "minversemousearea.h"
#include "mscrolldecoratorsizer.h"
#include "msnapshot.h"
#include "mtexttranslator.h"
#include "mthemeplugin.h"
#include "mwindowstate.h"
#include "sdeclarative.h"

#include "plugin.h"

#include <QDeclarativePropertyMap>
#include <QFont>

#ifdef HAVE_SHADER
# include "shadereffectitem/shadereffectitem.h"
# include "shadereffectitem/shadereffectsource.h"
#else
# include "shadereffectitem/shadereffectitemnull.h"
# include "shadereffectitem/shadereffectsourcenull.h"
#endif

static const int VERSION_MAJOR = 1;
static const int VERSION_MINOR = 0;

void FremantlePlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_ASSERT(uri == QLatin1String("org.maemo.fremantle") || uri == QLatin1String("Qt.labs.components.native"));

        QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
        QDeclarativeContext *context = engine->rootContext();

	// If plugin was initilized once, do not initialize it again
        if(engine->imageProvider(QLatin1String("theme"))) {
	    return;
	}

        // Intentionally override possible older version of the plugin.
        context->setProperty("ComponentsFlavor", "fremantle");
        context->setProperty("ComponentsVersionMajor", VERSION_MAJOR);
        context->setProperty("ComponentsVersionMinor", VERSION_MINOR);

        // SetUp Theme image provider
        engine->addImageProvider(QLatin1String("theme"), new MDeclarativeImageProvider);

        // Register global screen instance
        context->setContextProperty("screen", MDeclarativeScreen::instance());
        qmlRegisterUncreatableType<MDeclarativeScreen>(uri, 1, 0, "Screen", "");

        // Theme support
        context->setContextProperty("theme", new MThemePlugin);
        qmlRegisterUncreatableType<MThemePlugin>(uri, 1, 0, "Theme", "");

        // Register global ImputContext support (Keyboard Stuff)
        context->setContextProperty("inputContext", new MDeclarativeInputContext);
        qmlRegisterUncreatableType<MDeclarativeInputContext>(uri, 1, 0, "InputContext", "");

        SDeclarative *declarative = new SDeclarative(context);
        context->setContextProperty("maemo", declarative);

        context->setContextProperty("platformWindow", MWindowState::instance());
        qmlRegisterUncreatableType<MWindowState>(uri, 1, 0, "WindowState", "");

        context->setContextProperty("textTranslator", new MTextTranslator);
        qmlRegisterUncreatableType<MTextTranslator>(uri, 1, 0, "TextTranslator", "");

        // Disable cursor blinking + make double tapping work the way it is done in lmt.
        QApplication *app = qobject_cast<QApplication*>(QApplication::instance());
        if (app) {
            app->setCursorFlashTime(0);
            app->setDoubleClickInterval(MEEGOTOUCH_DOUBLETAP_INTERVAL);
        } 
}

void FremantlePlugin::registerTypes(const char *uri) {
        Q_ASSERT(uri == QLatin1String("org.maemo.fremantle") || uri == QLatin1String("Qt.labs.components.native"));

        // Add here custom types
        qmlRegisterType<MSnapshot>(uri, 1, 0, "Snapshot");

        qmlRegisterUncreatableType<SPageStatus>(uri, 1, 0, "PageStatus", "");
        qmlRegisterUncreatableType<MDialogStatus>(uri, 1, 0, "DialogStatus", "");
        qmlRegisterUncreatableType<MWindowState>(uri, 1, 0, "WindowState","");
        qmlRegisterUncreatableType<SPageOrientation>(uri, 1, 0, "PageOrientation", "");
        qmlRegisterUncreatableType<MToolBarVisibility>(uri, 1, 0, "ToolBarVisibility", "");
        qmlRegisterUncreatableType<MTextTranslator>(uri, 1, 0, "TextTranslator", "");

        // Custom primitives
        qmlRegisterType<MDeclarativeImplicitSizeItem>(uri, 1, 0, "ImplicitSizeItem");
        qmlRegisterType<MDeclarativeMaskedItem>(uri, 1, 0, "MaskedItem");
        qmlRegisterType<MDeclarativeFocusScope>(uri, 1, 0, "FocusScope");
        qmlRegisterType<MInverseMouseArea>(uri, 1, 0, "InverseMouseArea");
        qmlRegisterType<MDeclarativeMouseFilter>(uri, 1, 0, "MouseFilter");
        qmlRegisterType<MDeclarativeMouseEvent>(uri, 1, 0, "MMouseEvent");
        qmlRegisterType<MDeclarativeIMAttributeExtension>(uri, 1, 0, "SipAttributes");
        qmlRegisterType<MDeclarativeIMObserver>(uri, 1, 0, "InputMethodObserver");

        qmlRegisterType<MScrollDecoratorSizer>(uri, 1, 0, "ScrollDecoratorSizerCPP");

        // shader effect item (to be removed when supported in QML)
#ifdef HAVE_SHADER
        qmlRegisterType<ShaderEffectItem>(uri, 1, 0, "ShaderEffectItem");
        qmlRegisterType<ShaderEffectSource>(uri, 1, 0, "ShaderEffectSource");
#else
        qmlRegisterType<ShaderEffectItemNull>(uri, 1, 0, "ShaderEffectItem");
        qmlRegisterType<ShaderEffectSourceNull>(uri, 1, 0, "ShaderEffectSource");
#endif
}

#include "moc_plugin.cpp"

Q_EXPORT_PLUGIN2(fremantleplugin, FremantlePlugin);
