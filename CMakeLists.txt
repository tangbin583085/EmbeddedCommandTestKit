cmake_minimum_required(VERSION 3.16)

project(EmbeddedCommandTestKit
    VERSION 0.1.0
    DESCRIPTION "A small host-side library for testing embedded device commands"
    LANGUAGES CXX
)

option(ECTK_BUILD_EXAMPLES "Build example programs" ON)
option(ECTK_BUILD_TESTS "Build the source-level test program" ON)

add_library(EmbeddedCommandTestKit
    src/CommandRunner.cpp
    src/MockTransport.cpp
    src/ScenarioRunner.cpp
)

add_library(ECTK::Core ALIAS EmbeddedCommandTestKit)

set_target_properties(EmbeddedCommandTestKit PROPERTIES
    EXPORT_NAME Core
)

target_compile_features(EmbeddedCommandTestKit PUBLIC cxx_std_17)

target_include_directories(EmbeddedCommandTestKit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if (MSVC)
    target_compile_options(EmbeddedCommandTestKit PRIVATE /W4)
else()
    target_compile_options(EmbeddedCommandTestKit PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (ECTK_BUILD_EXAMPLES)
    add_executable(ectk_console_example examples/console/main.cpp)
    target_link_libraries(ectk_console_example PRIVATE ECTK::Core)
endif()

if (ECTK_BUILD_TESTS)
    enable_testing()
    add_executable(ectk_source_tests tests/test_main.cpp)
    target_link_libraries(ectk_source_tests PRIVATE ECTK::Core)
    add_test(NAME ectk_source_tests COMMAND ectk_source_tests)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ECTK_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/EmbeddedCommandTestKit)

configure_package_config_file(
    cmake/EmbeddedCommandTestKitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedCommandTestKitConfig.cmake
    INSTALL_DESTINATION ${ECTK_INSTALL_CMAKE_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedCommandTestKitConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS EmbeddedCommandTestKit
    EXPORT ECTKTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT ECTKTargets
    FILE EmbeddedCommandTestKitTargets.cmake
    NAMESPACE ECTK::
    DESTINATION ${ECTK_INSTALL_CMAKE_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedCommandTestKitConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedCommandTestKitConfigVersion.cmake
    DESTINATION ${ECTK_INSTALL_CMAKE_DIR}
)

