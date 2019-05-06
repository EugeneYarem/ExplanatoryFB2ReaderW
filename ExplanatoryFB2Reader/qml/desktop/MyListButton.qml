import QtQuick 2.0
import QtQuick.Controls 2.3

Button {
    height: 36
    width: height

    bottomPadding: 2
    leftPadding: bottomPadding
    rightPadding: bottomPadding
    topPadding: bottomPadding
    spacing: 0

    display: AbstractButton.IconOnly
    icon { height: 32; width: height; color: "transparent" }

    background: Rectangle {
        anchors.centerIn: parent
        height: parent.height
        width: parent.width

        border.color: "grey"
        border.width: 0
        color: parent.pressed ? "#d0d0d0" : ( (parent.hovered || parent.focus) ? "#e0e0e0" : "white"/*Qt.rgba(141 / 255, 209 / 255, 211 / 255, 0.7)*/) //parent.hovered ? "#d0d0d0" : "#e0e0e0"
        //radius: 7
    }
}

/*
Rectangle {
    property alias icon : icon.source

    height: 30
    width: height

    color: "white"
    border.color: "black"
    border.width: 1

    Image {
        id: icon

        height: 28
        width: height

        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        anchors.topMargin: 1
    }
}
*/
