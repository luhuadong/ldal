cmake_minimum_required (VERSION 3.0)

project(ldal_tool LANGUAGES C)

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -Wunused-function -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "-lldal")

set_target_properties(${TARGET} PROPERTIES
    COMPILE_FLAGS "-lldal"
    LINK_FLAGS "-lldal"
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../inc")

add_executable(list_device list_device.c)

target_link_libraries(list_device PRIVATE ldal pthread)