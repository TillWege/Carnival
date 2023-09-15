//
// Created by tillw on 15/09/2023.
//

#ifndef CPP_GE_WINDOW_H
#define CPP_GE_WINDOW_H
#include <SDL2/SDL.h>


class Window {
public:
    Window();
    ~Window();
    SDL_Window* window_handle = nullptr;
    int test = 0;
};


#endif //CPP_GE_WINDOW_H
