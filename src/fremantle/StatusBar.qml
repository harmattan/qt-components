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

import Qt 4.7

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
        source: platformStyle.background

        property int contentHeight: Math.round(root.height * 18 / 26)

        Item {
            id: battery_indicator
            width: battery_image.width; height: parent.height

            property int animatedLevel: 0

            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingSmall
            }

            Image {
                id: battery_image
                source: platformStyle.batteryFrames + (maemo.batteryInfo.charging ?
                   (parent.animatedLevel > 0 ? parent.animatedLevel : "-low")    :
                   (maemo.batteryInfo.batteryLevel > 0 ? maemo.batteryInfo.batteryLevel : "-verylow"))
                anchors.verticalCenter: parent.verticalCenter
            }

            NumberAnimation {
                running: maemo.batteryInfo.charging && root.visible && platformWindow.active
                target: battery_indicator; property: "animatedLevel"
                from: 0; to: platformStyle.batteryLevels; duration: platformStyle.batteryPeriod
                loops: Animation.Infinite
            }
        }

        // signal indicator
        Item {
            id: signal_indicator
            width: maemo.cellInfo.offline ? signal_offline_image.width: signal_online_image.width; height: parent.height

            anchors {
                left: battery_indicator.right
                leftMargin: platformStyle.paddingSmall
            }

            // Offline indicator
            Image {
                id: signal_offline_image
                visible: maemo.cellInfo.offline
                source: platformStyle.cellStatus + maemo.cellInfo.status
                anchors.verticalCenter: parent.verticalCenter
            }

            // Signal indicator
            Image {
                id: signal_online_image
                visible: !maemo.cellInfo.offline
                source: platformStyle.cellSignalFrames + maemo.cellInfo.strength
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // Operator name
        Text {
            id: provider_indicator
            visible: !maemo.cellInfo.offline
            text: maemo.cellInfo.provider
            color: platformStyle.indicatorColor

            anchors {
                left:signal_indicator.right
                leftMargin: platformStyle.paddingSmall
                verticalCenter: parent.verticalCenter
            }

            font {
                family: platformStyle.providerFont
                pixelSize: background.contentHeight
            }
        }

        // Cell range
        Item {
            id: cellrange_indicator
            width: cellrange_image.width; height: parent.height

            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingSmall
            }

            Image {
                id: cellrange_image
                visible: !maemo.cellInfo.offline
                source: platformStyle.cellRangeMode + maemo.cellInfo.mode +
                        (maemo.cellInfo.active ? "-active" : "")
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // clock
        Text {
            id: time_indicator
            text: maemo.currentTime
            color: platformStyle.clockColor

            anchors {
                right: parent.right
                rightMargin: platformStyle.paddingSmall
                verticalCenter: parent.verticalCenter
            }

            font {
                family: platformStyle.clockFont
                pixelSize: background.contentHeight
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
                    left: parent.left
                    leftMargin: platformStyle.paddingSmall
                    verticalCenter: parent.verticalCenter
                }
            }

            Image {
                source: platformStyle.closeButton
                anchors {
                    right: parent.right
                    rightMargin: platformStyle.paddingSmall
                    verticalCenter: parent.verticalCenter
                }
            }

            Text {
                color: platformStyle.indicatorColor
                anchors.centerIn: parent

                //% "'Tap' to switch or close"
                text: textTranslator.translate("qtn_statusbar_help")

                font {
                    family: platformStyle.clockFont
                    pixelSize: background.contentHeight
                }
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

