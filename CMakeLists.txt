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
target_compile_options(gvml PRIVATE -Wall -g)

# The PAINFUL parser
add_library(painful
    include/painful.h
    src/painful.c
)
target_include_directories(painful PUBLIC include)
target_include_directories(painful PUBLIC extern)
target_compile_options(painful PRIVATE -Wall -g)

# The Assembly parser
add_library(assembly
    include/assembly.h
    src/assembly.c
)
target_include_directories(assembly PUBLIC include)
target_include_directories(assembly PUBLIC extern)
target_compile_options(assembly PRIVATE -Wall -g)

add_library(gvm-repl
    include/repl.h
    src/repl.c
)
target_include_directories(gvm-repl PUBLIC include)
target_include_directories(gvm-repl PUBLIC extern)
target_link_libraries(gvm-repl PUBLIC linenoise)
target_link_libraries(gvm-repl PUBLIC gvml)

#target_link_libraries(gvm-repl PUBLIC painful)
target_link_libraries(gvm-repl PUBLIC assembly)

target_compile_options(assembly PRIVATE -Wall -g)

add_executable(gvm
    src/main.c
)
target_link_libraries(gvm PUBLIC gvml-repl)
target_compile_options(gvm PRIVATE -Wall -g)
