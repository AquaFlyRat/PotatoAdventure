#ifndef RENDERER2D_H_INCLUDED
#define RENDERER2D_H_INCLUDED

#include "graphics.h"
#include "math.h"

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
    bool use_mouse_mapping;
    Utils::Object<Graphics::RenderQueue<VertexFormat, 3>> render_queue;
    Utils::Object<Graphics::Shader> main_shader;
    int current_tex;
    ivec2 current_tex_size;

  public:
    static constexpr int default_render_queue_size = 0x10000;
    static const Graphics::ShaderSource &DefaultShaderSource();

    Renderer2D(ivec2 render_size, int render_queue_size = default_render_queue_size, const Graphics::ShaderSource &shader_src = DefaultShaderSource())
    {
        size = render_size;
        use_mouse_mapping = 0;
        current_tex = -1;
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

  private:
    class DrawCommand
    {
        DrawCommand(const DrawCommand &) = default;
        DrawCommand &operator=(const DrawCommand &) = default;

        Renderer2D *renderer;

        fvec2 dst_pos, dst_size;

        bool have_texture = 0;
        fvec2 texture_pos = {0,0}, texture_size = {1,1};

        bool have_center = 0;
        fvec2 sprite_center = {0,0};

        float sprite_angle = 0;

        bool have_color = 0;
        fvec3 sprite_colors[4] = {fvec3(0),fvec3(0),fvec3(0),fvec3(0)};

        bool have_tex_color_fac = 0;
        float tex_color_factors[4] = {1,1,1,1};

        float sprite_alpha[4] = {1,1,1,1};
        float sprite_opacity[4] = {1,1,1,1};

        bool absolute_pos = 0;
        bool absolute_tex_pos = 0;

        using rvalue = DrawCommand &&;

      public:
        DrawCommand(DrawCommand &&o) : DrawCommand(o)
        {
            o.renderer = 0;
        }
        DrawCommand &operator=(DrawCommand &&o)
        {
            *this = o;
            o.renderer = 0;
            return *this;
        }

        DrawCommand(Renderer2D *r, fvec2 pos, fvec2 sz) : renderer(r), dst_pos(pos), dst_size(sz) {}

        rvalue tex(fvec2 pos, fvec2 sz) &&
        {
            assert(!have_texture);
            have_texture = 1;

            texture_pos = pos;
            texture_size = sz;
            return (rvalue)*this;
        }
        rvalue tex(fvec2 pos) &&
        {
            assert(!have_texture);
            have_texture = 1;

            texture_pos = pos;
            texture_size = dst_size;
            return (rvalue)*this;
        }
        rvalue center(fvec2 c) &&
        {
            assert(!have_center);
            have_center = 1;

            sprite_center = c;
            return (rvalue)*this;
        }
        rvalue angle(float a) &&
        {
            sprite_angle = a;
            return (rvalue)*this;
        }
        rvalue color(fvec3 c) &&
        {
            assert(!have_color);
            have_color = 1;

            for (auto &it : sprite_colors)
                it = c;
            return (rvalue)*this;
        }
        rvalue color(fvec3 a, fvec3 b, fvec3 c, fvec3 d) &&
        {
            assert(!have_color);
            have_color = 1;

            sprite_colors[0] = a;
            sprite_colors[1] = b;
            sprite_colors[2] = c;
            sprite_colors[3] = d;
            return (rvalue)*this;
        }
        rvalue mix(float x) && // 0 - fill with color (default if color is provided), 1 - use texture
        {
            assert(!have_tex_color_fac);
            have_tex_color_fac = 1;

            for (auto &it : tex_color_factors)
                it = x;
            return (rvalue)*this;
        }
        rvalue mix(float a, float b, float c, float d) &&
        {
            assert(!have_tex_color_fac);
            have_tex_color_fac = 1;

            tex_color_factors[0] = a;
            tex_color_factors[1] = b;
            tex_color_factors[2] = c;
            tex_color_factors[3] = d;
            return (rvalue)*this;
        }
        rvalue alpha(float a) &&
        {
            for (auto &it : sprite_alpha)
                it = a;
            return (rvalue)*this;
        }
        rvalue alpha(float a, float b, float c, float d) &&
        {
            sprite_alpha[0] = a;
            sprite_alpha[1] = b;
            sprite_alpha[2] = c;
            sprite_alpha[3] = d;
            return (rvalue)*this;
        }
        rvalue opacity(float o) && // 1 - normal blending, 0 - additive blending
        {
            for (auto &it : sprite_opacity)
                it = o;
            return (rvalue)*this;
        }
        rvalue opacity(float a, float b, float c, float d) && // 1 - normal blending, 0 - additive blending
        {
            sprite_opacity[0] = a;
            sprite_opacity[1] = b;
            sprite_opacity[2] = c;
            sprite_opacity[3] = d;
            return (rvalue)*this;
        }
        rvalue absolute(bool x = 1) && // Interpret size as a position of the second corner
        {
            absolute_pos = x;
            return (rvalue)*this;
        }
        rvalue absolute_tex(bool x = 1) && // Interpret texture size as a position of the second corner
        {
            absolute_tex_pos = x;
            return (rvalue)*this;
        }

        ~DrawCommand()
        {
            if (!renderer)
                return;

            assert(have_texture || have_color);
            assert(absolute_pos + have_center < 2);

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

            fvec2 a = -sprite_center;
            fvec2 c = dst_size - sprite_center;

            fvec2 b = {a.x, c.y};
            fvec2 d = {c.x, a.y};

            if (sprite_angle != 0)
            {
                fmat2 m = fmat2::rotate2D(sprite_angle);
                a = m /mul/ a;
                b = m /mul/ b;
                c = m /mul/ c;
                d = m /mul/ d;
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

  public:
    DrawCommand Sprite(fvec2 pos, fvec2 size)
    {
        return {this, pos, size};
    }
};

/*
class Renderer2D
{
  public:
    //                                          pos    color  texpos factors (color: fixed..texture, alpha: fixed..texture, luminosity)
    using VertexFormat = Graphics::VertexFormat<fvec2, fvec4, fvec2, fvec3>;

  private:
    float scale = 1;
    ivec2 min_pos, max_pos;
    int font_index = 0;
    bool kerning = 1;
    std::vector<Graphics::Texture>  textures;
    std::vector<Graphics::Font>     fonts;
    std::vector<Graphics::FontData> fonts_data;
    Utils::Object<Graphics::Shader>  primary_shader;
    Utils::Object<Graphics::RenderQueue<VertexFormat,3>> render_queue;

  public:
    Renderer2D(const Renderer2D &) = delete;
    Renderer2D(Renderer2D &&) = delete;
    Renderer2D &operator=(const Renderer2D &) = delete;
    Renderer2D &operator=(Renderer2D &&) = delete;

    struct TextureInfo
    {
        Utils::BinaryInput file;
        bool linear;
    };

    struct FontInfo
    {
        Utils::BinaryInput file;
        Utils::ArrayView<uint16_t> glyphs;
        Graphics::Font::Quality quality;
        int texture;
        ivec2 dst, dstsz;
        int ptsize;
        int index = 0;
        u8vec4 color = {255,255,255,255};
    };

    struct BitmapFontInfo
    {
        ArrayView<uint16_t> glyphs;
        ivec2 src, glyph_sz;
        int row_len, asc = (glyph_sz.y+1)/2, adv = glyph_sz.x, lskip = glyph_sz.y;
    };

    Renderer2D(Utils::ArrayView<TextureInfo> textures_info, Utils::ArrayView<FontInfo> fonts_info, Utils::ArrayView<BitmapFontInfo> bitmap_fonts_info) : textures(textures_info.size())
    {
        fonts.resize(fonts_info.size());
        for (std::size_t i = 0; i < fonts_info.size(); i++)
        {
            fonts[i].Open((Utils::BinaryInput &&) fonts_info[i].file, fonts_info[i].ptsize, fonts_info[i].index);
        }

        fonts_data.resize(fonts_info.size() + bitmap_fonts_info.size());
        for (std::size_t i = 0; i < textures_info.size(); i++)
        {
            Graphics::ImageData img = Graphics::ImageData::FromPNG((Utils::BinaryInput &&) textures_info[i].file);
            for (std::size_t j = 0; j < fonts_info.size(); j++)
            {
                if (fonts_info[j].texture != int(i))
                    continue;
                fonts[j].RenderGlyphs(fonts_data[j], img, fonts_info[j].dst, fonts_info[j].dstsz, fonts_info[j].glyphs, 1, fonts_info[j].quality, fonts_info[j].color);
            }
            textures[i].SetData(img);
            textures[i].LinearInterpolation(textures_info[i].linear);
        }

        for (std::size_t i = 0; i < bitmap_fonts_info.size(); i++)
        {
            fonts_data[fonts_info.size() + i] = Graphics::FontData(bitmap_fonts_info[i].glyphs,
                                                                   bitmap_fonts_info[i].src,
                                                                   bitmap_fonts_info[i].glyph_sz,
                                                                   bitmap_fonts_info[i].row_len,
                                                                   bitmap_fonts_info[i].asc,
                                                                   bitmap_fonts_info[i].adv,
                                                                   bitmap_fonts_info[i].lskip);
        }

        primary_shader.create("Renderer2D primary shader", Graphics::ShaderSource
{{"a_pos","a_color","a_texpos","a_factors"},
 {"u_matrix","u_texture","u_texsize"},
ForWindows("#version 330 compatibility")
ForMac("#version 150")
ForMobile("#version 100") R"(
uniform mat4 u_matrix;
uniform vec2 u_texsize;
attribute vec2 a_pos;
attribute vec4 a_color;
attribute vec2 a_texpos;
attribute vec3 a_factors;
varying vec4 v_color;
varying vec2 v_texpos;
varying vec3 v_factors;
void main()
{
    gl_Position = u_matrix * vec4(a_pos,0,1);
    v_color = a_color;
    v_texpos = a_texpos / u_texsize;
    v_factors = a_factors;

})",
ForWindows("#version 330 compatibility")
ForMac("#version 150")
ForMobile("#version 100\nprecision mediump float;") R"(
uniform sampler2D u_texture;
varying vec4 v_color;
varying vec2 v_texpos;
varying vec3 v_factors;
void main()
{
    vec4 tex_color = texture2D(u_texture, v_texpos);
    gl_FragColor = vec4(mix(v_color.rgb, tex_color.rgb, v_factors.x), mix(v_color.a, tex_color.a, v_factors.y));
    gl_FragColor.rgb *= gl_FragColor.a;
    gl_FragColor.a *= (1 - v_factors.z);
})"});
        UpdateViewport();
        if (textures.size())
            UseTexture(0);

        render_queue.create(0x10000);
    }

    ~Renderer2D() {}

    auto &RenderQueue()
    {
        return *render_queue;
    }
    const auto &RenderQueue() const
    {
        return *render_queue;
    }

    static void InitializeState()
    {
        Graphics::Blend::Enable();
        Graphics::Blend::Presets::FuncNormal_Pre();
    }

    void SetScale(float s)
    {
        scale = s;
    }

    void UpdateViewport(ivec2 size = Window::Size())
    {
        min_pos = -(size + 1) / 2;
        max_pos = size / 2;
        primary_shader->SetUniform<fmat4>(0, fmat4::ortho({min_pos.x / scale, max_pos.y / scale}, {max_pos.x / scale, min_pos.y / scale}, -1, 1));
        min_pos.x = std::lround(std::ceil(min_pos.x / scale));
        min_pos.y = std::lround(std::ceil(min_pos.y / scale));
        max_pos.x = std::lround(std::ceil(max_pos.x / scale));
        max_pos.y = std::lround(std::ceil(max_pos.y / scale));
    }

    void SetMouseMapping() const
    {
        Input::SetMouseMapping(-(Window::Size() + 1) / 2, 1/scale);
    }

    ivec2 MinPos() const
    {
        return min_pos;
    }

    ivec2 MaxPos() const
    {
        return max_pos;
    }

    void UseTexture(int number)
    {
        primary_shader->SetUniform<Graphics::Texture>(1, textures[number]);
        primary_shader->SetUniform<fvec2>(2, textures[number].Size());
    }

    void Flush()
    {
        render_queue->Flush();
    }

    void BindShader()
    {
        primary_shader->Use();
    }

    struct Source
    {
        ivec2 tex_a, tex_b;
        fvec4 color00, color01, color10, color11;
        fvec3 factors00, factors01, factors10, factors11;

        void set_all_colors(fvec4 c)
        {
            color00 = color01 = color10 = color11 = c;
        }
        void set_all_factors(fvec3 f)
        {
            factors00 = factors01 = factors10 = factors11 = f;
        }

        Source()
        {
            tex_a = tex_b = {0,0};
            set_all_colors({0,0,0,1});
            set_all_factors({0,0,0});
        }
        Source(ivec2 src, ivec2 srcsz, float alpha = 1, float luminance = 0)
        {
            tex_a = src;
            tex_b = src+srcsz;
            set_all_colors({0,0,0,0});
            set_all_factors({1, alpha, luminance});
        }
        Source(fvec4 fill_color, float luminance = 1)
        {
            tex_a = tex_b = {0,0};
            set_all_colors(fill_color);
            set_all_factors({0,0,luminance});
        }
        Source(fvec4 c00, fvec4 c10, fvec4 c01, fvec4 c11, float luminance = 0)
        {
            tex_a = tex_b = {0,0};
            color00 = c00;
            color01 = c01;
            color10 = c10;
            color11 = c11;
            factors00 = factors01 = factors10 = factors11 = {0,0,luminance};
        }
        Source(fvec3 mix_color, ivec2 src, ivec2 srcsz, float mix, float alpha = 1, float luminance = 0)
        {
            tex_a = src;
            tex_b = src+srcsz;
            set_all_colors(mix_color.to_vec4(0));
            set_all_factors({mix, alpha, luminance});
        }
    };

    void Rectf(fvec2 dst, fvec2 dstsz, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        center *= dstsz / fvec2(src.tex_b - src.tex_a);
        if (angle == 0)
        {
            dst -= center;
            dstsz += dst;
            render_queue->Insert({fvec2(dst.x  , dst.y  ), src.color00, fvec2(src.tex_a.x, src.tex_a.y), src.factors00},
                                 {fvec2(dstsz.x, dst.y  ), src.color10, fvec2(src.tex_b.x, src.tex_a.y), src.factors10},
                                 {fvec2(dstsz.x, dstsz.y), src.color11, fvec2(src.tex_b.x, src.tex_b.y), src.factors11},
                                 {fvec2(dst.x  , dstsz.y), src.color01, fvec2(src.tex_a.x, src.tex_b.y), src.factors01});
        }
        else
        {
            dstsz -= center;
            render_queue->Insert({dst + fmat2::rotate2D(angle) /mul/ fvec2(-center.x, -center.y), src.color00, fvec2(src.tex_a.x, src.tex_a.y), src.factors00},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(dstsz.x  , -center.y), src.color10, fvec2(src.tex_b.x, src.tex_a.y), src.factors10},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(dstsz.x  , dstsz.y  ), src.color11, fvec2(src.tex_b.x, src.tex_b.y), src.factors11},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(-center.x, dstsz.y  ), src.color01, fvec2(src.tex_a.x, src.tex_b.y), src.factors01});
        }
    }
    void Rectf(fvec2 dst, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf(dst, src.tex_b - src.tex_a, src, center, angle);
    }

    void Rect(ivec2 dst, ivec2 dstsz, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf(dst, dstsz, src, center, angle);
    }
    void Rect(ivec2 dst, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf(dst, src, center, angle);
    }


    void Rectf_x(fvec2 dst, fvec2 dstsz, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        center *= dstsz / fvec2(src.tex_b - src.tex_a);
        if (angle == 0)
        {
            dst -= center;
            dstsz += dst;
            render_queue->Insert({fvec2(dst.x  , dst.y  ), src.color00, fvec2(src.tex_b.x, src.tex_a.y), src.factors00},
                                 {fvec2(dstsz.x, dst.y  ), src.color10, fvec2(src.tex_a.x, src.tex_a.y), src.factors10},
                                 {fvec2(dstsz.x, dstsz.y), src.color11, fvec2(src.tex_a.x, src.tex_b.y), src.factors11},
                                 {fvec2(dst.x  , dstsz.y), src.color01, fvec2(src.tex_b.x, src.tex_b.y), src.factors01});
        }
        else
        {
            dstsz -= center;
            render_queue->Insert({dst + fmat2::rotate2D(angle) /mul/ fvec2(-center.x, -center.y), src.color00, fvec2(src.tex_b.x, src.tex_a.y), src.factors00},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(dstsz.x  , -center.y), src.color10, fvec2(src.tex_a.x, src.tex_a.y), src.factors10},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(dstsz.x  , dstsz.y  ), src.color11, fvec2(src.tex_a.x, src.tex_b.y), src.factors11},
                                 {dst + fmat2::rotate2D(angle) /mul/ fvec2(-center.x, dstsz.y  ), src.color01, fvec2(src.tex_b.x, src.tex_b.y), src.factors01});
        }
    }
    void Rectf_x(fvec2 dst, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf_x(dst, src.tex_b - src.tex_a, src, center, angle);
    }

    void Rect_x(ivec2 dst, ivec2 dstsz, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf_x(dst, dstsz, src, center, angle);
    }
    void Rect_x(ivec2 dst, Source src, fvec2 center = {0,0}, float angle = 0)
    {
        Rectf_x(dst, src, center, angle);
    }


    void Kerning(bool enable)
    {
        kerning = enable;
    }

    void Textf(fvec2 dst, int font_index, float scale, ivec2 alignment, const char *str, fvec4 color, float angle = 0, float luminance = 1)
    {
        if (!(scale >= -1 && scale <= 1))
            Sys::Error("Text alignment is out of range.");

        const auto &data = fonts_data[font_index];

        fmat2 rot;
        if (angle != 0)
            rot = fmat2::rotate2D(angle);
        else
            rot = fmat2::identity();


        auto LocalLen = [&](const char *ptr) -> int
        {
            int ret = 0;
            uint16_t prev = 0;
            while (1)
            {
                uint16_t ch = u8decode(ptr, &ptr);
                if (ch == '\0' || ch == '\n')
                    break;
                if (data.HasGlyph(ch))
                    ret += data.Advance(ch) + data.Kerning(prev, ch) * kerning;
                prev = ch;
            };
            return ret;
        };
        fvec2 pos = dst;
        float yoffset = 0;
        if (alignment.x != -1)
            pos -= rot /mul/ fvec2(LocalLen(str) * (alignment.x+1)/2, 0);
        if (alignment.y == -1)
            yoffset += data.Ascent();
        else
        {
            int newlines = 0;
            const char *ptr = str;
            while (*ptr != '\0')
            {
                if (*ptr == '\n')
                    newlines++;
                ptr++;
            }
            yoffset -= newlines * data.Height() * (alignment.y+1)/2;
            if (alignment.y == 1)
                yoffset -= data.Descent();
        }
        pos += rot /mul/ fvec2(0, yoffset);
        uint16_t prev = 0;
        int line = 0;
        while (1)
        {
            uint16_t ch = u8decode(str, &str);
            if (ch == '\0')
                break;
            if (ch == '\n')
            {
                line++;
                pos = dst + rot /mul/ fvec2(0, data.Height() * line + yoffset);
                if (alignment.x != -1)
                    pos -= rot /mul/ fvec2(LocalLen(str) * (alignment.x+1)/2, 0);
            }
            else if (data.HasGlyph(ch))
            {
                ivec2 glyph_sz = data.Size(ch), glyph_offset = data.Offset(ch);
                Rectf(pos, glyph_sz * scale, {color.to_vec3(), data.Pos(ch), glyph_sz, 1, color.a, luminance}, {float(-glyph_offset.x),float(-glyph_offset.y)}, angle);
                pos += rot /mul/ fvec2(data.Advance(ch) + data.Kerning(prev, ch) * kerning, 0);
            }
            prev = ch;
        }
    }
    void Text(ivec2 dst, int font_index, float scale, ivec2 alignment, const char *str, fvec4 color, float angle = 0, float luminance = 1)
    {
        Textf(dst, font_index, scale, alignment, str, color, angle, luminance);
    }
};*/

#endif
