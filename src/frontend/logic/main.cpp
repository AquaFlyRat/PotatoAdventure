#include "../master.h"


namespace Backend
{
    void Start();
    void Tick();
}


void Boot();
void Main();


static int window_scale = 2;
static constexpr ivec2 default_window_size(480,270);



void PreInit()
{
    Sys::Config::ApplicationName("Potato Adventure");
    Window::Init::Name("Potato Adventure");
    Window::Init::Size(default_window_size * window_scale);
    Sys::SetCurrentFunction(Boot);

    glfl::check_errors = 1;
    glfl::terminate_on_error = 1;
}

static Utils::TickStabilizer *ts;
static Graphics::Texture *main_texture;
static Renderer2D *renderer;

void Resize()
{
    renderer->UpdateViewport(window_scale);
}

void Boot()
{
    MarkLocation("Boot");

    ts = new Utils::TickStabilizer(60, 8);

    Graphics::Blend::Enable();

    main_texture = new Graphics::Texture(Graphics::ImageData::FromPNG("assets/texture.png"));
    main_texture->LinearInterpolation(0);

    renderer = new Renderer2D(default_window_size);
    renderer->SetTexture(*main_texture);
    renderer->SetBlendingMode();
    renderer->UseMouseMapping(1);
    renderer->EnableShader();

    Sys::SetCurrentFunction(Main);
}

void Main()
{
    Backend::Start();
    auto Tick = [&]
    {

    };
    auto Render = [&]
    {
        Graphics::Clear();

        renderer->SetColorMatrix(fmat4::rotate({1,1,1}, Sys::TickCounter() / 10.0));

        renderer->Sprite(Input::MousePos(), {32,16}).tex({0,0});
    };

    while (1)
    {
        Sys::BeginFrame();
        while (ts->Tick())
        {
            Sys::Tick();
            Backend::Tick();
            Tick();
        }
        Render();
        renderer->Flush();


        Sys::EndFrame();
    }
}
