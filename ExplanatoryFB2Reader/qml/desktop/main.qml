import QtQuick 2.11
//import QtQuick.Window 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtLocation 5.9
import QtQuick.Controls.Material 2.1
import fb2Reader 1.0


ApplicationWindow {
    id: applicationWindow

    property FB2Reader bookView : book

    visible: true
    width: 880
    height: 480
    title: "Explanatory FB2-Reader"

    menuBar: MyMenuBar {
        id: menuBar
    }

    header: ToolBar {
        contentHeight: 0
        font.pointSize: 10
        font.family: "Verdana"

        Material.foreground: "white"

        Rectangle {
            id: separator
            height: 1
            width: parent.width
            color: "black"
        }

        RowLayout {
            id: row
            clip: true
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.top: separator.bottom
            spacing: 0

            MyToolButton { action: menuBar.booksList; icon.source: "qrc:/images/library.png" }
            ToolSeparator {}

            MyToolButton { action: menuBar.openBook; icon.source: "qrc:/images/openBook.png" }
            ToolSeparator {}

            MyToolButton { action: Action { id: contentsAction; text: qsTr("Зміст") } icon.source: "qrc:/images/contents.png" }
            ToolSeparator {}

            MyToolButton { action: menuBar.createBookmark; icon.source: "qrc:/images/createBookmark.png"; text: qsTr("Створити закладку") }
            ToolSeparator {}

            MyToolButton { action: Action { id: defenetionAction; text: qsTr("Знайти значення") } icon.source: "qrc:/images/definition.png" }
            ToolSeparator {}

            MyToolButton { action: Action { id: translationAction; text: qsTr("Переклад") } icon.source: "qrc:/images/translation.png" }
            ToolSeparator {}
        }

        Rectangle {
            id: separator2
            anchors.top: row.bottom
            height: 1
            width: parent.width
            color: "black"
        }
    }

    ScrollView {
        id: view

        anchors.left: parent.left
        anchors.leftMargin: 236
        anchors.top: parent.top
        anchors.topMargin: 101
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 13
        anchors.right: parent.right
        anchors.rightMargin: 13

        TextArea {
            id: bookView
            width: 631
            height: 366
            FB2Reader {
                id: book
                //book: ""
            }

            text: book.book //fb2Reader.book
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            smooth: false
            readOnly: true
            renderType: Text.NativeRendering
            anchors.topMargin: 0
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            anchors.bottomMargin: 0
            anchors.fill: parent
            textFormat: Text.RichText
            /*x: 236
            y: 101
            width: 631
            height: 366
            renderType: Text.NativeRendering
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            //readOnly: true
            //text: fb2Reader.parseBook("")
            textFormat: Text.RichText//: qsTr("Text")
            font.pixelSize: 12
            //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn*/
        }
    }



    Label {
        id: label
        x: 236
        y: 57
        width: 346
        height: 20
        text: qsTr("Label")
    }

    RowLayout {
        id: rowLayout
        x: 5
        y: 57
        width: 193
        height: 40
        spacing: 2

        MyListButton {
            id: contentsListButton
            icon.source: "qrc:/images/contents.png"

            ToolTip.visible: hovered
            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
            ToolTip.text: qsTr("Відкрити зміст книги")
        }

        MyListButton {
            id: bookmarksListButton
            icon.source: "qrc:/images/listBookmarks.png"

            ToolTip.visible: hovered
            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
            ToolTip.text: qsTr("Відкрити панель закладок")
        }

        MyListButton {
            id: definitionsListButton
            icon.source: "qrc:/images/listDefinitions.png"

            ToolTip.visible: hovered
            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
            ToolTip.text: qsTr("Відкрити панель визначень")
        }

        MyListButton {
            id: translationsListButton
            icon.source: "qrc:/images/listTranslations.png"

            ToolTip.visible: hovered
            ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
            ToolTip.text: qsTr("Відкрити панель перекладів")
        }
    }

}

/*##^## Designer {
    D{i:21;anchors_height:366;anchors_width:631;anchors_x:236;anchors_y:101}
}
 ##^##*/
