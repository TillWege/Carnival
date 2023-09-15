#include "window.h"

int defWindowWidth = 1280,
        defWindowHeight = 720;

Window::Window() {
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
            SDL_WINDOW_OPENGL
            | SDL_WINDOW_RESIZABLE
            | SDL_WINDOW_ALLOW_HIGHDPI
    );
    this->window_handle = SDL_CreateWindow(
            "Dear ImGui SDL",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            defWindowWidth,
            defWindowHeight,
            window_flags
    );

    SDL_SetWindowMinimumSize(this->window_handle, 500, 300);
}

Window::~Window() {

}

