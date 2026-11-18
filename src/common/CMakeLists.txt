qt_add_library(common_lib STATIC
    AppException.h)

# Указываем, где лежат заголовочные файлы (по методичке они в include/)
target_include_directories(common_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include/common
)

# Пока файлов нет, но когда создашь, просто перечисли их:
# target_sources(common_lib PRIVATE AppException.cpp Logger.cpp)

target_link_libraries(common_lib PUBLIC Qt6::Core)
