cmake_minimum_required(VERSION 3.12)
project(mandelpipeline)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES src/*.h src/*.cpp src/*.c)

add_executable(mandelpipeline ${SOURCES})

if (UNIX OR MINGW)
    find_package(PkgConfig)
    pkg_search_module(GTKMM REQUIRED gtkmm-3.0)
    link_directories(
            ${GTKMM_LIBRARY_DIRS})
    target_compile_options(mandelpipeline PUBLIC "-Wno-c++11-narrowing")
else ()
    find_path(GTKMM_INCLUDE_DIRS gtkmm/base.h)
    find_library(GTKMM_LIBRARY gtkmm REQUIRED)
    find_library(GLIBMM_LIBRARY glibmm-2.0 REQUIRED)
    find_library(SIGCPP_LIBRARY sigc-2.0 REQUIRED)
    find_library(GDKMM_LIBRARY gdkmm REQUIRED)
    find_library(ATKMM_LIBRARY atkmm REQUIRED)
    find_library(CAIROMM_LIBRARY cairomm-1.0 REQUIRED)
    find_library(CAIRO_LIBRARY cairo REQUIRED)
    set(GTKMM_LIBRARIES ${GTKMM_LIBRARY} ${GLIBMM_LIBRARY} ${SIGCPP_LIBRARY} ${GDKMM_LIBRARY} ${CAIROMM_LIBRARY} ${ATKMM_LIBRARY} ${CAIRO_LIBRARY})

    target_compile_options(mandelpipeline PRIVATE /bigobj)
    target_compile_options(mandelpipeline PRIVATE /wd4250)
endif ()

message(${GTKMM_LIBRARIES})

find_package(OpenCL REQUIRED)
include_directories(
        ${GTKMM_INCLUDE_DIRS})

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    find_package(OpenMP REQUIRED)
    target_link_libraries(mandelpipeline PUBLIC OpenMP::OpenMP_CXX)
endif()
target_link_libraries(mandelpipeline PUBLIC OpenCL::OpenCL ${GTKMM_LIBRARIES})