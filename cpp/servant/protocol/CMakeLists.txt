
##########################################################################################
#生成tars文件, 并将.h文件copy到servant目录下

set(OUT_DEPENDS_LIST)

#在当前目录生成tars对应的文件
complice_tars(OUT_DEPENDS_LIST "servant")

FILE(GLOB SRC_LIST "*.tars")

set(CLEAN_LIST)

#将生成tars文件copy到对应的源码目录
foreach (FILE ${SRC_LIST})

    #copy .h文件
    get_filename_component(NAME_WE ${FILE} NAME_WE)
    set(TARS_H ${NAME_WE}.h)
    set(CP_SOURCE_H_DIR "${PROJECT_SOURCE_DIR}/servant")

    add_custom_command(
            OUTPUT ${CP_SOURCE_H_DIR}/${TARS_H}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND cp -rf ${TARS_H} ${CP_SOURCE_H_DIR}
            COMMENT "cp -rf ${TARS_H} ${CP_SOURCE_H_DIR}")

    #设置依赖
    list(APPEND OUT_DEPENDS_LIST ${CP_SOURCE_H_DIR}/${TARS_H})

    #设置需要清除的.h文件
    list(APPEND CLEAN_LIST ${CP_SOURCE_CPP_DIR}/${NAME_WE}.h)

endforeach ()

#设置需要清除的文件
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_LIST}")

add_custom_target(COPY-SERVENT-TARS ALL DEPENDS TUP-TARS ${OUT_DEPENDS_LIST})

set(TARSPROTOCOLS
AdminF.tars
AdminF.h
Auth.tars
Auth.h
BaseF.tars
BaseF.h
ConfigF.tars
ConfigF.h
EndpointF.tars
EndpointF.h
LogF.tars
LogF.h
NodeF.tars
NodeF.h
NotifyF.tars
NotifyF.h
PropertyF.tars
PropertyF.h
QueryF.tars
QueryF.h
StatF.tars
StatF.h)
                      
INSTALL(FILES ${TARSPROTOCOLS} DESTINATION   /home/tarsproto/protocol/)
