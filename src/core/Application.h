#ifndef CPP_GE_APPLICATION_H
#define CPP_GE_APPLICATION_H

#include <string>
#include <SDL2/SDL.h>
#include "glad/glad.h"

namespace cpp_ge::core {
    const int defWindowWidth = 1280,
            defWindowHeight = 720;

    struct RenderingContext {
        SDL_Window *window_handle = nullptr;
        SDL_GLContext gl_context = nullptr;
        std::string glsl_version;
        GLuint shader_program = 0;
        GLuint vertex_buffer = 0;
        GLuint VertexArrayID = 0;
    };

    struct InputContext {
        SDL_GameController *controller = nullptr;
        SDL_Joystick *joystick = nullptr;
    };

    struct ApplicationState {
        int drawer_width = 300;
        int counter = 0;
        bool running = true;
        bool show_demo_window = false;
        bool show_another_window = false;
        int window_height = defWindowHeight;
        int window_width = defWindowWidth;
    };

    class Application {
    public:
        Application();
        ~Application();
        void Run();
        void setupTriangle();
    private:
        RenderingContext rendering_context;
        InputContext input_context;
        ApplicationState app_state;

        void InitSDL();
        void InitWindow();
        void InitOpenGl();
        void InitImGui() const;
        void HandleEvents();
        void render();
    };

}


#endif //CPP_GE_APPLICATION_H
