#include <filesystem>
#include "../common/imgui-style.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "../common/functions.h"
#include "Application.h"
#include "../common/shader.h"
#include "imgui_internal.h"
#include "../common/img.h"

using namespace carnival;

namespace carnival::core {
    static const GLfloat g_vertex_buffer_data[] = {
            -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            //1.0f, -1.0f, 0.0f,
    };

    Application::Application() {
        InitSDL();
        InitWindow();
        InitOpenGl();
        InitImGui();

        std::cout << "Current path is " << std::filesystem::current_path() << std::endl << std::flush;

        auto currentPath = std::filesystem::current_path();

        auto vertPath = currentPath / "src" / "shader" / "test.vert";
        auto fragPath = currentPath / "src" / "shader" / "test.frag";

        rendering_context.shader_program = LoadShaders(vertPath.string().c_str(), fragPath.string().c_str());
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    }

    Application::~Application() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(rendering_context.gl_context);
        SDL_DestroyWindow(rendering_context.window_handle);
        SDL_Quit();

        std::cout << "\n- - -\n" << "[" << currentTime(std::chrono::system_clock::now()) << "] " << "Quit\n";
    }

    void Application::Run() {
        while (app_state.running) {
            HandleEvents();
            render();
        }

    }

    void Application::InitOpenGl() {
        rendering_context.gl_context = SDL_GL_CreateContext(rendering_context.window_handle);
        if (rendering_context.gl_context == nullptr) {
            std::cerr << "[ERROR] Failed to create a GL context: "
                      << SDL_GetError() << std::endl;
            throw EXIT_FAILURE;
        }
        SDL_GL_MakeCurrent(rendering_context.window_handle, rendering_context.gl_context);

        // enable VSync
        SDL_GL_SetSwapInterval(1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

        if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
            std::cerr << "[ERROR] Couldn't initialize glad" << std::endl;
            throw EXIT_FAILURE;
        } else {
            std::cout << "[INFO] glad initialized" << std::endl;
        }

        glEnable(GL_MULTISAMPLE);

        std::cout << "[INFO] OpenGL renderer: "
                  << glGetString(GL_RENDERER)
                  << std::endl;

        std::cout << "[INFO] OpenGL version: "
                  << glGetString(GL_VERSION)
                  << std::endl;

        std::cout << "[INFO] OpenGL vendor: "
                  << glGetString(GL_VENDOR)
                  << std::endl;

        std::cout << "[INFO] OpenGL shading language version: "
                  << glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << std::endl;

        int nNumExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &nNumExtensions);

        for (int i = 0; i < nNumExtensions; i++) {
            std::cout << "[INFO] OpenGL extension: "
                      << glGetStringi(GL_EXTENSIONS, i)
                      << std::endl;
        }

        // apparently, that shows maximum supported version
        std::cout << "[INFO] OpenGL from glad: "
                  << GLVersion.major
                  << "."
                  << GLVersion.minor
                  << std::endl;
    }

    void Application::InitImGui() const {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.IniFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        setImGuiStyle();

        ImGui_ImplSDL2_InitForOpenGL(rendering_context.window_handle, rendering_context.gl_context);
        ImGui_ImplOpenGL3_Init(rendering_context.glsl_version.c_str());
    }

    void Application::InitWindow() {

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);


        auto window_flags = (SDL_WindowFlags) (
                SDL_WINDOW_OPENGL
                | SDL_WINDOW_RESIZABLE
        );
        rendering_context.window_handle = SDL_CreateWindow(
                "Carnival",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                defWindowWidth,
                defWindowHeight,
                window_flags
        );

        SDL_SetWindowMinimumSize(rendering_context.window_handle, 500, 300);

    }

    void Application::InitSDL() {
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
        rendering_context.glsl_version = "#version 150";
        result = result | SDL_GL_SetAttribute( // required on macOS
                SDL_GL_CONTEXT_FLAGS,
                SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
        );
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __linux__
        // GL 3.2 Core + GLSL 150
        rendering_context.glsl_version = "#version 150";
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif _WIN32
        // GL 3.0 + GLSL 130
        rendering_context.glsl_version = "#version 130";
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        result = result | SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

        if (result != 0) {
            std::cerr << "[ERROR] Failed to initialize SDL: "
                      << SDL_GetError() << std::endl;
            throw EXIT_FAILURE;
        }
    }

    void Application::HandleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch (event.type) {
                case SDL_QUIT:
                    app_state.running = false;
                    break;

                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            app_state.window_width = event.window.data1;
                            app_state.window_height = event.window.data2;
                            break;
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            app_state.running = false;
                            break;
                        case SDLK_f:
                            if (event.key.keysym.mod & KMOD_CTRL) {
                                if (SDL_GetWindowFlags(rendering_context.window_handle) &
                                    SDL_WINDOW_FULLSCREEN_DESKTOP) {
                                    SDL_SetWindowFullscreen(rendering_context.window_handle, 0);
                                } else {
                                    SDL_SetWindowFullscreen(rendering_context.window_handle,
                                                            SDL_WINDOW_FULLSCREEN_DESKTOP);
                                }
                            }
                            break;
                        case SDLK_v:
                            if (event.key.keysym.mod & KMOD_CTRL) {
                                SDL_GL_SetSwapInterval(!SDL_GL_GetSwapInterval());
                            }
                            break;
                        case SDLK_k:
                            if (input_context.controller != nullptr) {
                                SDL_GameControllerClose(input_context.controller);
                                input_context.controller = nullptr;
                                std::cout << "[INFO] Controller disconnected" << std::endl;
                            } else {
                                std::cout << "[ERROR] No controller connected" << std::endl;
                            }
                            break;
                        case SDLK_h: {
                            if (input_context.controller == nullptr) { break; }
                            input_context.joystick = SDL_GameControllerGetJoystick(input_context.controller);
                            if (input_context.joystick == nullptr) { break; }

                            int res = SDL_JoystickRumble(input_context.joystick, 0xFFFF, 0xFFFF, 1000);
                            std::cout << "[INFO] Rumble result: " << res << std::endl;
                            break;
                        }
                        case SDLK_j:
                            if (input_context.controller == nullptr) {
                                SDL_JoystickUpdate();
                                int joyStickCount = SDL_NumJoysticks();
                                if (joyStickCount != 0) {
                                    input_context.controller = SDL_GameControllerOpen(0);
                                    if (input_context.controller != nullptr) {
                                        std::cout << "[INFO] Controller connected" << std::endl;
                                    } else {
                                        std::cout << "[ERROR] Controller not connected" << std::endl;
                                        std::cout << SDL_GetError() << std::endl;
                                    }
                                } else {
                                    std::cout << "[ERROR] No controller connected" << std::endl;
                                }
                            } else {
                                std::cout << "[ERROR] Controller already connected" << std::endl;
                            }
                            break;
                        case SDLK_r:
                            log();
                            break;
                        case SDLK_F11:
                            if (SDL_GetWindowFlags(rendering_context.window_handle) & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                                SDL_SetWindowFullscreen(rendering_context.window_handle, 0);
                            } else {
                                SDL_SetWindowFullscreen(rendering_context.window_handle, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            }
                            break;
                    }
                    break;
            }
        }
    }

    void Application::setupTriangle()
    {
        glGenVertexArrays(1, &rendering_context.VertexArrayID);
        glBindVertexArray(rendering_context.VertexArrayID);
        // This will identify our vertex buffer
        // Generate 1 buffer, put the resulting identifier in vertex-buffer
        glGenBuffers(1, &rendering_context.vertex_buffer);
        // The following commands will talk about our 'vertex-buffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, rendering_context.vertex_buffer);
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    }

    void Application::setupImage()
    {
        GLuint FramebufferName = 0;
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        GLuint renderedTexture;
        glGenTextures(1, &renderedTexture);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        image_data.width = app_state.viewport_width;
        image_data.height = app_state.viewport_height;

        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_data.width, image_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // The depth buffer
        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, image_data.width, image_data.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

        // Set "renderedTexture" as our colour attachement #0
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        image_data.texture = renderedTexture;
        image_data.framebuffer = FramebufferName;
        image_data.depthbuffer = depthrenderbuffer;
    }

    void Application::updateTexture()
    {
        glDeleteTextures(1, &image_data.texture);
        glDeleteRenderbuffers(1, &image_data.depthbuffer);
        glDeleteFramebuffers(1, &image_data.framebuffer);
        setupImage();
    }

    void Application::setupGUI(ImGuiID dockID)
    {
        auto id1 = ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Left, 0.2f, nullptr, &dockID);
        auto id2 = ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Right, 0.2f, nullptr, &dockID);
        auto id3 = ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Down, 0.2f, nullptr, &dockID);


        ImGui::DockBuilderDockWindow("Left Panel", id1);
        ImGui::DockBuilderDockWindow("Right Panel", id2);
        ImGui::DockBuilderDockWindow("Bottom Panel", id3);
        ImGui::DockBuilderDockWindow("Viewport-Container", dockID);
        ImGui::DockBuilderDockWindow("Viewport-Container2", dockID);


        ImGui::DockBuilderFinish(dockID);
    }

    void Application::renderGUI()
    {
        // Wichtig!
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(rendering_context.window_handle);

        int sdl_width, sdl_height;
        static bool firstFrame = true;
        SDL_GetWindowSize(rendering_context.window_handle, &sdl_width, &sdl_height);

        ImGui::NewFrame();

        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Quit")) {
                app_state.running = false;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        ImGuiID dockID = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        if (firstFrame) {
            setupGUI(dockID);
            firstFrame = false;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGuiWindowClass window_class_fixed;
        window_class_fixed.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverCentralNode;
        ImGui::SetNextWindowClass(&window_class_fixed);
        ImGui::Begin("Viewport-Container", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

        auto size = ImGui::GetContentRegionAvail();

        if((int)size.x != app_state.viewport_width || (int)size.y != app_state.viewport_height)
        {
            app_state.viewport_width = (int)size.x;
            app_state.viewport_height = (int)size.y;
            app_state.resize_queued = true;
        }

        ImGui::Image((void*)(intptr_t)image_data.texture, ImVec2((float)image_data.width, (float)image_data.height));
        ImGui::End();

        ImGui::PopStyleVar();

        ImGuiWindowClass window_class_dockable;
        window_class_dockable.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;

        ImGui::SetNextWindowClass(&window_class_dockable);
        ImGui::Begin("Left Panel", nullptr);
        ImGui::Text("Render Controls");

        ImGui::End();

        ImGui::SetNextWindowClass(&window_class_dockable);
        ImGui::Begin("Right Panel", nullptr);
        ImGui::Text("Right Panel Controls");
        if(ImGui::Button("Test"))
        {
            app_state.secondOpen = !app_state.secondOpen;
        }
        ImGui::End();

        ImGui::SetNextWindowClass(&window_class_dockable);
        ImGui::Begin("Bottom Panel", nullptr);
        ImGui::Text("Bottom Panel Controls");
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui::EndFrame();


        // rendering

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::renderGL()
    {
        // 1st attribute buffer : vertices
        glViewport(0, 0, image_data.width, image_data.height);
        glBindFramebuffer(GL_FRAMEBUFFER, image_data.framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, rendering_context.vertex_buffer);
        glVertexAttribPointer(
                0,                // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                // size
                GL_FLOAT,         // type
                GL_FALSE,         // normalized?
                0,                // stride
                (void *) nullptr  // array buffer offset
        );

        // Draw the triangle !
        glUseProgram(rendering_context.shader_program);
        glDrawArrays(GL_LINE_STRIP, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
    }

    void Application::render() {
        if(app_state.resize_queued)
            updateTexture();

        renderGL();
        renderGUI();

        auto io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }

        SDL_GL_SwapWindow(rendering_context.window_handle);


    }


}