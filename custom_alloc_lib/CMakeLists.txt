cmake_minimum_required(VERSION 3.20)
project(custom_alloc_lib VERSION 1.0 LANGUAGES CXX)

add_library(pool_allocator INTERFACE)
target_sources(pool_allocator INTERFACE pool_alloc.h)
target_compile_features(pool_allocator INTERFACE cxx_std_20)