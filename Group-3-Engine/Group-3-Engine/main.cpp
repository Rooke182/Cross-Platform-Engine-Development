#define STB_IMAGE_IMPLEMENTATION
#include "Application.h"

int main()
{
    std::srand(std::time(nullptr));
    Application* app = new Application();
    app->Init();
    app->Loop();
    app->Cleanup();
    return 0;
}


