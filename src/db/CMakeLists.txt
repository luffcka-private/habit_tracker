qt_add_library(db_lib STATIC
    Habit.h
    DatabaseManager.h
    DatabaseManager.cpp
)

target_include_directories(db_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}   # заголовки db-слоя лежат рядом с исходниками
    ${CMAKE_SOURCE_DIR}/include/db
)

# Сюда будем добавлять репозитории (HabitRepository и т.п.)
target_link_libraries(db_lib PUBLIC
    common_lib
    Qt6::Sql
)
