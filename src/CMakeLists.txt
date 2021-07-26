cmake_minimum_required(VERSION 2.8)

project(ldal_library)

#set(CMAKE_C_FLAGS "-Wall -Werror -Wstrict-prototypes -Wmissing-prototypes")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -export-dynamic -lm -pthread -lev")

set(CMAKE_C_FLAGS "-std=gnu99 -g -Wall -Wunused-function -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lm -lev")

set_target_properties(${TARGET} PROPERTIES
    COMPILE_FLAGS "-pthread -lm -lev"
    LINK_FLAGS "-pthread -lm -lev"
)

# Headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../inc")
#include_directories("${PROJECT_SOURCE_DIR}/../inc")

# Sources
aux_source_directory(. DIR_LIB_SRCS)

message(STATUS "Compiling ${PROJECT_NAME} version: ${BOCON_VERSION}")

# shared library
IF (LDAL_BUILD_SHARED)
add_library(ldal SHARED
    ${DIR_LIB_SRCS}
)
ENDIF()

# static library
IF (LDAL_BUILD_STATIC)
add_library(ldal STATIC
    ${DIR_LIB_SRCS}
)
ENDIF()

# Link libraries
target_link_libraries(ldal PRIVATE m pthread ev)