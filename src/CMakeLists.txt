cmake_minimum_required(VERSION 3.0)

project(ldal LANGUAGES C)

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
aux_source_directory(./class DIR_CLASS_SRCS)

set(SRC 
    ldal.c
    at.c
    cJSON.c
    jsonrpc-c.c
    ringbuffer.c
    netdev.c
    server.c
    class/ldal_udp.c
    class/ldal_tcp.c
    class/ldal_analog.c
    class/ldal_digital.c
    class/ldal_file.c
    class/ldal_gpio.c
    class/ldal_me.c
    class/ldal_memory.c
    class/ldal_misc.c
    class/ldal_rtc.c
    class/ldal_serial.c
)

message(STATUS "Compiling ${PROJECT_NAME} version: ${BOCON_VERSION}")

# shared library
IF (LDAL_BUILD_SHARED)
add_library(${PROJECT_NAME} SHARED
    ${DIR_LIB_SRCS}
    ${DIR_CLASS_SRCS}
)
ENDIF()

# static library
IF (LDAL_BUILD_STATIC)
add_library(${PROJECT_NAME} STATIC
    ${DIR_LIB_SRCS}
    ${DIR_CLASS_SRCS}
)
ENDIF()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE m ev pthread)