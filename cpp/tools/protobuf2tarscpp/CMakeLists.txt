set(MODULE "pb2tarscpp")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
    
aux_source_directory(. DIR_SRCS)
        
link_libraries(protobuf;protoc)
          
add_executable("pb2tarscpp"  ${DIR_SRCS})

install(TARGETS "pb2tarscpp"
        RUNTIME DESTINATION tools)

