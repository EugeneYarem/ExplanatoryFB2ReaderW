import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ToolButton {
    icon { height: 20; width: 20; color: "transparent" }
    Layout.fillWidth: false;
    background: Rectangle {
        anchors.centerIn: parent
        height: parent.parent.height
        width: parent.width
        color: parent.pressed ? "#d0d0d0" : (parent.hovered ? "white" : "#e0e0e0") //parent.hovered ? "#d0d0d0" : "#e0e0e0"
    }
}
