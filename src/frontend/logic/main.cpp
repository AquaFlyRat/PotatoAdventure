#include "../master.h"


namespace Backend
{
    void Start();
    void Tick();
}


void Boot();
void Main();


static int window_scale = 2;
static constexpr ivec2 screen_size(480,270);



void PreInit()
{
    Sys::Config::ApplicationName("Potato Adventure");
    Window::Init::Name("Potato Adventure");
    Window::Init::Size(screen_size * window_scale);
    Window::Init::OpenGL::Vsync(Window::ContextSwapMode::late_swap_tearing);
    Sys::SetCurrentFunction(Boot);

    glfl::check_errors = 1;
    glfl::terminate_on_error = 1;
}

static Utils::TickStabilizer ts(60, 8);
static Graphics::Texture *main_texture;
static Renderer2D *renderer;

void Resize()
{
    renderer->UpdateViewport(window_scale);
}

void Boot()
{
    MarkLocation("Boot");

    Graphics::Blend::Enable();

    main_texture = new Graphics::Texture(Graphics::ImageData::FromPNG("assets/texture.png"));
    main_texture->LinearInterpolation(0);

    renderer = new Renderer2D(screen_size);
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

        //renderer->SetColorMatrix(fmat4::rotate({1,1,1}, Sys::TickCounter() / 30.0));

        renderer->Sprite(ivec2(fmat2::rotate2D(Sys::TickCounter() % 200 / float(200) * f_pi * 2) /mul/ fvec2(0,64)) + screen_size / 2, {32,32}).tex({0,0}).center().angle(Sys::TickCounter() % 200 / float(200) * f_pi * -2);
    };

    while (1)
    {
        Sys::BeginFrame();

        while (ts.Tick())
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
