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

#include "fpsdeclarativeview.h"

#define CHECK_VER(v, V) v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < V
#define MAX(a, b) ((a > b) ? a : b)

int main(int argc, char **argv)
{
    const QString qrc = "qrc:/ssu/main.qml";

    //Check qt version if qt > 4.7.4, run with opengl graphics
    const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
    if (CHECK_VER(v, 0x040704)) {
		qrc = "qrc:/cssu/main.qml";
        QApplication::setGraphicsSystem("opengl");
    }
    
    QApplication app(argc, argv);
    app.setProperty("NoMStyle", true);

  	FPSDeclarativeView window;

    QDir::setCurrent(app.applicationDirPath());
    window.setSource(QUrl(qrc));
    QObject::connect((QObject*)window.engine(), SIGNAL(quit()), &app, SLOT(quit()));

#ifdef __arm__
    window.showFullScreen();
#else
    (MAX(window.initialSize().width(), window.initialSize().height()) < 1024) ? window.showFullScreen() : window.show();
#endif

    return app.exec();
}

#undef MAX
#undef CHECK_VER
