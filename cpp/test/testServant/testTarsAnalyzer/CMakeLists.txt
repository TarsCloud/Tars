set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(TARGETNAME "test-tars-analyzer")

include_directories(${util_SOURCE_DIR}/include)
include_directories(${tools_SOURCE_DIR})
include_directories(${servant_SOURCE_DIR})

link_libraries(tarsparse tarsutil pthread)

aux_source_directory(. DIR_SRCS)
add_executable(${TARGETNAME} ${DIR_SRCS})

