set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(TARGETNAME "testAdminRegistry")

include_directories(${util_SOURCE_DIR}/include)
include_directories(${tools_SOURCE_DIR})
include_directories(${servant_SOURCE_DIR})
include_directories(${framework_SOURCE_DIR}/protocol)

link_libraries(tarsservant tarsparse tarsutil pthread z rt)

aux_source_directory(. DIR_SRCS)
add_executable(${TARGETNAME} ${DIR_SRCS})

