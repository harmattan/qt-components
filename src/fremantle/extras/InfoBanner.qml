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

import QtQuick 1.1
import com.nokia.meego 1.0
import "constants.js" as UI

ImplicitSizeItem {
    id: root

    /*
     * Property: iconSource
     * [url] The path to the icon image
     */
    property url iconSource: ""

    /*
     * Property: text
     * [string] Text to be displayed in InfoBanner
     */
    property alias text: text.text

    /*
     * Property: timerEnabled
     * [bool=true] Enable/disable timer that dismisses InfoBanner
     */
    property bool timerEnabled: true

    /*
     * Property: timerShowTime
     * [int=3000ms] For setting how long InfoBanner stays visible to user before being dismissed
     */
    property alias timerShowTime: sysBannerTimer.interval

    /*
     * Property: topMargin
     * [int=8 pix] Allows user to customize top margin if needed
     */
    property alias topMargin: root.y

    /*
     * Property: leftMargin
     * [int=8 pix] Allows user to customize left margin if needed
     */
    property alias leftMargin: root.x
    /* whether banner should respond to mouse events */
    property bool interactive : false

    property Style platformStyle: InfoBannerStyle {}

    signal clicked
    /*
     * Function: show
     * Show InfoBanner
     */
    function show() {
        root.parent = internal.rootObject();
        internal.setZOrder();
        background.source = root.interactive ?
                    platformStyle.backgroundInteractive : platformStyle.backgroundNormal //"image://theme/meegotouch-notification-system-background"

        stateGroup.state = "Visible"
        if (root.timerEnabled)
            sysBannerTimer.restart();
    }

    /*
     * Function: hide
     * Hide InfoBanner
     */
    function hide() {
        stateGroup.state = "Hidden"
    }

    implicitHeight: internal.getBannerHeight()
    implicitWidth: internal.getBannerWidth()
    x:8; y:8
    scale: 0

    BorderImage {
        id: background
        anchors.fill: root
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
        border { left: UI.INFO_BANNER_BORDER_MARGIN; top:UI.INFO_BANNER_BORDER_MARGIN;
            right: UI.INFO_BANNER_BORDER_MARGIN; bottom: UI.INFO_BANNER_BORDER_MARGIN }
        opacity: UI.INFO_BANNER_OPACITY
    }

    Image {
        id: image
        anchors { left: parent.left; leftMargin: UI.INFO_BANNER_INDENT_DEFAULT;
            top: parent.top; topMargin: UI.INFO_BANNER_INDENT_DEFAULT }
        source: root.iconSource
        visible: root.iconSource != ""
    }

    Text {
        id: text
        width: internal.getTextWidth()
        anchors { left: (image.visible ? image.right : parent.left); leftMargin: (image.visible ? UI_INFO_BANNER_INDENT_MEDIUM : UI.INFO_BANNER_INDENT_DEFAULT);
            top: parent.top; topMargin: internal.getTopMargin(); bottom: parent.bottom }
        wrapMode: Text.Wrap
        verticalAlignment: Text.AlignHCenter
        font {
            family: platformStyle.fontFamily
            pixelSize: platformStyle.fontPixelSize
            letterSpacing: platformStyle.fontLetterSpacing
        }
        //{QTQUICK1.1
        //maximumLineCount: 3
        //}
        elide: Text.ElideRight
        color: mouseArea.pressed ? platformStyle.pressedTextColor : platformStyle.textColor
    }

    QtObject {
        id: internal

        function rootObject() {
            var next = root.parent
            while (next && next.parent)
                next = next.parent
            return next
        }

        function getBannerHeight() {
            if (image.visible) {
                if (text.lineCount <= 2)
                    return 80;
                else
                    return 106;
            } else {
                if (text.lineCount <= 1)
                    return 64;
                else if (text.lineCount <= 2)
                    return 80;
                else
                    return 106;
            }
        }

        function getBannerWidth() {
            return parent.width - root.x*2;
        }

        function getTopMargin() {
            if (text.lineCount <= 1 && !image.visible) {
                // If there's only one line of text and no icon image, top and bottom margins are equal.
                return (root.height-text.paintedHeight)/2;
            } else {
                // In all other cases, top margin is 4 px more than bottom margin.
                return (root.height-text.paintedHeight)/2 + 2;
            }
        }

        function getTextWidth() {
            // 46(32 when there's no icon) is sum of all margins within banner. root.x*2 is sum of margins outside banner.
            // Text element width is dertermined by substracting parent width(screen width) by all the margins and
            // icon width(if applicable).
            return image.visible ? (parent.width-root.x*2-46-image.width) : (parent.width-root.x*2-32);
        }

        function getScaleValue() {
            // When banner is displayed, as part of transition effect, it'll first be enlarged to the point where its width
            // is equal to screen width. root.x*2/root.width calculates the amount of expanding required, where root.x*2 is
            // equal to screen.displayWidth minus banner.width
            return root.x*2/root.width + 1;
        }

        function setZOrder() {
            if (root.parent) {
                var maxZ = 0;
                var siblings = root.parent.children;
                for (var i = 0; i < siblings.length; ++i)
                    maxZ = Math.max(maxZ, siblings[i].z);
                root.z = maxZ + 1;
            }
        }
        function press() {
            if (root.interactive) {
                background.source = platformStyle.backgroundPressed;
            }
        }

        function release() {
            if (root.interactive)
                background.source = platformStyle.backgroundReleased;
        }

        function click() {
            if (root.interactive) {
                root.clicked();
            }
        }
    }

    Timer {
        id: sysBannerTimer
        repeat: false
        running: false
        interval: platformStyle.dismissTimer
        onTriggered: hide()
    }

    MouseArea {
        anchors.fill: parent
        onPressed: if (root.interactive) stateGroup.state = "Pressed"
        onReleased: if (root.interactive) {
                        if (stateGroup.state != "Cancelled" && stateGroup.state != "Hidden")
                            stateGroup.state = "Released";
                    } else {
                        stateGroup.state = "Hidden";
                    }
        onPressAndHold: if (root.interactive) {
                            if (sysBannerTimer.running) sysBannerTimer.stop();
                            stateGroup.state = "PressAndHold";
                        }
        onExited: if (root.interactive) {
                      if (stateGroup.state == "Pressed") {
                          stateGroup.state = "Cancelled";
                      } else if (stateGroup.state == "PressAndHold") {
                          stateGroup.state = "Cancelled";
                          if (sysBannerTimer.interval) sysBannerTimer.restart();
                      } else stateGroup.state = "Hidden"
                  }
        onClicked: hide()
    }
    StateGroup {
        id: stateGroup
        state: "Hidden"

        states: [
            State {
                name: "Visible"
                PropertyChanges { target: root; scale: internal.getScaleValue() }
            },
            State {
                name: "Hidden"
                PropertyChanges { target: root; scale: 0 }
            },
            State { name: "Pressed" },
            State { name: "PressAndHold" },
            State { name: "Released" },
            State { name: "Cancelled" }
        ]
        transitions: [
            Transition {
                to: "Pressed"
                ScriptAction { script: internal.press(); }
            },
            Transition {
                from: "Pressed"; to: "Released"
                ScriptAction { script: internal.release(); }
                ScriptAction { script: internal.click(); }
            },
            Transition {
                from: "PressAndHold"; to: "Released"
                ScriptAction { script: internal.release(); }
                ScriptAction { script: internal.click(); }
            },
            Transition {
                to: "Cancelled"
                ScriptAction { script: internal.release(); }
            },
            Transition {
                from: "Hidden"; to: "Visible"
                SequentialAnimation {
                    NumberAnimation { target: root; property: "scale"; from: 0; to: internal.getScaleValue(); duration: 200; easing.type: Easing.OutQuad}
                    NumberAnimation { target: root; property: "scale"; from: internal.getScaleValue(); to: 1; duration: 200 }
                }
            },
            Transition {
                to: "Hidden"
                NumberAnimation { target: root; properties: "scale"; to: 0; duration: 200; easing.type: Easing.InExpo }
            }
        ]
    }
}

