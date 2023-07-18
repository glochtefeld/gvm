cmake_minimum_required(VERSION 3.16)
project(gvm VERSION 0.0.1 LANGUAGES C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(extern)

add_library(gvml
    include/common.h
    src/gvm.c
)
target_include_directories(gvml PUBLIC include)
target_include_directories(gvml PUBLIC extern)

# The PAINFUL REPL
add_library(painful
    include/painful.h
    src/painful.c
)
target_include_directories(painful PUBLIC include)
target_include_directories(painful PUBLIC extern)
target_link_libraries(painful PUBLIC linenoise)
target_link_libraries(painful PUBLIC gvml)

add_executable(gvm
    src/main.c
)
target_link_libraries(gvm PUBLIC gvml)
target_link_libraries(gvm PUBLIC painful)
