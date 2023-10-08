#ifndef CARNIVAL_APPLICATION_H
#define CARNIVAL_APPLICATION_H

#include <string>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "imgui.h"

namespace carnival::core {
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

    struct ImageData {
        GLuint texture = 0;
        GLuint framebuffer = 0;
        GLuint depthbuffer = 0;
        int width = 0;
        int height = 0;
    };

    struct ApplicationState {
        int viewport_width = 256;
        int viewport_height = 256;
        bool running = true;
        bool resize_queued = false;
        int window_height = defWindowHeight;
        int window_width = defWindowWidth;
    };

    class Application {
    public:
        Application();
        ~Application();
        void Run();
        void setupTriangle();
        void setupImage();
    private:
        RenderingContext rendering_context;
        InputContext input_context;
        ApplicationState app_state;
        ImageData image_data;

        void InitSDL();
        void InitWindow();
        void InitOpenGl();
        void InitImGui() const;
        void HandleEvents();
        void render();
        void setupGUI(ImGuiID dockID);
        void renderGUI();
        void renderGL();
        void updateTexture();
    };

}


#endif //CARNIVAL_APPLICATION_H
