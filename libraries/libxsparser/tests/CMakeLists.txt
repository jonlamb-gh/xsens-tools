cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

macro(add_c_test NAME COND)
    if(${COND})
        #list (APPEND all_tests ${NAME})
        add_executable(${NAME} ${NAME}.c)
        set_property(TARGET ${NAME} PROPERTY C_STANDARD 99)
        target_include_directories(
            ${NAME}
            PRIVATE
            $<TARGET_PROPERTY:xsparser,INCLUDE_DIRECTORIES>)
        target_link_libraries(${NAME} xsparser)

        add_test(NAME ${NAME} COMMAND ${NAME})
    endif()
endmacro()

add_c_test(parser ON)
