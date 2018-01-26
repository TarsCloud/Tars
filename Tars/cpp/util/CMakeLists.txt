cmake_minimum_required(VERSION 2.8)
project(util)

include_directories(${MYSQL_DIR_INC})
link_directories(${MYSQL_DIR_LIB})

enable_language(ASM)

add_subdirectory(src)
add_subdirectory(include)


