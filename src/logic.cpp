#include "master.h"

void Boot();

void PreInit()
{
    Sys::SetCurrentFunction(Boot);
    Window::Init::Resizable(1);
}

void Resize()
{
    Graphics::ViewportFullscreen();
}

void Boot()
{
    MarkLocation("Boot");

    while (1)
    {
        Sys::BeginFrame();
        Sys::Tick();
        constexpr int period = 200;
        float f = std::cos(Sys::FrameCounter() % period / float(period) * pi<float>() * 2) * 0.5 + 0.5;
        glClearColor(f, f/2, 1 - f, 1);

        Sys::EndFrame();
    }

    Sys::Exit();
}