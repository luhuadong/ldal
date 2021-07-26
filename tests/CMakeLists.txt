cmake_minimum_required (VERSION 3.0)

project(ldal_test LANGUAGES C)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -Wunused-function -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "-lldal")

set_target_properties(${TARGET} PROPERTIES
    COMPILE_FLAGS "-lldal"
    LINK_FLAGS "-lldal"
)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../bin")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../inc")

add_executable(k37x_sample k37x_sample.c)
target_link_libraries(k37x_sample PRIVATE ldal)