import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow {
    id: root
    width: 1280
    height: 720
    visible: true
    title: qsTr("Трекер привычек")
    color: palette.window

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                text: "☰"
                font.pixelSize: 20
                Layout.preferredWidth: 150
                onClicked: drawer.opened ? drawer.close() : drawer.open()
            }
            Label {
                //по индексу
                text: ["Главная", "Сегодня", "Календарь", "Статистика"][pageLayout.currentIndex]

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
                font.bold: true
            }
            Item { Layout.preferredWidth: 150 }
        }
    }

    Drawer {
        id: drawer
        //modal: true
        width: 150
        height: root.height - root.header.height
        y: root.header.height

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10

            Button {
                text: "Главная"
                Layout.fillWidth: true
                onClicked: {
                    pageLayout.currentIndex = 0
                    drawer.close()
                }
            }
            Button {
                text: "Сегодня"
                Layout.fillWidth: true
                onClicked: {
                    pageLayout.currentIndex = 1
                    drawer.close()
                }
            }
            Button {
                text: "Календарь"
                Layout.fillWidth: true
                onClicked: {
                    pageLayout.currentIndex = 2
                    drawer.close()
                }
            }
            Button {
                text: "Статистика"
                Layout.fillWidth: true
                onClicked: {
                    pageLayout.currentIndex = 3
                    drawer.close()
                }
            }
        }
    }

    StackLayout {
        id: pageLayout
        anchors.fill: parent
        currentIndex: 0

        HomePage {}
        TodayPage {}
        CalendarPage {}
        StatsPage {}
    }
}