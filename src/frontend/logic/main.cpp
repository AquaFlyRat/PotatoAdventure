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
static Graphics::Font main_font_obj;
static Graphics::FontData main_font;

void Resize()
{
    renderer->UpdateViewport(window_scale);
}

void Boot()
{
    MarkLocation("Boot");

    Graphics::Blend::Enable();

    auto img = Graphics::ImageData::FromPNG("assets/texture.png");
    main_font_obj.Open("assets/Cat12.ttf", 12);
    main_font_obj.SetHinting(Graphics::Font::Hinting::light);
    main_font_obj.ExportGlyphs(img, main_font, {64,32}, {256,256}, Utils::Encodings::cp1251());
    main_texture = new Graphics::Texture(img);
    main_texture->LinearInterpolation(0);

    renderer = new Renderer2D(screen_size);
    renderer->SetTexture(*main_texture);
    renderer->SetDefaultFont(main_font);
    renderer->SetBlendingMode();
    renderer->UseMouseMapping(1);
    renderer->EnableShader();

    Sys::SetCurrentFunction(Main);
}

void Main()
{
    Backend::Start();
std::cout << main_font.Height();
    auto Tick = [&]
    {

    };
    auto Render = [&]
    {
        Graphics::Clear();

        renderer->SetColorMatrix(fmat4::rotate({1,1,1}, Sys::TickCounter() / 30.0));
        float angle = Sys::TickCounter() % 200 / float(200) * f_pi * 2;
        renderer->Sprite(ivec2(fmat2::rotate2D(angle) /mul/ fvec2(0,64)) + screen_size / 2, {32,32}).tex({0,0}).center().angle(-angle);
        renderer->Text(screen_size/2, "Hello, world!\n1234\n#\xcf\x97#").color({1,1,1}).shadow({1,1}).shadow_color({0.5,0,0}).bold_no_offset((Input::MousePos().x - screen_size.x/2) / 100.0);
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
