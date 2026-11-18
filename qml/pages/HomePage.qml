import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import HabitTracker.Presentation 1.0

Page {
    id: rootPage
    title: qsTr("Главная")

    background: Rectangle {
        color: palette.window
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width - 60
            x: 30
            spacing: 20

            Label {
                text: "С чего начнем?"
                font.pixelSize: 24
                font.bold: true
                color: palette.windowText

                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.topMargin: 30
            }

            Label {
                text: "Твои ежедневные цели:"
                font.pixelSize: 16
                color: palette.windowText
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.bottomMargin: 10
            }

            HabitCard {
                Layout.fillWidth: true
                habitName: "Выпить стакан воды"
                habitStreak: "3"
                isDone: true
            }

            HabitCard {
                Layout.fillWidth: true
                habitName: "Тренировка 30 минут"
                habitStreak: "4"
                isDone: false
            }
        }
    }
}