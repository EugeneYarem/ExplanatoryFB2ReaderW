import QtQuick 2.0
import QtQuick.Controls 2.3

MenuItem {
    id: menuItem

    property alias shortcutAnchors : text2.anchors

    icon { height: 20; width: 20; color: "transparent" }
    text: ""

    Text {
        id: text1

        anchors.left: parent.left
        anchors.leftMargin: parent.icon.width + 10
        anchors.verticalCenter: parent.verticalCenter

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        font.family: "Verdana"
        font.pointSize: 9

        text: parent.action.text
    }

    Text {
        id: text2

        anchors.right: parent.right
        anchors.rightMargin: 7
        anchors.verticalCenter: parent.verticalCenter

        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter

        font.family: "Verdana"
        font.pointSize: 9
        font.underline: true

        text: parent.action.shortcut
    }

    states: [
        State {
            name: "enabled"
            when: enabled == true

            PropertyChanges {
                target: menuItem
                opacity: 1
            }
        },
        State {
            name: "disenabled"
            when: enabled == false

            PropertyChanges {
                target: menuItem
                opacity: 0.5
            }
        }
    ]
}
