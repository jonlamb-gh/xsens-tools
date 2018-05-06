cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(libxsparser VERSION 0.1 LANGUAGES C)

add_library(
    xsparser
    STATIC
    src/xsparser.c)

set_property(
    TARGET
    xsparser
    PROPERTY
    C_STANDARD
    99)

target_compile_options(
    xsparser
    PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wwrite-strings
    -Wstrict-prototypes
    -Wmissing-include-dirs
    -Werror)

target_include_directories(
    xsparser
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

# this makes the project importable from the build directory
export(
    TARGETS
    xsparser
    FILE
    LibXsParserConfig.cmake)

enable_testing()
add_subdirectory(tests)
