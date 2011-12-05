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
import "." 1.0

import "UIConstants.js" as UI

Item {
    id: root

    anchors.top: parent.top
    width: parent ? parent.width : 0
    height: background.height

    // Set StatusBar in the Z index required by Window
    z: UI.STATUS_BAR_Z_INDEX

    // Whether status bar should be showed
    property bool showStatusBar: true

    // Extended API (fremantle only)
    property bool showHelp: true

    // Styling for the StatusBar
    property Style platformStyle: StatusBarStyle {}

    //Statusbar background
    BorderImage {
        id: background
        width: root.width
        source: platformStyle.background

        // battery indicator
        Item {
            id: batteryIndicator
	    height: parent.height
            property int animatedLevel : 0

	    anchors {
	        left: parent.left
                leftMargin: platformStyle.paddingSmall
	    }

            Image {
                id: indicator
                source: platformStyle.batteryFrames + (maemo.batteryInfo.charging ?
                            (parent.animatedLevel > 0 ? parent.animatedLevel : "-low")    :
                            (maemo.batteryInfo.batteryLevel > 0 ? maemo.batteryInfo.batteryLevel : "-verylow"))

                anchors.verticalCenter: parent.verticalCenter
            }

            NumberAnimation {
                id: batteryChargingAnimation
                running: maemo.batteryInfo.charging && root.visible && platformWindow.active
                target: batteryIndicator; property: "animatedLevel"
                from: 0; to: platformStyle.batteryLevels; duration: platformStyle.batteryPeriod
                loops: Animation.Infinite
            }
        }

        // clock
        Text {
            id: timeItem
            // Clock properties
            text: maemo.currentTime
            color: platformStyle.clockColor
            horizontalAlignment: Text.AlignRight

            // Clock position
            width: Math.round(parent.height * 44 / 26)

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: platformStyle.paddingSmall
            }

            font {
                pixelSize: priv.contentHeight
                weight: Font.Light
            }
        }
    }

    Rectangle {
        id: help_background
        width: parent.width; height: parent.height
        color: "black"
        opacity: 0.0
        enabled: showHelp

        Item {
            id: help_contents
            opacity: 0.0
            anchors.fill: parent

            Image {
                source: platformStyle.homeButton
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: platformStyle.paddingSmall
                }
            }

            Image {
                source: platformStyle.closeButton
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: platformStyle.paddingSmall
                }
            }

            Label {
                color: UI.COLOR_INVERTED_FOREGROUND
                anchors.centerIn: parent
                text: qsTr("'Tap' to switch or close")
            }
        }
    }

    Timer{
        id: stimer
        interval: platformStyle.showHelpDuration; running: showHelp; repeat: false;
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onPressed: {
            //FIXME: Implement a StatusApplet. See symbian Belle for details
            console.log("Area clicked")
        }
    }

    QtObject {
        id: priv

        property int contentHeight: Math.round(root.height * 18 / 26)
        property int paddingSmallOneQuarter: Math.round(platformStyle.paddingSmall / 4)
        property int paddingSmallThreeQuarters: Math.round(platformStyle.paddingSmall * 3 / 4)

        function signalWidthPercentage(signalStrength) {
            if (signalStrength < 10)
                return 0;
            else if (signalStrength < 20)
                return 1/5;
            else if (signalStrength < 30)
                return 2/5;
            else if (signalStrength < 60)
                return 3/5;
            else if (signalStrength < 100)
                return 4/5;
            else
                return 1;
        }
    }

    states: [
        State {
            name: "hidden"
            when: showStatusBar == false
            PropertyChanges {target: root; anchors.topMargin: -root.height; visible: false}

        },
        State {
            name: "start"
            when: stimer.running == true
            PropertyChanges {target: root; height: platformStyle.paddingSmall * 8}
            PropertyChanges {target: help_background; opacity: 1.0 }
            PropertyChanges {target: help_contents; opacity: 1.0; }
        }
    ]

    transitions: [
        // Transition between active and inactive states.
        Transition {
            from: ""; to: "hidden"; reversible: true
            ParallelAnimation {
                PropertyAnimation {properties: "anchors.topMargin"; easing.type: Easing.InOutExpo;  duration: platformStyle.visibilityTransitionDuration }
                PropertyAnimation { target: statusBar; properties: "visible"; }
            }
        },
        Transition {
            from: "start"; to: ""; reversible:  false
            SequentialAnimation {
                PropertyAnimation {targets: help_contents; properties: "opacity"; easing.type: Easing.InOutExpo;  duration: platformStyle.helpTransitionDuration }
                PauseAnimation    {duration:  platformStyle.visibilityTransitionDuration}
                PropertyAnimation {targets: root; properties:"height"; easing.type: Easing.InOutExpo;  duration: platformStyle.helpTransitionDuration }
                PauseAnimation    {duration:  platformStyle.visibilityTransitionDuration}
                PropertyAnimation {targets: help_background; properties: "opacity"; easing.type: Easing.InOutExpo;  duration: platformStyle.helpTransitionDuration }
            }
        }
    ]
}

