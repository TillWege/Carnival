cmake_minimum_required(VERSION 3.26)
project(cpp_ge)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(sources
        main.cpp
        imgui-style.h
        functions.h
)

add_executable(cpp_ge ${sources})

target_link_libraries(cpp_ge
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_link_libraries(cpp_ge PRIVATE glad::glad)
target_link_libraries(cpp_ge PRIVATE imgui::imgui)