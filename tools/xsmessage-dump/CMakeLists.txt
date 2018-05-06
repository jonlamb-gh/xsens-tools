cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(xsmessage-dump VERSION 0.1 LANGUAGES C)

add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/libxsparser
    ${CMAKE_CURRENT_BINARY_DIR}/libxsparser)

add_executable(
    xsmessage-dump
    src/main.c)

set_property(
    TARGET
    xsmessage-dump
    PROPERTY
    C_STANDARD
    99)

target_compile_options(
    xsmessage-dump
    PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wwrite-strings
    -Wstrict-prototypes
    -Wmissing-include-dirs
    -Werror)

target_include_directories(
    xsmessage-dump
    PRIVATE
    $<TARGET_PROPERTY:xsparser,INCLUDE_DIRECTORIES>)

target_link_libraries(
    xsmessage-dump
    -static-libgcc
    -static)

target_link_libraries(
    xsmessage-dump
    xsparser)
