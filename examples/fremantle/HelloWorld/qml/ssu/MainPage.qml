import Qt 4.7
import org.maemo.fremantle 1.0

Page {
    id: mainPage
    tools: commonTools

    Button{
        id: button
        anchors.centerIn: parent
        text: qsTr("Click here!")
        onClicked: label.visible=true
    }
    Label {
        id: label
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: button.bottom
        anchors.topMargin: 10
        text: qsTr("Hello world!")
        visible: false
    }
    TextField{
        id: input
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        placeholderText: qsTr("Insert some text here")
    }
}
