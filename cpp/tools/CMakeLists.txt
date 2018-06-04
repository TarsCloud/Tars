cmake_minimum_required(VERSION 2.8)
project(tools)

include_directories(${util_SOURCE_DIR}/include)

link_libraries(tarsutil)

function(tars2language TARGET)
    set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

    include_directories(${PROJECT_SOURCE_DIR}/tarsparse)

    aux_source_directory(. DIR_SRCS)

    link_libraries(tarsparse)

    add_executable(${TARGET} ${DIR_SRCS})

    install(TARGETS ${TARGET}
            RUNTIME DESTINATION tools)
endfunction()

macro(complice_lex OUT_DEPENDS_LEX_LIST)
    set(DEPENDS_LIST)

    FILE(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.l")

    set(CLEAN_LIST)

    foreach (FILE ${SRC_LIST})

        get_filename_component(NAME_WE ${FILE} NAME_WE)

        # 生成tars文件
        set(LEX_IN ${FILE})
        set(LEX_CPP ${NAME_WE}.lex.cpp)

        add_custom_command(
                OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${LEX_CPP}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND flex ${LEX_IN}
                COMMENT "flex ${LEX_IN}")

        list(APPEND DEPENDS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/${LEX_CPP})

        #设置需要清除的文件
        get_filename_component(PATH ${FILE} PATH)
        list(APPEND CLEAN_LIST ${PATH}/${LEX_CPP})

    endforeach (FILE)

    set(OUT_DEPENDS_LEX_LIST ${DEPENDS_LIST})

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_LIST}")

endmacro()

macro(complice_yacc OUT_DEPENDS_YACC_LIST)
    set(DEPENDS_LIST)

    FILE(GLOB SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.y")

    set(CLEAN_LIST)

    foreach (FILE ${SRC_LIST})

        get_filename_component(NAME_WE ${FILE} NAME_WE)

        set(YACC_IN ${FILE})
        set(YACC_H ${NAME_WE}.tab.hpp)
        set(YACC_CPP ${NAME_WE}.tab.cpp)

        add_custom_command(
                OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${YACC_H}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND bison  ${YACC_IN} -o ${YACC_CPP}
                COMMENT "bison  ${YACC_IN}  -o ${YACC_CPP}")

        list(APPEND DEPENDS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/${YACC_H})

        #设置需要清除的文件
        get_filename_component(PATH ${FILE} PATH)
        
        list(APPEND CLEAN_LIST ${PATH}/${YACC_H})
        list(APPEND CLEAN_LIST ${PATH}/${YACC_CPP})
        #if (EXISTS ${PATH}/${NAME_WE}.tab.cpp)
        #    list(APPEND CLEAN_LIST ${PATH}/${NAME_WE}.tab.cpp)
        #endif (EXISTS ${PATH}/${NAME_WE}.tab.cpp)

    endforeach (FILE)

    set(OUT_DEPENDS_YACC_LIST ${DEPENDS_LIST})

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_LIST}")

endmacro()

add_subdirectory(tarsgrammar)
add_subdirectory(tarsparse)
add_subdirectory(tars2cpp)
add_subdirectory(tars2python)
add_subdirectory(tars2cs)
add_subdirectory(tars2c)
add_subdirectory(tars2oc)
add_subdirectory(tars2php)
add_subdirectory(tars2android)
add_subdirectory(tars2node)

FIND_PACKAGE(Protobuf)
IF(PROTOBUF_FOUND)
    add_subdirectory(protobuf2tarscpp)
ENDIF()
