
##########################################################################################
#生成.l和.y文件生成的代码文件copy到tarsparse目录下

set(OUT_DEPENDS_LEX_LIST)

complice_lex(OUT_DEPENDS_LEX_LIST)

FILE(GLOB SRC_LEX_LIST "*.l")

set(CLEAN_LIST)

#将生成代码文件copy到对应的目录
foreach (FILE ${SRC_LEX_LIST})

    get_filename_component(NAME_WE ${FILE} NAME_WE)
    set(LEX_CPP ${NAME_WE}.lex.cpp)
    set(CP_SOURCE_CPP_DIR "${PROJECT_SOURCE_DIR}/tarsparse")

    add_custom_command(
            OUTPUT ${CP_SOURCE_CPP_DIR}/${LEX_CPP}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND cp -rf ${LEX_CPP} ${CP_SOURCE_CPP_DIR}
            COMMENT "cp -rf ${LEX_CPP} ${CP_SOURCE_CPP_DIR}")

    #设置依赖
    list(APPEND OUT_DEPENDS_LEX_LIST ${CP_SOURCE_CPP_DIR}/${LEX_CPP})

    #设置需要清除的.h文件
    list(APPEND CLEAN_LIST ${CP_SOURCE_CPP_DIR}/${LEX_CPP})

endforeach ()

set(OUT_DEPENDS_YACC_LIST)

complice_yacc(OUT_DEPENDS_YACC_LIST)

FILE(GLOB SRC_YACC_LIST "*.y")

#将生成代码文件copy到对应的目录
foreach (FILE ${SRC_YACC_LIST})

    get_filename_component(NAME_WE ${FILE} NAME_WE)
    set(YACC_H ${NAME_WE}.tab.hpp)
    set(YACC_CPP ${NAME_WE}.tab.cpp)
    set(CP_SOURCE_H_DIR "${PROJECT_SOURCE_DIR}/tarsparse")

    add_custom_command(
            OUTPUT ${CP_SOURCE_H_DIR}/${YACC_H}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND cp -rf ${YACC_H} ${YACC_CPP} ${CP_SOURCE_H_DIR}
            COMMENT "cp -rf ${YACC_H} ${YACC_CPP} ${CP_SOURCE_H_DIR}")

    #设置依赖
    list(APPEND OUT_DEPENDS_YACC_LIST ${CP_SOURCE_H_DIR}/${YACC_H})

    #设置需要清除的.h文件
    list(APPEND CLEAN_LIST ${CP_SOURCE_H_DIR}/${YACC_H})
    list(APPEND CLEAN_LIST ${CP_SOURCE_H_DIR}/${YACC_CPP})

endforeach ()

#设置需要清除的文件
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_LIST}")

#add_custom_target(COPY-LEX-YACC ALL DEPENDS ${OUT_DEPENDS_LEX_LIST} ${OUT_DEPENDS_YACC_LIST})

add_custom_target(COPY-LEX-YACC ALL DEPENDS ${OUT_DEPENDS_LEX_LIST} ${CP_SOURCE_CPP_DIR}/${LEX_CPP} ${OUT_DEPENDS_YACC_LIST} ${CP_SOURCE_H_DIR}/${YACC_H})
