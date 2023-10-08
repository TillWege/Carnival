#include "core/Application.h"

using namespace carnival::core;

Application* app;

int main(int argc, char* args[])
{
    app = new Application();
    app->setupTriangle();
    app->setupImage();
    app->Run();
    delete app;
    return 0;
}


