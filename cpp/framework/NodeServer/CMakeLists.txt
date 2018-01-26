
include_directories(${PROJECT_SOURCE_DIR}/patchclient)

link_libraries(patch)

set(MODULE "tarsnode")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/deploy/${MODULE}/bin)

#complice_module(${MODULE} "Node.tars")
complice_module(${MODULE})

#tarsnode依赖patch的客户端
add_dependencies(${MODULE} patch)
