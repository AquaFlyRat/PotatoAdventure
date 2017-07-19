#ifndef RENDERER2D_H_INCLUDED
#define RENDERER2D_H_INCLUDED

#include <algorithm>

#include "graphics.h"
#include "math.h"
#include "os.h"
#include "system.h"
#include "utils.h"

class Renderer2D
{
  public:
    using VertexFormat = Graphics::VertexFormat<fvec2,  // pos
                                                fvec4,  // color
                                                fvec2,  // texture coords
                                                fvec3>; // x - texture color factor (0 - fill color, 1 - texture color)
                                                        // y - texture alpha factor (0 - fill alpha, 1 - texture alpha)
                                                        // z - post alpha factor (1 - normal drawing, 0 - additive blending)


  private:
    ivec2 size;
    bool use_mouse_mapping = 0;
    Utils::Object<Graphics::RenderQueue<VertexFormat, 3>> render_queue;
    Utils::Object<Graphics::Shader> main_shader;
    int current_tex = -1;
    ivec2 current_tex_size;

    const Graphics::FontData *default_font = 0;

  public:
    static constexpr int default_render_queue_size = 0x10000;
    static const Graphics::ShaderSource &DefaultShaderSource();

    Renderer2D(ivec2 render_size, int render_queue_size = default_render_queue_size, const Graphics::ShaderSource &shader_src = DefaultShaderSource())
    {
        size = render_size;
        render_queue.create(render_queue_size);
        main_shader.create("Renderer2D shader", shader_src);
        main_shader->SetUniform<fmat4>(0, fmat4::ortho({0,float(size.y)}, {float(size.x), 0}, -1, 1));
        ResetColorMatrix();
    }

    void UseMouseMapping(bool n)
    {
        use_mouse_mapping = n;
    }

    void UpdateViewport(float scale = 1)
    {
        ivec2 sz = (size * scale).apply((long(*)(double))lround);
        ivec2 pos = Window::Size() / 2 - sz / 2;
        Graphics::Viewport(pos, sz);
        if (use_mouse_mapping)
            Input::SetMouseMapping(pos, 1 / scale);
    }

    void SetTexture(const Graphics::Texture &tex)
    {
        if (current_tex != tex.Slot())
        {
            current_tex = tex.Slot();
            Flush();
            main_shader->SetUniform<Graphics::Texture>(1, tex);
        }
        if (current_tex_size != tex.Size())
        {
            current_tex_size = tex.Size();
            Flush();
            main_shader->SetUniform<fvec2>(2, tex.Size());
        }
    }

    void SetDefaultFont(const Graphics::FontData &font) // The font data object must remain alive while you render text with it.
    {
        default_font = &font;
    }

    void EnableShader()
    {
        main_shader->Use();
    }

    void SetBlendingMode()
    {
        Graphics::Blend::Presets::FuncNormal_Pre();
    }

    void Flush()
    {
        if (!render_queue->Empty())
            render_queue->Flush();
    }

    void SetColorMatrix(fmat4 m)
    {
        Flush();
        main_shader->SetUniform<fmat4>(3, m);
    }
    void ResetColorMatrix()
    {
        SetColorMatrix(fmat4::identity());
    }



    class Sprite
    {
        Sprite(const Sprite &) = delete;
        Sprite(Sprite &&) = delete;
        Sprite &operator=(const Sprite &) = delete;
        Sprite &operator=(Sprite &&) = delete;

        using rvalue = Sprite &&;

        Renderer2D *renderer;

        fvec2 dst_pos, dst_size;

        bool have_texture = 0;
        fvec2 texture_pos = {0,0}, texture_size = {1,1};

        bool have_center = 0;
        fvec2 sprite_center = {0,0};

        bool have_matrix = 0;
        fmat2 sprite_matrix = fmat2::identity();

        bool have_color = 0;
        fvec3 sprite_colors[4] = {fvec3(0),fvec3(0),fvec3(0),fvec3(0)};

        bool have_tex_color_fac = 0;
        float tex_color_factors[4] = {1,1,1,1};

        float sprite_alpha[4] = {1,1,1,1};
        float sprite_opacity[4] = {1,1,1,1};

        bool absolute_pos = 0;
        bool absolute_tex_pos = 0;

        bool sprite_flip_x = 0, sprite_flip_y = 0;

      public:
        Sprite(Renderer2D *r, fvec2 pos, fvec2 sz) : renderer(r), dst_pos(pos), dst_size(sz) {}

        rvalue tex(fvec2 pos, fvec2 sz)
        {
            Assert("2D renderer: Sprite texture specified twice.", !have_texture);
            have_texture = 1;

            texture_pos = pos;
            texture_size = sz;
            return (rvalue)*this;
        }
        rvalue tex(fvec2 pos)
        {
            Assert("2D renderer: Sprite texture specified twice.", !have_texture);
            have_texture = 1;

            texture_pos = pos;
            texture_size = dst_size;
            return (rvalue)*this;
        }
        rvalue center(fvec2 c)
        {
            Assert("2D renderer: Sprite center specified twice.", !have_center);
            have_center = 1;

            sprite_center = c;
            return (rvalue)*this;
        }
        rvalue center()
        {
            Assert("2D renderer: Sprite center specified twice.", !have_center);
            have_center = 1;

            sprite_center = dst_size / 2;
            return (rvalue)*this;
        }
        rvalue angle(float a) // Uses `matrix()`.
        {
            matrix(fmat2::rotate2D(a));
            return (rvalue)*this;
        }
        rvalue matrix(fmat2 m) // This can be called multiple times, resulting in multiplying matrices in the order they were passed.
        {
            if (have_matrix)
                sprite_matrix = sprite_matrix /mul/ m;
            else
                sprite_matrix = m;
            have_matrix = 1;
            return (rvalue)*this;
        }
        rvalue color(fvec3 c)
        {
            Assert("2D renderer: Sprite color specified twice.", !have_color);
            have_color = 1;

            for (auto &it : sprite_colors)
                it = c;
            return (rvalue)*this;
        }
        rvalue color(fvec3 a, fvec3 b, fvec3 c, fvec3 d)
        {
            Assert("2D renderer: Sprite color specified twice.", !have_color);
            have_color = 1;

            sprite_colors[0] = a;
            sprite_colors[1] = b;
            sprite_colors[2] = c;
            sprite_colors[3] = d;
            return (rvalue)*this;
        }
        rvalue mix(float x) // 0 - fill with color (default if color is provided), 1 - use texture
        {
            Assert("2D renderer: Sprite color mix factor specified twice.", !have_tex_color_fac);
            have_tex_color_fac = 1;

            for (auto &it : tex_color_factors)
                it = x;
            return (rvalue)*this;
        }
        rvalue mix(float a, float b, float c, float d)
        {
            assert(!have_tex_color_fac);
            have_tex_color_fac = 1;

            tex_color_factors[0] = a;
            tex_color_factors[1] = b;
            tex_color_factors[2] = c;
            tex_color_factors[3] = d;
            return (rvalue)*this;
        }
        rvalue alpha(float a)
        {
            for (auto &it : sprite_alpha)
                it = a;
            return (rvalue)*this;
        }
        rvalue alpha(float a, float b, float c, float d)
        {
            sprite_alpha[0] = a;
            sprite_alpha[1] = b;
            sprite_alpha[2] = c;
            sprite_alpha[3] = d;
            return (rvalue)*this;
        }
        rvalue opacity(float o) // 1 - normal blending, 0 - additive blending
        {
            for (auto &it : sprite_opacity)
                it = o;
            return (rvalue)*this;
        }
        rvalue opacity(float a, float b, float c, float d) // 1 - normal blending, 0 - additive blending
        {
            sprite_opacity[0] = a;
            sprite_opacity[1] = b;
            sprite_opacity[2] = c;
            sprite_opacity[3] = d;
            return (rvalue)*this;
        }
        rvalue absolute(bool x = 1) // Interpret size as a position of the second corner
        {
            absolute_pos = x;
            return (rvalue)*this;
        }
        rvalue absolute_tex(bool x = 1) // Interpret texture size as a position of the second corner
        {
            absolute_tex_pos = x;
            return (rvalue)*this;
        }
        rvalue flip_x(bool f = 1) // Flips texture horizontally if it was specified. Updates the center accordingly if it was specified.
        {
            sprite_flip_x = f;
            return (rvalue)*this;
        }
        rvalue flip_y(bool f = 1) // Flips texture vertically if it was specified. Updates the center accordingly if it was specified.
        {
            sprite_flip_y = f;
            return (rvalue)*this;
        }

        ~Sprite()
        {
            Assert("2D renderer: Attempt to render a sprite with no texture nor color specified.", have_texture || have_color);
            Assert("2D renderer: Attempt to render a sprite with absolute corner coodinates with a center specified.", absolute_pos + have_center < 2);
            Assert("2D renderer: Attempt to render a sprite with both texture and color specified, but without a mixing factor.", (have_texture && have_color) <= have_tex_color_fac);
            Assert("2D renderer: Attempt to render a sprite with a matrix but without a center specified.", have_matrix <= have_center);

            if (absolute_pos)
            {
                dst_size -= dst_pos;
            }
            if (absolute_tex_pos)
            {
                assert(have_texture);
                texture_size -= texture_pos;
            }

            fvec4 final_colors[4];
            fvec3 factors[4];

            if (!have_texture)
            {
                if (!have_tex_color_fac)
                {
                    for (auto &it : factors)
                        it.x = 0;
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                        factors[i].x = tex_color_factors[i];
                }

                for (int i = 0; i < 4; i++)
                {
                    final_colors[i] = sprite_colors[i].to_vec4(sprite_alpha[4]);
                    factors[i].y = 0;
                }
            }
            else
            {
                for (int i = 0; i < 4; i++)
                {
                    factors[i].x = tex_color_factors[i];
                    final_colors[i] = sprite_colors[i].to_vec4(0);
                    factors[i].y = sprite_alpha[i];
                }

                sprite_center = sprite_center * dst_size / texture_size;
            }
            for (int i = 0; i < 4; i++)
                factors[i].z = sprite_opacity[i];

            if (sprite_flip_x)
            {
                texture_pos.x += texture_size.x;
                texture_size.x = -texture_size.x;
                if (have_center)
                    sprite_center.x = dst_size.x - sprite_center.x;
            }
            if (sprite_flip_y)
            {
                texture_pos.y += texture_size.y;
                texture_size.y = -texture_size.y;
                if (have_center)
                    sprite_center.y = dst_size.y - sprite_center.y;
            }

            fvec2 a = -sprite_center;
            fvec2 c = dst_size - sprite_center;

            fvec2 b = {a.x, c.y};
            fvec2 d = {c.x, a.y};

            if (have_matrix)
            {
                a = sprite_matrix /mul/ a;
                b = sprite_matrix /mul/ b;
                c = sprite_matrix /mul/ c;
                d = sprite_matrix /mul/ d;
            }

            fvec2 tex_a = texture_pos;
            fvec2 tex_c = texture_pos + texture_size;

            fvec2 tex_b = {tex_a.x, tex_c.y};
            fvec2 tex_d = {tex_c.x, tex_a.y};

            renderer->render_queue->Insert({dst_pos + a, final_colors[0], tex_a, factors[0]},
                                           {dst_pos + b, final_colors[1], tex_b, factors[1]},
                                           {dst_pos + c, final_colors[2], tex_c, factors[2]},
                                           {dst_pos + d, final_colors[3], tex_d, factors[3]});
        }
    };
    using Sprite_t = Sprite; // For cases when `Sprite` conflicts with the function name.

    class Text
    {
        Text(Text &&) = delete;
        Text &operator=(const Text &) = delete;
        Text &operator=(Text &&) = delete;

        Text(const Text &) = default;

        using rvalue = Text &&;

        Renderer2D *renderer;

        fvec2 position;
        std::string_view text;

        float text_alpha = 1;

        float text_char_count = -1;

        fmat2 text_matrix = fmat2::identity();
        ivec2 text_alignment = {0,0};

      public:
        struct Shadow
        {
            enum Type {add, set_color, set_alpha, set_opacity, mode_global, mode_text, mode_glyph};
            Type type;
            fvec3 value;
        };

        enum class RefFrame
        {
            global = Shadow::mode_global,
            text   = Shadow::mode_text,
            glyph  = Shadow::mode_glyph,
        };

        struct Style
        {
            Style(bool was_initialized = 0) : was_initialized(was_initialized) {}

            bool was_initialized = 0;

            const Graphics::FontData *font;

            fvec3 color = {1,1,1};
            float alpha = 1;
            float opacity = 1;

            fvec2 spacing = {0,0};

            fmat2 glyph_matrix = fmat2::identity();
            fvec2 glyph_offset = {0,0};

            RefFrame duplicate_ref_frame = RefFrame::glyph;
            std::vector<fvec2> duplicate_offsets;

            std::vector<Shadow> shadows;
        };

        using StyleVector = std::vector<Style>;

      private:
        int text_cur_style = 0;
        StyleVector text_styles = {{1}};

        bool internal_temporary_object = 0; // Used by shadows and `extend()`ed copies.
        fvec2 internal_line_pos;

        bool internal_stub_object; // For stub objects used only to edit styles.

        static bool internal_IsStyleMarker(char ch)
        {
            return ch == '\r' || (ch >= '\1' && ch <= '\7');
        }
        static int internal_StyleMarkerToIndex(char ch)
        {
            if (ch == '\r')
                return 0;
            return ch;
        }

        static float internal_LineWidth(const StyleVector &style_list, std::string_view str, int state, int *final_state = 0)
        {
            float ret = -style_list[state].spacing.x / 2;
            uint16_t prev_ch = u8invalidchar;
            for (auto it = str.begin(); it != str.end(); it++)
            {
                if (*it == '\n')
                    break;
                else if (internal_IsStyleMarker(*it))
                {
                    state = internal_StyleMarkerToIndex(*it);
                    prev_ch = u8invalidchar;
                }
                else
                {
                    if (!u8firstbyte(it))
                        continue;
                    if (int(u8charlen(it)) > str.end() - it)
                        Sys::Error("Invalid UTF-8.");
                    uint16_t ch = u8decode(it);
                    if (!style_list[state].font->GlyphExists(ch))
                        ch = 0;
                    ret += style_list[state].font->Glyph(ch).advance + style_list[state].spacing.x;
                    if (prev_ch != u8invalidchar)
                        ret += style_list[state].font->Kerning(prev_ch, ch);
                    prev_ch = ch;
                }
            }
            if (final_state)
                *final_state = state;
            return ret - style_list[state].spacing.x / 2;
        }
        static float internal_BaselineHeight(const StyleVector &style_list, std::string_view str, int state, int *final_state = 0)
        {
            float ret = 0;
            for (char it : str)
            {
                if (it == '\n')
                    ret += style_list[state].font->LineSkip() + style_list[state].spacing.y;
                else if (internal_IsStyleMarker(it))
                    state = internal_StyleMarkerToIndex(it);
            }
            if (final_state)
                *final_state = state;
            return ret - style_list[state].spacing.y;
        }

      public:

        Text(Renderer2D *r, fvec2 pos, std::string_view text, bool stub = 0) : renderer(r), position(pos), text(text), internal_stub_object(stub)
        {
            text_styles[0].font = r->default_font;
        }


        // Utilities

        static float LineWidth(const StyleVector &style_list, std::string_view str)
        {
            return internal_LineWidth(style_list, str, 0);
        }
        static float Height(const StyleVector &style_list, std::string_view str)
        {
            int final_state;
            float ret = internal_BaselineHeight(style_list, str, 0, &final_state);
            return ret + style_list[0].font->Ascent() + style_list[final_state].font->Descent();
        }
        static int VisibleCharCount(std::string_view str)
        {
            int ret = 0;
            for (char it : str)
            {
                if (u8firstbyte(it) && it != '\n' && !internal_IsStyleMarker(it))
                    ret++;
            }
            return ret;
        }
        static std::string InsertLineBreaksToFit(const StyleVector &style_list, std::string_view str, float max_width)
        {
            std::string ret;
            ret.reserve(str.size());

            int state = 0;

            float line_width = -style_list[state].spacing.x / 2;

            uint16_t prev_ch = u8invalidchar;

            int current_line_start = 0;

            bool word_found_on_this_line = 0;

            auto RemoveTrailingWhitespaces = [&]
            {
                std::size_t index = ret.find_last_not_of(' ');
                if (index == std::string::npos)
                    ret.clear();
                else if (index+1 != ret.size())
                    ret.resize(index+1);
            };
            auto NextLine = [&]
            {
                prev_ch = u8invalidchar;
                line_width = -style_list[state].spacing.x / 2;
                ret += '\n';
                current_line_start = ret.size();
                word_found_on_this_line = 0;
            };

            auto it = str.begin();
            while (it != str.end())
            {
                if (!u8firstbyte(it))
                {
                    ret += *it;
                    it++;
                    continue;
                }

                if (*it == '\n')
                {
                    RemoveTrailingWhitespaces();
                    NextLine();
                    it++;
                }
                else if (internal_IsStyleMarker(*it))
                {
                    state = internal_StyleMarkerToIndex(*it);
                    prev_ch = u8invalidchar;
                    ret += *it;
                    it++;
                }
                else
                {
                    if (int(u8charlen(it)) > str.end() - it)
                        Sys::Error("Invalid UTF-8.");
                    uint16_t ch = u8decode(it);
                    if (!style_list[state].font->GlyphExists(ch))
                        ch = 0;
                    float char_width = style_list[state].font->Glyph(ch).advance + style_list[state].spacing.x;
                    if (prev_ch != u8invalidchar)
                        char_width += style_list[state].font->Kerning(prev_ch, ch);

                    if (line_width + char_width + style_list[state].spacing.x / 2 > max_width && word_found_on_this_line)
                    {
                        if (ch == ' ') // If we hit width limit in a middle of a whitespace sequence.
                        {
                            RemoveTrailingWhitespaces();
                            it++;
                            while (it != str.end() && *it == ' ')
                                it++;
                            if (it != str.end())
                                NextLine();
                            if (it != str.end() && *it == '\n')
                                it++;
                        }
                        else // If we hit width limit in a middle of a word.
                        {
                            std::string_view line_view = ret;
                            line_view.remove_prefix(current_line_start);

                            std::size_t space_index = line_view.find_last_of(" ");

                            if (space_index == std::string::npos) // If there are no whitespaces in this line to turn into line feeds.
                            {
                                RemoveTrailingWhitespaces();
                                NextLine();
                            }
                            else // If there is a whitespace in this line to turn into a line feed.
                            {
                                while (space_index > 0 && line_view[space_index-1] == ' ')
                                    space_index--;
                                ret.resize(current_line_start + space_index);
                                NextLine();
                                it -= line_view.size() - space_index;
                                while (it != str.end() && *it == ' ')
                                    it++;
                            }
                        }
                    }
                    else
                    {
                        ret += *it;
                        line_width += char_width;
                        prev_ch = ch;
                        it++;
                        word_found_on_this_line = 1;
                    }
                }
            }
            RemoveTrailingWhitespaces();
            ret.shrink_to_fit();
            return ret;
        }

        // Style-independent settings

        rvalue global_alpha(float a)
        {
            text_alpha = a;
            return (rvalue)*this;
        }
        rvalue align_h(int a)
        {
            text_alignment.x = a;
            return (rvalue)*this;
        }
        rvalue align_v(int a)
        {
            text_alignment.y = a;
            return (rvalue)*this;
        }
        // Limits the amount of rendered characters (excluding line breaks and style markers).
        // If the parameter is not integral, there will be `ceil(count)` chars and the last one of them will have its alpha multiplied by `frac(count)`.
        // The default value is `-1` and means no limit on character count.
        rvalue character_count(float count)
        {
            text_char_count = count;
            return (rvalue)*this;
        }
        rvalue scale(float s) // Uses `matrix()`.
        {
            matrix(fmat2::scale2D(s));
            return (rvalue)*this;
        }
        rvalue angle(float a) // Uses `matrix()`.
        {
            matrix(fmat2::rotate2D(a));
            return (rvalue)*this;
        }
        rvalue matrix(fmat2 m) // This can be called multiple times, resulting in multiplying matrices in the order they were passed.
        {
            text_matrix = text_matrix /mul/ m;
            return (rvalue)*this;
        }

        // Those change spacing for all styles simultaneously.
        rvalue global_spacing(float pixels) // Negative values are allowed, but don't work well. The function can be called multiple times, accumulating the value.
        {
            for (auto &it : text_styles)
                if (it.was_initialized)
                    it.spacing.x += pixels;
            return (rvalue)*this;
        }
        rvalue global_vertical_spacing(float pixels) // Negative values are allowed, but don't work well. The function can be called multiple times, accumulating the value.
        {
            for (auto &it : text_styles)
                if (it.was_initialized)
                    it.spacing.y += pixels;
            return (rvalue)*this;
        }


        // Styles

        // `index` must be in range [0;7]. `0` is default. To activate a style 1..7, use '\#' in your string (where # = style index). To activate the default (0th) style, use '\r'.
        // When `configure_style` is called for the first time for an index (other than 0), it's copied from a `src`-th style.
        rvalue configure_style(int index, int src = 0)
        {
            Assert("2D renderer: Style index is out of range", index >= 0 && index < 8);
            Assert("2D renderer: Source style index is invalid", src >= 0 && src < 8 && src < int(text_styles.size()) && src != index && text_styles[src].was_initialized);
            text_cur_style = index;
            if (index >= int(text_styles.size()))
                text_styles.resize(index+1);
            if (!text_styles[index].was_initialized)
                text_styles[index] = text_styles[0];
            return (rvalue)*this;
        }
        rvalue style(const Style &style, int index = -1) // If index is equal to -1, the current style is changed.
        {
            if (index == -1)
                index = text_cur_style;
            Assert("2D renderer: Imported style index is invalid", index >= 0 && index < 8 && (index >= int(text_styles.size()) || text_styles[index].was_initialized == 0));
            if (index >= int(text_styles.size()))
                text_styles.resize(index+1);
            text_styles[index] = style;
            return (rvalue)*this;
        }
        rvalue styles(const StyleVector &style_vec) // This overwrites all styles.
        {
            text_styles = style_vec;
            return (rvalue)*this;
        }
        rvalue export_style(Style &style, int index = -1) // If index is equal to -1, the current style is exported. This discards current text drawing command. It's recommended to use it with stub (null) text and postiton.
        {
            Assert("2D renderer: Normal text object was used for style editing instead of stub one.", internal_stub_object);
            if (index == -1)
                index = text_cur_style;
            Assert("2D renderer: Exported text style index is out of range", index >= 0 && index < int(text_styles.size()));
            style = text_styles[index];
            return (rvalue)*this;
        }
        rvalue export_styles(StyleVector &style_vec)
        {
            Assert("2D renderer: Normal text object was used for style editing instead of stub one.", internal_stub_object);
            style_vec = text_styles;
        }
        Style &&export_style(int index = -1) // If index is equal to -1, the current style is exported. This discards current text drawing command. It's recommended to use it with stub (null) text and postiton.
        {
            Assert("2D renderer: Normal text object was used for style editing instead of stub one.", internal_stub_object);
            if (index == -1)
                index = text_cur_style;
            Assert("2D renderer: Exported text style index is out of range", index >= 0 && index < int(text_styles.size()));
            return (Style &&)text_styles[index];
        }
        StyleVector &&export_styles()
        {
            Assert("2D renderer: Normal text object was used for style editing instead of stub one.", internal_stub_object);
            return (StyleVector &&)text_styles;
        }


        // Style settings

        rvalue font(const Graphics::FontData &ref)
        {
            text_styles[text_cur_style].font = &ref;
            return (rvalue)*this;
        }
        rvalue color(fvec3 c)
        {
            text_styles[text_cur_style].color = c;
            return (rvalue)*this;
        }
        rvalue alpha(float a)
        {
            text_styles[text_cur_style].alpha = a;
            return (rvalue)*this;
        }
        rvalue opacity(float a)
        {
            text_styles[text_cur_style].opacity = a;
            return (rvalue)*this;
        }

        rvalue italic(float tan) // Uses `glyph_matrix()`. `tan` is the italic angle tangent. `0` is the normal text, `1` is 45 degree italic, `+inf` would be 90 degree italic. Negative values are also accepted.
        {
            glyph_matrix({1, -tan, 0, 1});
            return (rvalue)*this;
        }
        rvalue bold(float pixels) // Uses `spacing()`, `offset()` and `extend()`. Fractional values larger than 1 don't work well with (large) semitransparent fonts.
        {
            if (pixels > 0)
            {
                spacing(pixels);
                glyph_offset({-pixels/2, 0});
                for (int i = 1; i <= int(pixels); i++)
                    extend({float(i),0});
                if (pixels != int(pixels))
                    extend({pixels,0});
            }
            return (rvalue)*this;
        }
        rvalue bold_aligned(float pixels) // Same as normal `bold()`, but each glyph is aligned at the left edge instead of center. Useful for small width values used with pixel fonts.
        {
            if (pixels > 0)
            {
                spacing(pixels);
                for (int i = 1; i <= int(pixels); i++)
                    extend({float(i),0});
                if (pixels != int(pixels))
                    extend({pixels,0});
            }
            return (rvalue)*this;
        }
        rvalue spacing(float pixels) // Negative values are allowed, but don't work well. The function can be called multiple times, accumulating the value.
        {
            text_styles[text_cur_style].spacing.x += pixels;
            return (rvalue)*this;
        }
        rvalue vertical_spacing(float pixels) // Negative values are allowed, but don't work well. The function can be called multiple times, accumulating the value.
        {
            text_styles[text_cur_style].spacing.y += pixels;
            return (rvalue)*this;
        }

        rvalue glyph_matrix(fmat2 m) // This can be called multiple times. This matrix is applied after the normal one and doesn't affect relative glyph locations.
        {
            text_styles[text_cur_style].glyph_matrix = text_styles[text_cur_style].glyph_matrix /mul/ m;
            return (rvalue)*this;
        }
        rvalue glyph_offset(fvec2 o) // This can be called multiple times too. The offset is multiplied by the current glyph matrix.
        {
            text_styles[text_cur_style].glyph_offset += text_styles[text_cur_style].glyph_matrix /mul/ o;
            return (rvalue)*this;
        }

        // These can be called multiple times.
        // They create a copy of the same text with speicified offsets. Shadows are copied too.
        rvalue extend(fvec2 offset)
        {
            text_styles[text_cur_style].duplicate_offsets.push_back(offset);
            return (rvalue)*this;
        }
        rvalue extend(const std::vector<fvec2> &offsets)
        {
            for (const auto &it : offsets)
                extend(it);
            return (rvalue)*this;
        }

        // These affect all text copies and specify a reference frame in which offsets are specified.
        rvalue extend_ref_frame_glyph()
        {
            text_styles[text_cur_style].duplicate_ref_frame = RefFrame::glyph;
            return (rvalue)*this;
        }
        rvalue extend_ref_frame_text()
        {
            text_styles[text_cur_style].duplicate_ref_frame = RefFrame::text;
            return (rvalue)*this;
        }
        rvalue extend_ref_frame_global()
        {
            text_styles[text_cur_style].duplicate_ref_frame = RefFrame::global;
            return (rvalue)*this;
        }

        // These can be used multiple times.
        // Shadows are drawn front to back.
        rvalue shadow(fvec2 offset)
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::add, offset.to_vec3()});
            return (rvalue)*this;
        }
        rvalue shadow(const std::vector<fvec2> &offsets)
        {
            for (const auto &it : offsets)
                shadow(it);
            return (rvalue)*this;
        }

        // These can be used multiple times.
        // They affect all shadows created BEFORE they were called.
        rvalue shadow_color(fvec3 c)
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::set_color, c});
            return (rvalue)*this;
        }
        rvalue shadow_alpha(float a)
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::set_alpha, {a,0,0}});
            return (rvalue)*this;
        }
        rvalue shadow_opacity(float o)
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::set_opacity, {o,0,0}});
            return (rvalue)*this;
        }

        // These affect all shadows and specify a reference frame in which offsets are specified.
        rvalue shadow_ref_frame_glyph() // This is the default.
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::mode_glyph, {}});
            return (rvalue)*this;
        }
        rvalue shadow_ref_frame_text()
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::mode_text, {}});
            return (rvalue)*this;
        }
        rvalue shadow_ref_frame_global()
        {
            text_styles[text_cur_style].shadows.push_back({Shadow::Type::mode_global, {}});
            return (rvalue)*this;
        }

        ~Text()
        {
            if (renderer == 0 || internal_stub_object == 1)
                return;

            if (internal_temporary_object == 0)
            {
                for (auto &it : text_styles)
                {
                    it.glyph_matrix = text_matrix /mul/ it.glyph_matrix;
                    it.alpha *= text_alpha;
                }

                text_cur_style = 0;

                int text_final_settings;
                {
                    auto iter = std::find_if(text.rbegin(), text.rend(), internal_IsStyleMarker);
                    if (iter == text.rend())
                        text_final_settings = 0;
                    else
                        text_final_settings = internal_StyleMarkerToIndex(*iter);
                }

                if (text_alignment.y == 0)
                    position -= (internal_BaselineHeight(text_styles, text, text_cur_style)) / 2 * text_matrix.y;
                else if (text_alignment.y > 0)
                    position -= (internal_BaselineHeight(text_styles, text, text_cur_style) + text_styles[text_final_settings].font->Descent()) * text_matrix.y;
                else
                    position += text_styles[text_cur_style].font->Ascent() * text_matrix.y;

                internal_line_pos = position;

                if (text_alignment.x == 0)
                    position -= (internal_LineWidth(text_styles, text, text_cur_style) + text_styles[text_cur_style].spacing.x) / 2 * text_matrix.x;
                else if (text_alignment.x > 0)
                    position -= (internal_LineWidth(text_styles, text, text_cur_style) + text_styles[text_cur_style].spacing.x / 2) * text_matrix.x;
                else
                    position -= (text_styles[text_cur_style].spacing.x / 2) * text_matrix.x;
            }

            // Move shadow vectors to a separate variable. We don't need temporary shadow text objects to have them.
            std::vector<std::vector<Shadow>> shadows_copy(text_styles.size());
            for (std::size_t i = 0; i < text_styles.size(); i++)
                std::swap(text_styles[i].shadows, shadows_copy[i]);

            auto iter = text.begin();

            while (1)
            {
                Style &s = text_styles[text_cur_style];

                // Render shadows
                if (shadows_copy[text_cur_style].size())
                {
                    fvec3 tmp_shadow_color = {0,0,0};
                    float tmp_shadow_alpha = 1;
                    float tmp_shadow_opacity = 1;

                    RefFrame tmp_shadow_ref_frame = RefFrame::glyph;

                    DEBUG(bool unused_settings = 0;)

                    for (auto it = shadows_copy[text_cur_style].rbegin(); it != shadows_copy[text_cur_style].rend(); it++)
                    {
                        switch (it->type)
                        {
                          case Shadow::add:
                            {
                                auto tmp_text = *this;
                                tmp_text.internal_temporary_object = 1;
                                tmp_text.text.remove_prefix(iter - text.begin());
                                ivec2 offset;
                                switch (tmp_shadow_ref_frame)
                                {
                                  case RefFrame::global:
                                    offset = it->value.to_vec2();
                                    break;
                                  case RefFrame::text:
                                    offset = text_matrix /mul/ it->value.to_vec2();
                                    break;
                                  case RefFrame::glyph:
                                    offset = text_matrix /mul/ s.glyph_matrix /mul/ it->value.to_vec2();
                                    break;
                                }
                                tmp_text.position += offset;
                                tmp_text.internal_line_pos += offset;
                                tmp_text.text_styles[text_cur_style].color = tmp_shadow_color;
                                tmp_text.text_styles[text_cur_style].alpha = tmp_shadow_alpha * text_alpha;
                                tmp_text.text_styles[text_cur_style].opacity = tmp_shadow_opacity;
                                DEBUG(unused_settings = 0;)
                            }
                            break;
                          case Shadow::set_color:
                            tmp_shadow_color = it->value;
                            DEBUG(unused_settings = 1;)
                            break;
                          case Shadow::set_alpha:
                            tmp_shadow_alpha = it->value.x;
                            DEBUG(unused_settings = 1;)
                            break;
                          case Shadow::set_opacity:
                            tmp_shadow_opacity = it->value.x;
                            DEBUG(unused_settings = 1;)
                            break;
                          case Shadow::mode_global:
                          case Shadow::mode_text:
                          case Shadow::mode_glyph:
                            tmp_shadow_ref_frame = (RefFrame)it->type;
                            DEBUG(unused_settings = 1;)
                            break;
                        }
                    }
                    Assert("2D renderer: Attempt to specify text shadow parameters without an active shadow.", unused_settings == 0);
                }

                // Render copies
                if (s.duplicate_offsets.size() > 0)
                {
                    // We can't put it outside of the `while` because each shadow needs it's own set of duplicate offsets.

                    std::vector<std::vector<fvec2>> duplicate_offsets_copy(text_styles.size());
                    for (std::size_t i = 0; i < text_styles.size(); i++)
                        std::swap(text_styles[i].duplicate_offsets, duplicate_offsets_copy[i]);

                    for (const auto &it : duplicate_offsets_copy[text_cur_style])
                    {
                        auto tmp_text = *this;
                        tmp_text.internal_temporary_object = 1;
                        tmp_text.text.remove_prefix(iter - text.begin());

                        ivec2 offset;
                        switch (s.duplicate_ref_frame)
                        {
                          case RefFrame::global:
                            offset = it;
                            break;
                          case RefFrame::text:
                            offset = text_matrix /mul/ it;
                            break;
                          case RefFrame::glyph:
                            offset = text_matrix /mul/ s.glyph_matrix /mul/ it;
                            break;
                        }
                        tmp_text.position += offset;
                        tmp_text.internal_line_pos += offset;
                    }

                    for (std::size_t i = 0; i < text_styles.size(); i++)
                        std::swap(text_styles[i].duplicate_offsets, duplicate_offsets_copy[i]);
                }

                // Render the text itself
                uint16_t prev_ch = u8invalidchar;

                for (;; iter++)
                {
                    if (iter == text.end())
                        return;
                    if (!u8firstbyte(iter))
                        continue;
                    if (int(u8charlen(iter)) > text.end() - iter)
                        Sys::Error("Invalid UTF-8.");
                    uint16_t ch = u8decode(iter);
                    if (!s.font->GlyphExists(ch))
                        ch = 0;

                    if (ch == '\n')
                    {
                        internal_line_pos += s.font->LineSkip() * text_matrix.y;
                        position = internal_line_pos;
                        if (text_alignment.x == 0)
                            position -= (internal_LineWidth(text_styles, text.substr(iter - text.begin() + 1), text_cur_style) + s.spacing.x) / 2 * text_matrix.x;
                        else if (text_alignment.x > 0)
                            position -= (internal_LineWidth(text_styles, text.substr(iter - text.begin() + 1), text_cur_style) + s.spacing.x / 2) * text_matrix.x;
                        else
                            position -= (text_styles[text_cur_style].spacing.x / 2) * text_matrix.x;
                        prev_ch = u8invalidchar;
                        continue;
                    }
                    else if (internal_IsStyleMarker(ch)) // Style selectors
                    {
                        if (internal_temporary_object == 1)
                            return;
                        text_cur_style = internal_StyleMarkerToIndex(ch);
                        prev_ch = u8invalidchar;
                        break;
                    }
                    else
                    {
                        const auto &glyph_data = s.font->Glyph(ch);

                        int kerning;
                        if (prev_ch != u8invalidchar)
                            kerning = s.font->Kerning(prev_ch, ch);
                        else
                            kerning = 0;

                        float center_offset_x = glyph_data.offset.x + glyph_data.size.x / 2.f;

                        position += (kerning + center_offset_x + s.spacing.x / 2) * text_matrix.x;

                        bool stop = 0;

                        renderer->Sprite(position + s.glyph_offset.x * text_matrix.x + (s.glyph_offset.y + glyph_data.offset.y + glyph_data.size.y / 2.f) * text_matrix.y, glyph_data.size)
                            .center()
                            .tex(glyph_data.pos, glyph_data.size)
                            .color(s.color)
                            .mix(0)
                            .alpha(text_char_count < -0.5f ? s.alpha                         : // If no char count limit.
                                   text_char_count > 1     ? (text_char_count -= 1, s.alpha) : // If current char is not the last one.
                                   (stop = 1, s.alpha * text_char_count)) // If current char is the last one.
                            .opacity(s.opacity)
                            .matrix(s.glyph_matrix);

                        if (stop)
                            return;

                        position += (glyph_data.advance - center_offset_x + s.spacing.x / 2) * text_matrix.x;
                    }

                    prev_ch = ch;
                }

                iter++;
            }
        }
    };
    using Text_t = Text; // For cases when `Sprite` conflicts with the function name.

    Sprite Sprite(fvec2 pos, fvec2 size)
    {
        return {this, pos, size};
    }

    Text_t Text(fvec2 pos, std::string_view text)
    {
        return {this, pos, text};
    }
    Text_t Text() // Makes stub object for style editing purposes.
    {
        return {this, {0,0}, "", 1};
    }
};

#endif
