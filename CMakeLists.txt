cmake_minimum_required(VERSION 3.20)
file(STRINGS VERSION APP_VERSION LIMIT_COUNT 1)
project(qrgen VERSION ${APP_VERSION} LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PkgConfig REQUIRED)
pkg_check_modules(QRENCODE REQUIRED IMPORTED_TARGET libqrencode)
find_package(PNG REQUIRED)

add_library(qrcore STATIC src/core.c)
target_include_directories(qrcore PUBLIC src)
target_link_libraries(qrcore PUBLIC PkgConfig::QRENCODE PNG::PNG)

add_executable(qrgen src/main.c)
target_compile_definitions(qrgen PRIVATE APP_VERSION="${PROJECT_VERSION}")
target_link_libraries(qrgen PRIVATE qrcore)

include(CTest)
if(BUILD_TESTING)
    add_executable(test_core tests/test_core.c)
    target_link_libraries(test_core PRIVATE qrcore)
    add_test(NAME core COMMAND test_core)

    add_test(NAME cli_version COMMAND qrgen --version)
    set_tests_properties(cli_version PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")

    add_test(NAME cli_bad_args COMMAND qrgen generate --no-such-option)
    set_tests_properties(cli_bad_args PROPERTIES WILL_FAIL TRUE)
endif()

install(TARGETS qrgen RUNTIME DESTINATION bin)
install(FILES packaging/qrgen.conf DESTINATION /etc/qrgen)
