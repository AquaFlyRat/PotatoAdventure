#include "../master.h"

#include "gui.h"

#include <deque>

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
                         log_text_width = log_width - log_text_margin_left - log_text_margin_right,
                         log_text_height = 270-log_text_margin_bottom;

    static constexpr float log_text_typing_speed = 0.4, log_text_typing_speed_high = 2,
                           log_text_insertion_offset_pixel_speed = 0.25, log_text_insertion_offset_frac_change_per_frame = 0.025;

    static constexpr fvec3 color_log_background(0,0,0),
                           color_log_boundary(0.5,0.5,0.5),
                           color_log_boundary_dark(0.2,0.2,0.2),
                           color_log_text(1,1,1);
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

static std::deque<std::string> log_queue;

namespace GUI
{
    // Low level

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


    // High level

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
        .color(Cfg::color_log_text)
        .shadow({1,1}).shadow_color({0.1f,0.1f,0.1f})
        .configure_style(1).bold_aligned(1)
        .configure_style(2).italic(0.25)
        .configure_style(3).color({0.2f,1,0.2f})
        .configure_style(4).color({1,1,0.2f})
        .configure_style(5).color({1,0.2f,0.2f})
        .export_styles();

    Sys::SetCurrentFunction(Main);
}

void Main()
{
    Backend::Start();

    std::deque<std::string> log_lines;
    float log_tmp_offset_y = 0;
    float log_queue_current_str_pos = 0;
    int log_queue_front_visible_char_count = -1;

    int log_position = 0;

    int log_pixel_len;
    int log_scrollbar_height;

    const int text_input_y = screen_size.y - Cfg::log_text_margin_bottom + main_font.LineSkip();

    auto Tick = [&]
    {
        if (log_queue.empty())
            Backend::Tick();

        // Updating log queue
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
                log_queue_current_str_pos += (Input::AnyKeyDown() ? Cfg::log_text_typing_speed_high : Cfg::log_text_typing_speed);
        }
        if (log_tmp_offset_y > 0)
        {
            log_tmp_offset_y -= log_tmp_offset_y * Cfg::log_text_insertion_offset_frac_change_per_frame + Cfg::log_text_insertion_offset_pixel_speed/main_font.LineSkip();
            if (log_tmp_offset_y < 0)
                log_tmp_offset_y = 0;
        }

        log_pixel_len = std::max(1u, log_lines.size()) * main_font.LineSkip();
        log_scrollbar_height = std::min(screen_size.y, (int)std::lround(screen_size.y * Cfg::log_text_height / float(log_pixel_len)));

        //log_position += (Input::KeyDown(Input::Key_Up()) - Input::KeyDown(Input::Key_Down()));

        if (log_position > int(log_lines.size()) * main_font.LineSkip() - Cfg::log_text_height)
            log_position = int(log_lines.size()) * main_font.LineSkip() - Cfg::log_text_height;
        if (log_position < 0)
            log_position = 0;

        //if (Input::KeyDown(Input::Key_Enter())) GUI::WriteLine("Meow!");
    };
    auto Render = [&]
    {
        Graphics::Clear();

        // Log background
        renderer->Sprite({float(Cfg::log_x), 0}, fvec2(Cfg::log_width, screen_size.y)).color(Cfg::color_log_background);


        // Log lines
        if (log_lines.size())
        {
            int max_visible_line = std::min(int(log_lines.size()), int(std::lround(std::ceil((log_position+log_tmp_offset_y) / float(main_font.LineSkip())))) + screen_size.y / main_font.LineSkip() + 2);
            int min_visible_line = std::max(0, int(std::lround(log_position / float(main_font.LineSkip())) - 2));
            for (int i = min_visible_line; i < max_visible_line; i++)
            {
                renderer->Text({float(Cfg::log_x + Cfg::log_text_margin_left), log_position + std::round(screen_size.y - Cfg::log_text_margin_bottom - main_font.LineSkip() * (i + 1 - log_tmp_offset_y))}, log_lines[log_lines.size() - 1 - i])
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


        // Text input background
        renderer->Sprite(fvec2(Cfg::log_x, text_input_y), fvec2(Cfg::log_width, screen_size.y - text_input_y)).color(Cfg::color_log_background);
        // Line separating log from text input
        renderer->Sprite(fvec2(Cfg::log_x, text_input_y), fvec2(Cfg::log_width-2,1)).color(Cfg::color_log_boundary_dark);

        // Line separating log from map and stats;
        renderer->Sprite({float(Cfg::log_x), 0}, {1,float(screen_size.y)}).color(Cfg::color_log_boundary);
        // Line separating log scrollbar
        renderer->Sprite({float(Cfg::log_x+Cfg::log_width - 2), 0}, {1,float(screen_size.y)}).color(Cfg::color_log_boundary);

        // Scrollbar
        renderer->Sprite(fvec2(Cfg::log_x + Cfg::log_width - 1,
                               screen_size.y - log_scrollbar_height - std::min(screen_size.y - log_scrollbar_height,
                                                                               (int)std::lround(screen_size.y * log_position / float(log_pixel_len)))),
                         fvec2(1, log_scrollbar_height))
            .color(Cfg::color_log_text);
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
