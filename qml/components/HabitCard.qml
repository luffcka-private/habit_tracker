import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle{
    id: root
    implicitHeight: 80

    color: palette.base
    radius: 8
    border.color: palette.mid
    border.width: 1

    property string habitName: "Название привычки"
    property string habitStreak: "0 дней"
    property bool isDone: false

    RowLayout{
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        Rectangle {
            id: checkCircle
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            radius: 20

            color: root.isDone ? "#4caf50" : "transparent"
            border.color: root.isDone ? "#4caf50" : palette.mid
            border.width: 2

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    root.isDone = !root.isDone
                }
            }

            Text {
                anchors.centerIn: parent
                text: "✔"
                font.pixelSize: 20
                color: "white"
                visible: root.isDone
            }
        }
            ColumnLayout{
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                spacing: 5

                Label{
                    text: root.habitName
                    font.pixelSize: 18
                    font.bold: true
                    color: palette.windowText
                    elide: ElideRight
                    Layout.fillWidth: true
                }

                Label{
                    text: "Дней подряд: " + root.habitStreak
                    font.pixelSize: 14
                    color: palette.windowText
                }
            }
        }
    }
