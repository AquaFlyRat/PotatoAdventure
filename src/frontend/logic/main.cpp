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
static Graphics::Font mono_font_obj;
static Graphics::FontData main_font;
static Graphics::FontData mono_font;

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
    mono_font_obj.Open("assets/CatV_6x12_9.ttf", 12);
    mono_font_obj.SetHinting(Graphics::Font::Hinting::light);
    mono_font_obj.ExportGlyphs(img, mono_font, {256,32}, {256,256}, Utils::Encodings::cp1251());
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

    auto Tick = [&]
    {

    };
    auto Render = [&]
    {
        static constexpr const char *lorem_ipsum =
"\4Lorem ipsum\r dolor sit amet, consectetur adipisicing elit, "
"sed do eiusmod tempor incididunt ut \4labore\r et dolore magna aliqua. "
"Ut enim ad minim veniam, quis nostrud exercitation ullamco "
"laboris nisi ut \4aliquip\r ex ea commodo consequat. Duis aute irure "
"dolor in reprehenderit in voluptate velit esse cillum dolore eu "
"fugiat nulla pariatur. \4Excepteur\r sint occaecat cupidatat non "
"proident, sunt in culpa qui officia deserunt \4mollit\r anim id est laborum.";

        Graphics::Clear();

        renderer->SetColorMatrix(fmat4::rotate({1,1,1}, Sys::TickCounter() / 30.0));
        float angle = Sys::TickCounter() % 200 / float(200) * f_pi * 2;
        renderer->Sprite(ivec2(fmat2::rotate2D(angle) /mul/ fvec2(0,64)) + screen_size / 2, {32,32}).tex({0,0}).center().angle(-angle);
        renderer->Sprite({float(Input::MousePos().x), 0}, {1,screen_size.y}).color({0.5,0,0});
        renderer->Sprite({float(screen_size.x-Input::MousePos().x-2), 0}, {1,screen_size.y}).color({0.5,0,0});

        /*
        renderer->Text(screen_size/2, "Hello, world!\nWe have \1colors\r, \2mono\r, \3italic\r, \4bold\r, \5shadows\r, \6sparse\r, and \7this thing\r.\n1234\n#\xcf\x97#")
            .color({1,1,1})
            .configure_style(1).color({0,0,1})
            .configure_style(2).font(mono_font)
            .configure_style(3).italic(.25)
            .configure_style(4).bold_aligned(1)
            .configure_style(5).shadow({1,1}).shadow_color({0.75,0.25,0.25})
            .configure_style(6).spacing(3)
            .configure_style(7).glyph_matrix(fmat2::rotate2D(f_pi));
        */

        auto s = renderer->Text()
            .color({1,1,1})
            .configure_style(1).color({0,0,1})
            .configure_style(2).font(mono_font)
            .configure_style(3).italic(.25)
            .configure_style(4).bold_aligned(1)
            .configure_style(5).shadow({1,1}).shadow_color({0.75,0.25,0.25})
            .configure_style(6).spacing(3)
            .configure_style(7).glyph_matrix(fmat2::rotate2D(f_pi))
            .export_styles();

        //renderer->Text(screen_size/2, "Hello, world!\nWe have \1colors\r, \2mono\r, \3italic\r, \4bold\r, \5shadows\r, \6sparse\r, and \7this thing\r.\n1234\n#\xcf\x97#").styles(s).global_alpha(Input::MousePos().x / 100.f - 1);
        int w = 2*std::max(Input::MousePos().x - screen_size.x/2, 0);
        std::string tmp = Renderer2D::Text::InsertLineBreaksToFit(s, lorem_ipsum, w);
        renderer->Text({float(screen_size.x/2), 0}, tmp).styles(s).align_v(-1);
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
