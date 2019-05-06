import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.2

MenuBar {
    property Action booksList : booksListAction
    property Action openBook : openBookAction
    property Action createBookmark : createBookmarkAction

    height: 40
    font.pointSize: 12
    font.family: "Verdana"
    width: parent.width
    /*delegate: MenuBarItem {
        Rectangle {
            anchors.fill: parent
            color: "red"
        }
    }*/

    Menu {
        id: mainMenu
        title: qsTr("Головне меню")

        MyMenuItem {
            //id: menuItem1
            action: Action {
                id: booksListAction
                shortcut: "Ctrl+L"
                text: qsTr("Переглянути книги")
            }
            icon.source: "qrc:/images/library.png"
        }

        MenuSeparator {}

        MyMenuItem {
            //id: menuItem2
            action: Action {
                id: exitAction
                shortcut: "Ctrl+Q"
                text: qsTr("Вихід з програми")
            }
            icon.source: "qrc:/images/close.png"
        }
    }

    Menu {
        id: fileMenu
        title: qsTr("Файл")

        MyMenuItem {
            //id: menuItem3

            FileDialog {
                id: fileDialog

                folder: shortcuts.home
                nameFilters: "Books (*.fb2)"
                title: qsTr("Будь ласка, оберіть книгу")

                onAccepted: {
                    console.log("You chose: " + fileDialog.fileUrl)
                    //applicationWindow.bookView.loadHtml("http://www.example.com/documents/overview.html"/*fb2Reader.parseBook(fileDialog.fileUrl)*/, "http://www.example.com/documents/")
                    //applicationWindow.bookView.text = fb2Reader.parseBook(fileDialog.fileUrl)
                    applicationWindow.bookView.parseBook(fileDialog.fileUrl)
                    //console.log(fb2Reader.parseBook(fileDialog.fileUrl).length)
                    //Qt.quit()
                }
                onRejected: {
                    console.log("Canceled")
                    Qt.quit()
                }
            }

            action: Action {
                id: openBookAction
                shortcut: "Ctrl+O"
                text: qsTr("Відкрити книгу")
                onTriggered: fileDialog.visible = true
            }
            icon.source: "qrc:/images/openBook.png"
            shortcutAnchors.rightMargin: 10
        }

        MyMenuItem {
            //id: menuItem4
            action: Action {
                id: closeBookAction
                shortcut: "Ctrl+W"
                text: qsTr("Закрити книгу")
            }
            icon.source: "qrc:/images/closeBook.png"
        }

        //Action { id: openBookAction; text: qsTr("Вікрити книгу") }
        //Action { id: closeBookAction; text: qsTr("Закрити книгу") }
    }

    Menu {
        id: bookmarksMenu
        title: qsTr("Закладки")

        MyMenuItem {
            //id: menuItem5
            action: Action {
                id: watchBookmarksAction
                shortcut: "Ctrl+B"
                text: qsTr("Переглянути всі")
            }
            icon.source: "qrc:/images/bookmark.png"
        }

        MyMenuItem {
            //id: menuItem6
            action: Action {
                id: createBookmarkAction
                shortcut: "Ctrl+N"
                text: qsTr("Створити")
            }
            icon.source: "qrc:/images/bookmarkNew.png"
        }

        MyMenuItem {
            //id: menuItem7
            action: Action {
                id: editBookmarkAction
                enabled: false
                shortcut: "Ctrl+E"
                text: qsTr("Редагувати")
            }
            icon.source: "qrc:/images/bookmarkEdit.png"
        }

        MyMenuItem {
            //id: menuItem8
            action: Action {
                id: deleteBookmarkAction
                enabled: false
                shortcut: "Ctrl+D"
                text: qsTr("Видалити")
            }
            icon.source: "qrc:/images/bookmarkDelete.png"
        }

        //Action { id: watchBookmarksAction; text: qsTr("Переглянути закладки") }
        //Action { id: createBookmarkAction; text: qsTr("Створити закладку") }
        //Action { id: editBookmarkAction; text: qsTr("Редагувати закладку") }
        //Action { id: deleteBookmarkAction; text: qsTr("Видалити закладку") }
    }

    Menu {
        id: helpMenu
        title: qsTr("Допомога")

        MyMenuItem {
            //id: menuItem9
            action: Action {
                id: settingsAction
                shortcut: "Ctrl+S"
                text: qsTr("Налаштування")
            }
            icon.source: "qrc:/images/settings.png"
        }

        MyMenuItem {
            //id: menuItem10
            action: Action {
                id: aboutAction
                shortcut: ""
                text: qsTr("Про програму")
            }
            icon.source: "qrc:/images/about.png"
        }

        //delegate: MenuItem { font.pointSize: 9; font.family: "Verdana" }

        /*Action { id: settingsAction; text: qsTr("Налаштування") }
        Action { id: aboutAction; text: qsTr("Про програму") }*/
    }
}
