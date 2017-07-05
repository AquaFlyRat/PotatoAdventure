#include "../master.h"

#include "gui.h"

#include <deque>
#include <vector>

namespace Backend
{
    void Start();
    void Tick();
}


void Boot();
void Main();


static int window_scale = 2;
static constexpr ivec2 screen_size(480,270);

namespace Cfg
{
    static constexpr int log_x = 146, log_width = 334,
                         log_text_margin_bottom = 34, log_text_margin_left = 8, log_text_margin_right = 8,
                         log_text_width = log_width - log_text_margin_left - log_text_margin_right;

    static constexpr float log_text_typing_speed = 0.4,
                           log_text_insertion_offset_pixel_speed = 0.25, log_text_insertion_offset_frac_change_per_frame = 0.025;
}


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
static Renderer2D::Text::StyleVector main_font_style_vec;

static std::vector<std::string> log_lines;
static std::deque<std::string> log_queue;
static float log_tmp_offset_y = 0;
static float log_queue_current_str_pos = 0;
static int log_queue_front_visible_char_count = -1;

static int log_position = 0;

namespace GUI
{
    Renderer2D &Renderer() {return *renderer;}

    namespace Fonts
    {
        const Graphics::FontData &Main() {return main_font;}
        const Graphics::FontData &Mono() {return mono_font;}
    }

    namespace FontStyleVectors
    {
        const Renderer2D::Text::StyleVector &Main() {return main_font_style_vec;}
    }

    void WriteLine(std::string_view line) // Line feed is automatically added at the end.
    {
        std::string str = Renderer2D::Text::InsertLineBreaksToFit(main_font_style_vec, line, Cfg::log_text_width);
        char *it = str.data();
        const char *word_start = str.c_str();
        while (1)
        {
            bool end = 0;

            if (*it == '\n')
                *it = '\0';
            else if (*it == '\0')
                end = 1;

            if (*it == '\0')
            {
                log_queue.emplace_back(word_start);
                if (!end)
                    word_start = it+1;
            }

            if (end)
                return;

            it++;
        }
    }
}

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


    main_font_style_vec = renderer->Text()
        .color({1,1,1})
        .configure_style(1).bold_aligned(1)
        .configure_style(2).italic(0.25)
        .export_styles();

    Sys::SetCurrentFunction(Main);
}

void Main()
{
    Backend::Start();

    auto Tick = [&]
    {
        if (log_queue.empty())
            Backend::Tick();

        // Updating rendering queue
        if (log_queue.size())
        {

            if (log_queue_front_visible_char_count == -1)
                log_queue_front_visible_char_count = Renderer2D::Text::VisibleCharCount(log_queue.front());

            if (log_queue_current_str_pos > log_queue_front_visible_char_count)
            {
                log_lines.emplace_back((std::string &&)log_queue.front());
                log_queue.pop_front();
                log_tmp_offset_y += 1;
                log_queue_current_str_pos = 0;
                log_queue_front_visible_char_count = -1;
            }
            else
                log_queue_current_str_pos += Cfg::log_text_typing_speed;
        }
        if (log_tmp_offset_y > 0)
        {
            log_tmp_offset_y -= log_tmp_offset_y * Cfg::log_text_insertion_offset_frac_change_per_frame + Cfg::log_text_insertion_offset_pixel_speed/main_font.LineSkip();
            if (log_tmp_offset_y < 0)
                log_tmp_offset_y = 0;
        }

        //if (Input::KeyPressed(Input::Key_Space())) GUI::WriteLine("Meow meow!");
    };
    auto Render = [&]
    {
        Graphics::Clear();

        // This separates log from map and stats;
        renderer->Sprite({float(Cfg::log_x), 0}, {1,(float)screen_size.y}).color({0.5,0.5,0.5});

        // Log lines
        if (log_lines.size())
        {
            for (int i = 0; Cfg::log_text_margin_bottom + main_font.LineSkip()*(i-1) < screen_size.y && i + log_position / main_font.LineSkip() < int(log_lines.size()); i++)
            {
                fvec2 line_pos(Cfg::log_x + Cfg::log_text_margin_left, std::round(screen_size.y - Cfg::log_text_margin_bottom - main_font.LineSkip() * (i + 1 - log_tmp_offset_y)));
                renderer->Text(line_pos, log_lines[log_lines.size() - 1 - i - log_position / main_font.LineSkip()])
                    .styles(main_font_style_vec)
                    .align_h(-1).align_v(-1);
            }
        }

        // Current unfinished line
        if (log_queue.size())
        {
            renderer->Text(fvec2(Cfg::log_x + Cfg::log_text_margin_left, std::round(screen_size.y - Cfg::log_text_margin_bottom + main_font.LineSkip() * log_tmp_offset_y)), log_queue.front())
                .styles(main_font_style_vec)
                .align_h(-1).align_v(-1)
                .character_count(log_queue_current_str_pos);
        }
    };

    while (1)
    {
        Sys::BeginFrame();

        while (ts.Tick())
        {
            Sys::Tick();
            Tick();
        }

        Render();
        renderer->Flush();


        Sys::EndFrame();
    }
}
