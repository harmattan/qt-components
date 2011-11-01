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

import QtQuick 1.0
import org.maemo.fremantle 1.0
import org.maemo.extras 1.0

Page {
    id: container
    anchors.margins: UiConstants.DefaultMargin
    tools: commonTools

    Item {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Button {
            id: tumblerButton
            anchors.top: parent.top
            anchors.topMargin: 10
            text: "Launch Dialog"
            width: 206
            height: 35
            onClicked: launchDialog()
        }

        Button {
            id: todayButton
            anchors.top: tumblerButton.bottom
            anchors.topMargin: 10
            text: "Launch Today"
            width: 206
            height: 35
            onClicked: launchDialogToToday()
        }

        Button {
            id: dateButton
            anchors.top: todayButton.bottom
            anchors.topMargin: 10
            text: "Launch Oct 22, 10"
            width: 206
            height: 35
            onClicked: launchDate(2010, 10, 22)
        }

        Label {
            id: result
            anchors.top: dateButton.bottom
            anchors.topMargin: 10
        }
    }

    function launchDialog() {
        console.log("main::launchDialog");
        tDialog.open();
    }

    function launchDialogToToday() {
        console.log("main::launchDialogToToday");
        var d = new Date();
        tDialog.year = d.getFullYear();
        tDialog.month = d.getMonth() + 1; // month is 0 based in Date()
        tDialog.day = d.getDate();
        tDialog.open();
    }

    function launchDate(year, month, date) {
        console.log("main::launchDialog");
        tDialog.minimumYear = year - 10;
        tDialog.maximumYear = year + 10;
        tDialog.year = year;
        tDialog.month = month;
        tDialog.day = date;
        tDialog.open();
    }

    function callbackFunction() {
        result.text = tDialog.year + " " + tDialog.month + " " + tDialog.day
    }

    DatePickerDialog {
        id: tDialog
        titleText: "Date of birth"
        onAccepted: callbackFunction()
    }
}