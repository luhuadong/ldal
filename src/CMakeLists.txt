cmake_minimum_required (VERSION 2.8)

project (ldal)

set(PROJECT_SOURCE_DIR ..)
include_directories("${PROJECT_SOURCE_DIR}/inc")
aux_source_directory(. DIR_LIB_SRCS)

# shared library
add_library(ldal SHARED
    ${DIR_LIB_SRCS}
)
