cmake_minimum_required(VERSION 3.26)
project(carnival)
set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 CONFIG REQUIRED)

file(GLOB sources CONFIGURE_DEPENDS
        "src/*.h" "src/*.cpp" "src/common/*.h" "src/common/*.cpp" "src/core/*.h" "src/core/*.cpp"
        "src/external/imgui/*.h" "src/external/imgui/*.cpp"
        "src/external/imgui/backends/imgui_impl_sdl2.h" "src/external/imgui/backends/imgui_impl_sdl2.cpp"
        "src/external/imgui/backends/imgui_impl_opengl3.h" "src/external/imgui/backends/imgui_impl_opengl3.cpp"
        "src/external/glad/src/glad.c" "src/external/glad/include/glad/glad.h" "src/external/glad/include/KHR/khrplatform.h"
        "src/external/stb/stb_image.h"
)

include_directories(src/external/imgui
        src/external/imgui/backends
        src/external/glad/include
        src/external/stb
)

add_executable(carnival ${sources}
        src/common/img.h)

target_link_libraries(carnival
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
