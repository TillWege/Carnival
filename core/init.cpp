
#include <SDL.h>
#include <string>

/**
 * Initializes SDL
 * @return 0 on success, 1 on failure
 */
int initSDL(std::string& glsl_version)
{
    int result = 0;

    result = result | SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

    result = result | SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    result = result | SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    result = result | SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    result = result | SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE
    );

    #ifdef __APPLE__
        // GL 3.2 Core + GLSL 150
        glsl_version = "#version 150";
        result = result | SDL_GL_SetAttribute( // required on Mac OS
            SDL_GL_CONTEXT_FLAGS,
            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
            );
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    #elif __linux__
        // GL 3.2 Core + GLSL 150
        glsl_version = "#version 150";
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    #elif _WIN32
        // GL 3.0 + GLSL 130
        glsl_version = "#version 130";
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #endif


    return result;
}