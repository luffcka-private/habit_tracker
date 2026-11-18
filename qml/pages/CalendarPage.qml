import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: rootPage
    title: qsTr("Календарь")

    background: Rectangle {
        color: palette.window
    }

    property int currentMonth: new Date().getMonth()
    property int currentYear: new Date().getFullYear()
    property date selectedDate: new Date()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "<"
                onClicked: {
                    if (rootPage.currentMonth === 0) {
                        rootPage.currentMonth = 11;
                        rootPage.currentYear--;
                    } else {
                        rootPage.currentMonth--;
                    }
                }
            }

            Label {
                text: ["Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                    "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"][rootPage.currentMonth]
                + " " + rootPage.currentYear
                font.pixelSize: 24
                font.bold: true
                color: palette.windowText
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                text: ">"
                onClicked: {
                    if (rootPage.currentMonth === 11) {
                        rootPage.currentMonth = 0;
                        rootPage.currentYear++;
                    } else {
                        rootPage.currentMonth++;
                    }
                }
            }
        }

        MonthGrid {
            id: monthGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            month: rootPage.currentMonth
            year: rootPage.currentYear

            delegate: Rectangle {

                property bool isSelected: model.day === rootPage.selectedDate.getDate() &&
                                          model.month === rootPage.selectedDate.getMonth() &&
                                          model.year === rootPage.selectedDate.getFullYear()

                property bool isToday: model.date.getDate() === new Date().getDate() &&
                                       model.month === new Date().getMonth() &&
                                       model.year === new Date().getFullYear()

                color: isToday ? "#A9A9A9" : (isSelected ? "#20000000" : "transparent")
                radius: 8

                border.width: isSelected ? 2 : 1
                border.color: isToday ? "#808080" : (isSelected ? "#808080" : palette.mid)


                Label {
                    anchors.centerIn: parent
                    text: model.day
                    font.pixelSize: 16
                    color: isToday ? "white" : (model.month === monthGrid.month ? palette.windowText : palette.mid)
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                                    onClicked: {
                    rootPage.selectedDate = model.date
                    console.log("Выбран день:", model.day)
                    }
                }
            }
        }
    }
}