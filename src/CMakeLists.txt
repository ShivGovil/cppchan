cmake_minimum_required(VERSION 3.30)
project(CppChannels)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# export lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

add_subdirectory(channels)
add_subdirectory(waitgroup)
add_subdirectory(test)
