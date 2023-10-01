#include "core/Application.h"

using namespace cpp_ge::core;

Application* app;

int main(int argc, char* args[])
{
    app = new Application();
    app->setupTriangle();
    app->Run();
    delete app;
    return 0;
}


