#include "../master.h"


namespace Backend
{
    void Start();
    void Tick();
}


void Boot();
void Main();


static int window_scale = 3;
static constexpr ivec2 ref_window_sz(480,270);



void PreInit()
{
    Sys::Config::ApplicationName("Potato Adventure");
    Window::Init::Name("Potato Adventure");
    Window::Init::Size(ref_window_sz * window_scale);
    Sys::SetCurrentFunction(Boot);
}

void Resize()
{
    Graphics::ViewportFullscreen();
}

Utils::TickStabilizer *ts;

void Boot()
{
    MarkLocation("Boot");

    ts = new Utils::TickStabilizer(60, 8);

    Sys::SetCurrentFunction(Main);
}

void Main()
{
    Backend::Start();

    //ts = new Utils::TickStabilizer();
    while (1)
    {
        Sys::BeginFrame();
        while (ts->Tick())
        {
            Sys::Tick();

        }


        Sys::EndFrame();
    }
}
