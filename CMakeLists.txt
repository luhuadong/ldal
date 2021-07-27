cmake_minimum_required(VERSION 3.0)

project(ldal DESCRIPTION "Linux Device Abstraction Layer" LANGUAGES C)

set(LIB_MAJOR_VERSION "1")
set(LIB_MINOR_VERSION "0")
set(LIB_PATCH_VERSION "0")
set(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

if (CMAKE_VERSION VERSION_LESS 3.0)
    PROJECT(LDAL DESCRIPTION "Linux Device Abstraction Layer" LANGUAGES C)
    set(PROJECT_VERSION_MAJOR "${LIB_MAJOR_VERSION}")
    set(PROJECT_VERSION_MINOR "${LIB_MINOR_VERSION}")
    set(PROJECT_VERSION_PATCH"${LIB_PATCH_VERSION}")
    set(PROJECT_VERSION "${LIB_VERSION_STRING}")
else()
    cmake_policy(SET CMP0048 NEW)
    PROJECT(LDAL VERSION "${LIB_VERSION_STRING}" DESCRIPTION "Linux Device Abstraction Layer" LANGUAGES C)
endif()


SET(LDAL_BUILD_SHARED TRUE CACHE BOOL "Build shared library")
SET(PROJECT_SOURCE_DIR .)

add_subdirectory(src)
add_subdirectory(tests)
#add_subdirectory(tools)
