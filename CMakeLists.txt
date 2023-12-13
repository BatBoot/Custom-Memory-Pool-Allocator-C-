cmake_minimum_required(VERSION 3.20)
project(Allocators VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(custom_alloc_lib)

SET(APP_NAME Allocators)
add_executable(Allocators)

target_link_libraries(${APP_NAME} PUBLIC pool_allocator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g")

target_sources(${APP_NAME} PRIVATE main.cpp)
target_compile_features(${APP_NAME} PRIVATE cxx_std_20)
set_target_properties(${APP_NAME} PROPERTIES CXX_EXTENSIONS OFF)

IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "AppleClang")
    SET(GNU_COMPATIBLE TRUE)
ELSE ()
    SET(GNU_COMPATIBLE FALSE)
ENDIF()

IF(GNU_COMPATIBLE)
    IF(CMAKE_BUILD_TYPE AND NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "Debug"))
        message(FATAL_ERROR "Restricted build type: ${CMAKE_BUILD_TYPE}")
    ELSE ("${CMAKE_BUILD_TYPE}" STREQUAL "")
        SET(CMAKE_BUILD_TYPE Debug)
    ENDIF ()
ENDIF ()

IF(GNU_COMPATIBLE)
    target_compile_options(${APP_NAME} PRIVATE -Werror -Wall -Wextra -pedantic)
    IF(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${APP_NAME} PRIVATE -O3)
    ELSE(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${APP_NAME} PRIVATE -Og -g -fno-omit-frame-pointer)
    ENDIF()
ELSE("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(${APP_NAME} PRIVATE
            $<$<AND:$<CXX_COMPILER_ID:MSVC>: /Wall /Wextra /Wpedantic /Werror>)
    target_compile_options(${APP_NAME} PRIVATE
            $<$<AND:$<CXX_COMPILER_ID:MSVC>, <CONFIG:Release>>: /O2>)
    target_compile_options(${APP_NAME} PRIVATE
            $<$<AND:$<CXX_COMPILER_ID:MSVC>, <CONFIG:Debug>>: /RTC1 /Od /Zi /W4>)
ENDIF()


