qt_add_library(db_lib STATIC)

target_include_directories(db_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include/db
)

# Сюда будем добавлять DatabaseManager.cpp и репозитории
target_link_libraries(db_lib PUBLIC
    common_lib
    Qt6::Sql
)
