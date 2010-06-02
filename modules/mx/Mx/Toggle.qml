/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project on Trolltech Labs.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
import Qt 4.7

Item {
    id: toggle

    property bool active: state == "activated"
    property alias tooltipText: tooltip.text;

    width: 105
    height: 39

    Image {
        source: "images/toggle-background.png"
        anchors.fill: parent
        visible: !active
    }

    Image {
        source: "images/toggle-background-active.png"
        anchors.fill: parent
        visible: active
    }

    MouseArea {
        id: backMouse
        anchors.fill: parent
        hoverEnabled: tooltip.text
        onClicked: { active = !active; }
        onPressed: { tooltip.pressDismiss = true; }
    }

    Image {
        id: handle
        x: 4
        y: 4
        source: "images/toggle-handle.png"

        MouseArea {
            id: handleMouse

            property int lastPosition;

            // Usually lastPosition was equal to mouse.x during onRelease.
            property int secondLastPosition;

            property bool dragged;
            property bool userPressed;

            anchors.fill: parent
            hoverEnabled: tooltip.text
            drag.target: handle
            drag.axis: "XAxis"
            drag.minimumX: 4
            drag.maximumX: toggle.width - handle.width - 4

            onPressed: {
                tooltip.pressDismiss = true;
                userPressed = true;
                dragged = false;
                lastPosition = handle.x;
                secondLastPosition = handle.x;
            }

            onPositionChanged: {
                dragged = true;
                secondLastPosition = lastPosition;
                lastPosition = handle.x;
            }

            onReleased: {
                // Was just clicked
                if (!dragged) {
                    active = !active;
                    userPressed = false;
                    return;
                }

                // Edges or last movement
                if (handle.x == drag.minimumX || handle.x < secondLastPosition) {
                    active = false;
                } else if (handle.x == drag.maximumX || handle.x > secondLastPosition) {
                    active = true;
                } else {
                    active = handle.x > (toggle.width - handle.width - 4) ? true : false;
                }

                dragged = false;
                userPressed = false;
            }
        }
    }

    TooltipLoader {
        id: tooltip;
        anchors.fill: parent;

        property bool pressDismiss: false;
        property bool containsMouse: backMouse.containsMouse
                                  || handleMouse.containsMouse;

        // When the mouse moves from one MouseArea to another we get two
        // mouseChanged events (to false and back to true). To debounce that
        // we use a 10 msec timer.
        onContainsMouseChanged: {
            if (!containsMouse)
                dismissTimer.start();
        }
        Timer {
            id: dismissTimer;
            interval: 10;
            onTriggered: {
                // Condition for "onExited" event
                if (!tooltip.containsMouse)
                    tooltip.pressDismiss = false;
            }
        }

        shown: containsMouse && !pressDismiss;
    }

    states: [
        State {
            name: "deactivated"
            when: !active && !handleMouse.userPressed
            PropertyChanges {
                target: handle
                restoreEntryValues: false
                x: 4
            }
        },
        State {
            name: "activated"
            when: active && !handleMouse.userPressed
            PropertyChanges {
                target: handle
                restoreEntryValues: false
                x: toggle.width - handle.width - 4
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "deactivated"
            PropertyAnimation {
                properties: "x"
                duration: 300
            }
        },
        Transition {
            from: "*"
            to: "activated"
            PropertyAnimation {
                properties: "x"
                duration: 300
            }
        }
    ]
}
