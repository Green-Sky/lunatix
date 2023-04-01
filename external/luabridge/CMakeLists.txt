cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_library(luabridge INTERFACE)

target_compile_features(luabridge INTERFACE cxx_std_17)
target_include_directories(luabridge INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

