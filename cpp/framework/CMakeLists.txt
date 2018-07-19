cmake_minimum_required(VERSION 2.8)

project(framework)

include_directories(${util_SOURCE_DIR}/include)
include_directories(${servant_SOURCE_DIR})
include_directories(${MYSQL_DIR_INC})

link_libraries(tarsservant tarsutil)
link_libraries(${MYSQL_DIR_LIB}/libmysqlclient.a)
link_libraries(pthread z dl rt)

macro(complice_module MODULE)

    include_directories(${PROJECT_SOURCE_DIR}/protocol)
    include_directories(${servant_SOURCE_DIR}/servant)

    aux_source_directory(. DIR_SRCS)

    add_executable(${MODULE} ${DIR_SRCS})
    add_dependencies(${MODULE} FRAMEWORK-PROTOCOL)

endmacro()

#调用tars2cpp, 生成tars对应的文件
    macro(complice_tars OUT_DEPENDS_LIST)
    set(DEPENDS_LIST)

    set(SERVANT_PROTOCOL "${servant_SOURCE_DIR}/protocol")

    set(CLEAN_LIST)

    set(CURRENT_PROTOCOL_DIR ${PROJECT_SOURCE_DIR}/protocol)

    FILE(GLOB SRC_LIST "${CURRENT_PROTOCOL_DIR}/*.tars")

    foreach (FILE ${SRC_LIST})

        #设置tars文件搜索路径
        set(INCLUDE_STRING "--include=\"${SERVANT_PROTOCOL}\"")

        get_filename_component(NAME_WE ${FILE} NAME_WE)

        # 生成tars文件
        set(TARS_IN ${FILE})
        set(TARS_H ${NAME_WE}.h)

        add_custom_command(
                OUTPUT ${CURRENT_PROTOCOL_DIR}/${TARS_H}
                WORKING_DIRECTORY ${CURRENT_PROTOCOL_DIR}
                DEPENDS tars2cpp
                COMMAND ${TARS2CPP} --with-tars ${INCLUDE_STRING} ${TARS_IN}
                COMMENT "${TARS2CPP} --with-tars ${INCLUDE_STRING} ${TARS_IN}")

        list(APPEND DEPENDS_LIST ${CURRENT_PROTOCOL_DIR}/${TARS_H})

        #设置需要清除的文件
        list(APPEND CLEAN_LIST ${CURRENT_PROTOCOL_DIR}/${NAME_WE}.h)

    endforeach ()

    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_LIST}")

    set(OUT_DEPENDS_LIST ${DEPENDS_LIST})
endmacro()

add_subdirectory(protocol)
add_subdirectory(ConfigServer)
add_subdirectory(NodeServer)
add_subdirectory(NotifyServer)
add_subdirectory(PatchServer)
add_subdirectory(AdminRegistryServer)
add_subdirectory(RegistryServer)
add_subdirectory(PropertyServer)
add_subdirectory(LogServer)
add_subdirectory(patchclient)
add_subdirectory(StatServer)
add_subdirectory(QueryStatServer)
add_subdirectory(QueryPropertyServer)
add_subdirectory(AuthServer)
############################################################################################
# 打包deploy, 用于部署

set(DEPENDS_LIST)
list(APPEND DEPENDS_LIST tarsnode)
list(APPEND DEPENDS_LIST tarsregistry)
list(APPEND DEPENDS_LIST tarsAdminRegistry)
list(APPEND DEPENDS_LIST tarspatch)
list(APPEND DEPENDS_LIST tarsconfig)

#set(FRAMEWORK-TGZ "${CMAKE_BINARY_DIR}/framework-tmp.tgz")
set(FRAMEWORK-TGZ "${CMAKE_BINARY_DIR}/framework.tgz")

#需要压缩的文件
set(MODULES "tars_install.sh")
set(MODULES "${MODULES} tarsnode_install.sh")

foreach (MODULE ${DEPENDS_LIST})
    set(MODULES "${MODULES} ${MODULE}")
endforeach ()

#打包脚本
SET(RUN_DEPLOY_COMMAND_FILE "${PROJECT_BINARY_DIR}/run-deploy-framework.cmake")
FILE(WRITE ${RUN_DEPLOY_COMMAND_FILE} "#deploy framework-tar\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "EXECUTE_PROCESS(\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/deploy/\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND cp -rf ${PROJECT_SOURCE_DIR}/deploy/tars_install.sh .\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND cp -rf ${PROJECT_SOURCE_DIR}/deploy/tarsnode_install.sh .\n")
foreach (MODULE ${DEPENDS_LIST})
    FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND cp -rf ${PROJECT_SOURCE_DIR}/deploy/${MODULE} .\n")
endforeach ()
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} ")\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "EXECUTE_PROCESS(\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/deploy/\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND tar czfv ${FRAMEWORK-TGZ} ${MODULES}\n")
#FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND mv ${FRAMEWORK-TGZ} ${CMAKE_BINARY_DIR}/framework.tgz\n")
FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} ")\n")

#执行命令
add_custom_command(OUTPUT ${FRAMEWORK-TGZ}
        COMMAND ${CMAKE_COMMAND} -P ${RUN_DEPLOY_COMMAND_FILE}
        COMMENT "call ${RUN_DEPLOY_COMMAND_FILE}")

add_custom_target(framework-tar
        DEPENDS ${FRAMEWORK-TGZ} ${DEPENDS_LIST})

############################################################################################
# 打包tarsnotify tarsstat tarsproperty tarslog tarsauth
set(TARSQUERYSTAT tarsquerystat)
set(TARSQUERYPROPERTY tarsqueryproperty)

set(DEPENDS_LIST)
list(APPEND DEPENDS_LIST tarsnotify)
list(APPEND DEPENDS_LIST tarsstat)
list(APPEND DEPENDS_LIST tarsproperty)
list(APPEND DEPENDS_LIST tarslog)
list(APPEND DEPENDS_LIST tarsquerystat)
list(APPEND DEPENDS_LIST tarsqueryproperty)
list(APPEND DEPENDS_LIST tarsauth)


foreach (MODULE ${DEPENDS_LIST})

    #set(MODULE-TGZ "${CMAKE_BINARY_DIR}/${MODULE}-tmp.tgz")
    set(MODULE-TGZ "${CMAKE_BINARY_DIR}/${MODULE}.tgz")

    #打包脚本
    SET(RUN_DEPLOY_COMMAND_FILE "${PROJECT_BINARY_DIR}/run-deploy-${MODULE}.cmake")
    FILE(WRITE ${RUN_DEPLOY_COMMAND_FILE} "#deploy ${MODULE}-tar\n")
    FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "EXECUTE_PROCESS(\n")
    FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/deploy/\n")
    FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND tar czfv ${MODULE-TGZ} ${MODULE}\n")
    #FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} "COMMAND mv ${MODULE-TGZ} ${CMAKE_BINARY_DIR}/${MODULE}.tgz\n")
    FILE(APPEND ${RUN_DEPLOY_COMMAND_FILE} ")\n")

    add_custom_command(OUTPUT ${MODULE-TGZ}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            COMMAND ${CMAKE_COMMAND} -P ${RUN_DEPLOY_COMMAND_FILE}
            COMMENT "call ${RUN_DEPLOY_COMMAND_FILE}")

    add_custom_target(${MODULE}-tar DEPENDS ${MODULE-TGZ} ${MODULE})
endforeach ()

############################################################################################












