include_directories(${PROJECT_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

aux_source_directory(. DIR_SRCS)

add_library(tarsservant ${DIR_SRCS})

target_link_libraries(tarsservant)

if(${TARS_SSL} EQUAL 1)
    target_link_libraries(tarsservant; crypto; ssl)
else() 
    target_link_libraries(tarsservant)
endif()

add_dependencies(tarsservant COPY-SERVENT-TARS tarsframework)

install(TARGETS tarsservant
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
