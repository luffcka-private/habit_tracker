cmake_minimum_required(VERSION 3.28)

project(habit_tracker_01 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Sql)
qt_standard_project_setup(REQUIRES 6.11)

add_subdirectory(src/common)
add_subdirectory(src/db)
add_subdirectory(src/logic)
add_subdirectory(src/ui)

qt_add_executable(apphabit_tracker_01
    src/main.cpp
)


target_link_libraries(apphabit_tracker_01 PRIVATE
    ui_lib
)

set_target_properties(apphabit_tracker_01 PROPERTIES
    WIN32_EXECUTABLE TRUE
)