qt_add_library(ui_lib SHARED)

target_include_directories(ui_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include/ui
)

# Алиас для иконки
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/res/icons/app_icon1.svg
    PROPERTIES QT_RESOURCE_ALIAS "icons/app_icon1.svg"
)

# 🚨 АЛИАСЫ ДЛЯ ВСЕХ QML-ФАЙЛОВ (чтобы они лежали в корне модуля без лишних папок)
set_source_files_properties(../../qml/Main.qml PROPERTIES QT_RESOURCE_ALIAS "Main.qml")
set_source_files_properties(../../qml/pages/HomePage.qml PROPERTIES QT_RESOURCE_ALIAS "HomePage.qml")
set_source_files_properties(../../qml/pages/TodayPage.qml PROPERTIES QT_RESOURCE_ALIAS "TodayPage.qml")
set_source_files_properties(../../qml/pages/CalendarPage.qml PROPERTIES QT_RESOURCE_ALIAS "CalendarPage.qml")
set_source_files_properties(../../qml/pages/StatsPage.qml PROPERTIES QT_RESOURCE_ALIAS "StatsPage.qml")
set_source_files_properties(../../qml/components/HabitCard.qml PROPERTIES QT_RESOURCE_ALIAS "HabitCard.qml")

qt_add_qml_module(ui_lib
    URI HabitTracker.Presentation
    VERSION 1.0
    QML_FILES
        ../../qml/Main.qml
        ../../qml/pages/HomePage.qml
        ../../qml/pages/TodayPage.qml
        ../../qml/pages/CalendarPage.qml
        ../../qml/pages/StatsPage.qml
        ../../qml/components/HabitCard.qml
)

target_link_libraries(ui_lib PUBLIC
    Qt6::Quick
    Qt6::QuickControls2
    common_lib
    logic_lib
)