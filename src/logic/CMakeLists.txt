qt_add_library(logic_lib STATIC)

target_include_directories(logic_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include/logic
)

target_link_libraries(logic_lib PUBLIC
    common_lib
    db_lib
)
