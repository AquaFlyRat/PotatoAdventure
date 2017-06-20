#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <utility>

#include "exceptions.h"
#include "lib/gl.h"
#include "lib/sdl.h"
#include "lib/sdlttf.h"
#include "math.h"
#include "os.h"
#include "system.h"
#include "utils.h"
#include "window.h"

/* GLSL version chart:
    1.10.59		2.0		April 2004		#version 110
    1.20.8		2.1		September 2006	#version 120
    1.30.10		3.0		August 2008		#version 130
    1.40.08		3.1		March 2009		#version 140
    1.50.11		3.2		August 2009		#version 150
    3.30.6		3.3		February 2010	#version 330
    4.00.9		4.0		March 2010		#version 400
    4.10.6		4.1		July 2010		#version 410
    4.20.11		4.2		August 2011		#version 420
    4.30.8		4.3		August 2012		#version 430
    4.40		4.4		July 2013		#version 440
    4.50		4.5		August 2014		#version 450
    1.00        ES 2                    #version 100

    GLSL ES can be tested with `#ifdef GL_ES`.
    GLSL ES lacks default precision for float inside of fragment shaders.
*/

namespace Graphics
{
    #ifdef E0INTERNAL_GRAPHICS_H_SPECIAL_ACCESS
    void Initialize();
    void Cleanup();
    void BeginFrame();
    void EndFrame();
    #endif

    namespace Init
    {
        void MaxTextureCount(int);
    }

    namespace Blend
    {
        enum Factors
        {
            zero                 = GL_ZERO,
            one                  = GL_ONE,
            src                  = GL_SRC_COLOR,
            one_minus_src        = GL_ONE_MINUS_SRC_COLOR,
            dst                  = GL_DST_COLOR,
            one_minus_dst        = GL_ONE_MINUS_DST_COLOR,
            src_a                = GL_SRC_ALPHA,
            one_minus_src_a      = GL_ONE_MINUS_SRC_ALPHA,
            dst_a                = GL_DST_ALPHA,
            one_minus_dst_a      = GL_ONE_MINUS_DST_ALPHA,
            constant             = GL_CONSTANT_COLOR,
            one_minus_constant   = GL_ONE_MINUS_CONSTANT_COLOR,
            constant_a           = GL_CONSTANT_ALPHA,
            one_minus_constant_a = GL_ONE_MINUS_CONSTANT_ALPHA,
            src_a_saturate       = GL_SRC_ALPHA_SATURATE,
            ForPC
            (
                src1                 = GL_SRC1_COLOR,
                one_minus_src1       = GL_ONE_MINUS_SRC1_COLOR,
                src1_a               = GL_SRC1_ALPHA,
                one_minus_src1_a     = GL_ONE_MINUS_SRC1_ALPHA,
            )
        };
        enum Equations
        {
            eq_add              = GL_FUNC_ADD,
            eq_subtract         = GL_FUNC_SUBTRACT,
            eq_reverse_subtract = GL_FUNC_REVERSE_SUBTRACT,
            ForPC
            (
                eq_min              = GL_MIN,
                eq_max              = GL_MAX,
            )
        };

        // Func(a,b) and Equation(a) set same parameters for both color and alpha blending.
        // Func(a,b,c,d) and Equation(a,b) set same parameters for color and alpha blending separately.
        inline void Enable() {glEnable(GL_BLEND);}
        inline void Disable() {glDisable(GL_BLEND);}
        inline void Func(Factors src, Factors dst)                             {glBlendFunc(src, dst);}
        inline void Func(Factors src, Factors dst, Factors srca, Factors dsta) {glBlendFuncSeparate(src, dst, srca, dsta);}
        inline void Equation(Equations eq) {glBlendEquation(eq);}
        inline void Equation(Equations eq, Equations eqa) {glBlendEquationSeparate(eq, eqa);}

        namespace Presets
        {
            inline void FuncOverwrite      () {Func(one, zero);}
            inline void FuncSimple         () {Func(src_a, one_minus_src_a);} // This one is crappy.
            inline void FuncNormal_RawToPre() {Func(src_a, one_minus_src_a, one, one_minus_src_a);} // Src textures are normal, output is premultiplied.
            inline void FuncNormal_Pre     () {Func(one, one_minus_src_a);} // Src and output are premultiplited
        }
    }

    // Here and below 01 means that colors are in [0;1] range. Otherwise it's [0;255].

    void Depth(bool on);
    inline void Culling(bool on) {(on ? glEnable : glDisable)(GL_CULL_FACE);}
    inline void SetClearColor01(fvec4 c) {glClearColor(c.r, c.g, c.b, c.a);}
    inline void SetClearColor(u8vec4 c) {SetClearColor01(c / 255.0f);}

    // You usually DON'T need to call these manually if you don't use framebuffers.
    // Color is cleared automatically each frame. Depth is cleared each frame if depth testing is enabled.
    void Clear(); // This clears color and if depth test is enabled, it also clears depth.
    inline void ClearColor() {glClear(GL_COLOR_BUFFER_BIT);}
    inline void ClearDepth() {glClear(GL_DEPTH_BUFFER_BIT);}
    inline void ClearColorAndDepth() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}

    // These functions set a size and a position or a rectange to draw into. You need to call one of them before rendering to thing with a different size or when the window is resized.
    // __2 ones can increase viewport size by one extra pixel in each dimension to ensure that size % 2 == 0. They are necessary for good 2D pixel graphics.
    // __2y are same, but if they increase Y size, they also decrease viewport Y position by 1. These are good for the window.
    inline void Viewport(ivec2 pos, ivec2 size) {glViewport(pos.x, pos.y, size.x, size.y);}
    inline void Viewport(ivec2 size)            {Viewport({0, 0}, {size.x, size.y});}
    inline void Viewport2(ivec2 size)           {Viewport({0, 0}, size+size%2);}
    inline void Viewport2y(ivec2 size)          {Viewport({0, -size.x%2}, size+size%2);}
    template <typename T>
    void ViewportObj(const T &obj)     {Viewport((ivec2)obj.Size());}
    template <typename T>
    void ViewportObj2(const T &obj)    {Viewport2((ivec2)obj.Size());}
    template <typename T>
    void ViewportObj2y(const T &obj)   {Viewport2y((ivec2)obj.Size());}
    inline void ViewportFullscreen()   {Viewport(Window::Size());}
    inline void ViewportFullscreen2()  {Viewport2(Window::Size());}
    inline void ViewportFullscreen2y() {Viewport2y(Window::Size());}

    void ForceErrorCheck();

    inline bool ExtensionSupported(std::string name)
    {
        return (bool)SDL_GL_ExtensionSupported(name.c_str());
    }

    inline namespace Templates
    {
        template <typename T> struct type2glconst;
        template <> struct type2glconst<unsigned char>  {static constexpr GLenum value = GL_UNSIGNED_BYTE;};
        template <> struct type2glconst<signed char>    {static constexpr GLenum value = GL_BYTE;};
        template <> struct type2glconst<char>           {static constexpr GLenum value = Utils::SysInfo::char_is_signed ? GL_BYTE : GL_UNSIGNED_BYTE;};
        template <> struct type2glconst<unsigned short> {static constexpr GLenum value = GL_UNSIGNED_SHORT;};
        template <> struct type2glconst<signed short>   {static constexpr GLenum value = GL_SHORT;};
        template <> struct type2glconst<unsigned int>   {static constexpr GLenum value = GL_UNSIGNED_INT;};
        template <> struct type2glconst<signed int>     {static constexpr GLenum value = GL_INT;};
        template <> struct type2glconst<float>          {static constexpr GLenum value = GL_FLOAT;};
        #ifdef GL_DOUBLE
        template <> struct type2glconst<double>         {static constexpr GLenum value = GL_DOUBLE;};
        #endif

        template <typename T>
        inline void SetAttribPointer(GLuint index, GLint size, GLsizei stride, const GLvoid *pointer)
        {
            static_assert(type2glconst<T>::value, "Invalid type!");
            #ifdef glVertexAttribIPointer
            glVertexAttribIPointer(index, size, type2glconst<T>::value, stride, pointer);
            #endif
        }
        template <>
        inline void SetAttribPointer<float>(GLuint index, GLint size, GLsizei stride, const GLvoid *pointer)
        {
            glVertexAttribPointer(index, size, type2glconst<float>::value, GL_FALSE, stride, pointer);
        }
        template <>
        inline void SetAttribPointer<double>(GLuint index, GLint size, GLsizei stride, const GLvoid *pointer)
        {
            #ifdef glVertexAttribLPointer
            glVertexAttribLPointer(index, size, type2glconst<double>::value, stride, pointer);
            #endif
        }
    }

    inline namespace VertexFormatDefinition
    {
        template <typename F, typename ...P> struct VertexFormat;

        namespace InternalPackTemplates
        {
            template <unsigned int N, typename F, typename ...P> struct At
            {
                static auto &func(VertexFormat<F, P...> *th) {return At<N-1, P...>::func(&th->next);}
            };
            template <typename F, typename ...P> struct At<0, F, P...>
            {
                static auto &func(VertexFormat<F, P...> *th) {return th->first;}
            };
        }

        template <typename F, typename ...P> struct VertexFormat
        {
            static constexpr unsigned int size = sizeof...(P)+1;
            F first;
            VertexFormat<P...> next;
            VertexFormat() {}
            VertexFormat(const F &f, const P &... p) : first(f), next(p...) {}
            template <unsigned int N> using TypeAt = typename Utils::TypeAt<N, F, P...>::type;
            template <unsigned int N> auto &At() {return InternalPackTemplates::At<N, F, P...>::func(this);}
        };
        template <typename F> struct VertexFormat<F>
        {
            static constexpr unsigned int size = 1;
            F first;
            VertexFormat() {}
            VertexFormat(const F &f) : first(f) {}
            template <unsigned int N> using TypeAt = typename Utils::TypeAt<N, F>::type;
            template <unsigned int N> auto &At() {return InternalPackTemplates::At<N, F>::func(this);}
        };
    }


    namespace Attribs
    {
        uint64_t Status();
        void SetStatus(uint64_t new_status);
        void Enable(unsigned int num);
        void Disable(unsigned int num);

        inline namespace PackSetup
        {
            namespace Internal
            {
                template <typename T> struct DimOf {static constexpr unsigned int value = 1;};
                template <unsigned int D, typename T> struct DimOf<vec<D, T>> {static constexpr unsigned int value = D;};
                template <typename T> struct BaseTypeOf {using type = T;};
                template <unsigned int D, typename T> struct BaseTypeOf<vec<D, T>> {using type = T;};


                template <unsigned int SIZE, unsigned int POS, unsigned int BYTEPOS, typename T> struct SetAttribs
                {
                    static void func() {}
                };
                template <unsigned int SIZE, unsigned int POS, unsigned int BYTEPOS, typename F, typename ...P> struct SetAttribs<SIZE, POS, BYTEPOS, VertexFormat<F, P...>>
                {
                    static void func()
                    {
                        SetAttribPointer<typename BaseTypeOf<typename VertexFormat<F, P...>::template TypeAt<0>>::type>(POS, DimOf<F>::value, SIZE, (void *)BYTEPOS);
                        using pack = VertexFormat<F, P...>;
                        SetAttribs<SIZE, POS+1, BYTEPOS + offsetof(pack, next), VertexFormat<P...>>::func();
                    }
                };
                template <unsigned int SIZE, unsigned int POS, unsigned int BYTEPOS, typename F> struct SetAttribs<SIZE, POS, BYTEPOS, VertexFormat<F>>
                {
                    static void func()
                    {
                        SetAttribPointer<typename BaseTypeOf<typename VertexFormat<F>::template TypeAt<0>>::type>(POS, DimOf<F>::value, SIZE, (void *)BYTEPOS);
                    }
                };
            }

            template <typename T> void SetForType()
            {
                Attribs::SetStatus((1ull << T::size) - 1);
                Internal::SetAttribs<sizeof (T), 0, 0, T>::func();
            }
        }
    }


    enum class StorageType : GLenum
    {
        draw_static  = GL_STATIC_DRAW,
        draw_dynamic = GL_DYNAMIC_DRAW,
        draw_stream  = GL_STREAM_DRAW,
        #ifdef GL_STATIC_COPY
        copy_static  = GL_STATIC_COPY,
        copy_dynamic = GL_DYNAMIC_COPY,
        copy_stream  = GL_STREAM_COPY,
        read_static  = GL_STATIC_READ,
        read_dynamic = GL_DYNAMIC_READ,
        read_stream  = GL_STREAM_READ,
        #endif
    };

    class InternalState
    {
        template <typename T> friend class VertexArray;
        friend class Shader;
        static GLuint vbo_binding;
        static bool vbo_attribs_configured;
    };

    enum RenderMode : GLenum
    {
        points     = GL_POINTS,
        lines      = GL_LINES,
        line_strip = GL_LINE_STRIP,
        line_loop  = GL_LINE_LOOP,
        triangles  = GL_TRIANGLES,
        triangle_strip = GL_TRIANGLE_STRIP,
        triangle_fan   = GL_TRIANGLE_FAN,
    };

    template <typename T>
    class VertexArray
    {
        static_assert(sizeof (T), "Void is not allowed as a parameter.");
        GLuint vbo;
      public:
        using Element = T;

        VertexArray()
        {
            static unsigned int id = 0;
            glGenBuffers(1, &vbo);
            if (!vbo)
                Sys::Error(Str("glGenBuffers() failed for VertexArray #", id, '!'));
            id++;
        }
        VertexArray(ArrayView<T> src, StorageType acc = StorageType::draw_static) : VertexArray() // (`data` may be null) Binds VBO after construction.
        {
            NewData(src, acc);
        }
        VertexArray(ArrayView<uint8_t> src, StorageType acc = StorageType::draw_static) : VertexArray() // (`data` may be null) Binds VBO after construction.
        {
            NewDataBytes(src, acc);
        }

        VertexArray(const VertexArray &) = delete;
        VertexArray(VertexArray &&) = delete;
        VertexArray &operator=(const VertexArray &) = delete;
        VertexArray &operator=(VertexArray &&) = delete;

        ~VertexArray()
        {
            if (InternalState::vbo_binding == vbo)
                InternalState::vbo_binding = 0;
            glDeleteBuffers(1, &vbo);
        }

        GLuint Handle() const
        {
            return vbo;
        }

        static void BindStorageHandle(GLuint id) // Binds a VBO.
        {
            if (id == InternalState::vbo_binding)
                return;
            InternalState::vbo_binding = id;
            glBindBuffer(GL_ARRAY_BUFFER, id);
            InternalState::vbo_attribs_configured = 0;
        }

        template <typename TT> static void BindHandle(GLuint id) // Binds a VBO and configures attrib pointers based on template parameter.
        {
            BindStorageHandle(id);
            if (!InternalState::vbo_attribs_configured)
            {
                InternalState::vbo_attribs_configured = 1;
                Attribs::SetForType<TT>();
            }
        }

        void BindStorage() const // Binds VBO.
        {
            BindStorageHandle(vbo);
        }
        static void BindStorageNothing() // Unbinds current VBO.
        {
            BindStorageHandle(0);
        }
        void Bind() const // Binds VBO and configures pointers.
        {
            BindHandle<T>(vbo);
        }
        static void BindNothing() // Unbinds current VBO and clears all attrib pointers.
        {
            BindHandle<void>(0);
        }

        void NewData(ArrayView<T> src, StorageType acc = StorageType::draw_static) // (`src` may be null) Auto binds VBO
        {
            BindStorage();
            glBufferData(GL_ARRAY_BUFFER, src.size() * sizeof (T), src.data(), (GLenum)acc);
        }
        WarningForMobile("This does not work on mobile platforms.")
        void Get(ArrayProxy<T> dst, unsigned int src_pos) const // Auto binds VBO
        {
            BindStorage();
            ForPC
            (
            glGetBufferSubData(GL_ARRAY_BUFFER, src_pos * sizeof (T), dst.size() * sizeof (T), dst.data());
            )
        }
        void Set(ArrayView<T> src, unsigned int dst_pos) // Auto binds VBO
        {
            BindStorage();
            glBufferSubData(GL_ARRAY_BUFFER, dst_pos * sizeof (T), src.size() * sizeof (T), src.data());
        }

        void NewDataBytes(ArrayView<uint8_t> src, StorageType acc = StorageType::draw_static) // (`src` may be null) Auto binds VBO
        {
            BindStorage();
            glBufferData(GL_ARRAY_BUFFER, src.size(), src.data(), (GLenum)acc);
        }
        WarningForMobile("This does not work on mobile platforms.")
        void GetBytes(ArrayProxy<uint8_t> dst, unsigned int src_pos) const // Auto binds VBO
        {
            BindStorage();
            ForPC
            (
            glGetBufferSubData(GL_ARRAY_BUFFER, src_pos, dst.size(), dst.data());
            )
        }
        void SetBytes(ArrayView<uint8_t> src, unsigned int dst_pos) // Auto binds VBO
        {
            BindStorage();
            glBufferSubData(GL_ARRAY_BUFFER, dst_pos, src.size(), src.data());
        }

        void Draw(unsigned int pos, unsigned int count, RenderMode m = RenderMode::triangles) const {Bind(); glDrawArrays((GLenum)m, pos, count);} // Auto binds VBO
        void Draw(                  unsigned int count, RenderMode m = RenderMode::triangles) const {Bind(); glDrawArrays((GLenum)m,   0, count);} // Auto binds VBO
    };

    template <typename T>
    class SizedVertexArray : public VertexArray<T>
    {
        unsigned int size;

        SizedVertexArray(const SizedVertexArray &) = delete;
        SizedVertexArray(SizedVertexArray &&) = delete;
        SizedVertexArray &operator=(const SizedVertexArray &) = delete;
        SizedVertexArray &operator=(SizedVertexArray &&) = delete;

      public:
        SizedVertexArray()
        {
            size = 0;
        }
        SizedVertexArray(ArrayView<T> src, StorageType acc = StorageType::draw_static) : VertexArray<T>(src, acc) // (`data` may be null) Binds VBO after construction.
        {
            size = src.size();
        }

        void NewData(ArrayView<T> src, StorageType acc = StorageType::draw_static)
        {
            size = src.size();
            VertexArray<T>::NewData(src, acc);
        }
        void NewDataBytes(ArrayView<uint8_t> src, StorageType acc = StorageType::draw_static) = delete;

        using VertexArray<T>::Draw; // Auto binds VBO
        void Draw(RenderMode m = RenderMode::triangles) const {Draw(size, m);} // Auto binds VBO

        unsigned int Vertices() const {return size;}
    };


    template <typename L>
    class RenderArray
    {
      protected:
        VertexArray<L> vao;
        uint32_t size;
        uint32_t pos;
        std::vector<L> arr;

      public:
        RenderArray(uint32_t l, StorageType acc = StorageType::draw_dynamic) : vao({(const uint8_t *)0, sizeof (L) * l}, acc)
        {
            if (l == 0)
                Sys::Error("Invalid rendering array size.");
            size = l;
            pos = 0;
            arr.resize(l);
        }
        RenderArray(const RenderArray &) = delete;
        RenderArray(RenderArray &&) = delete;
        RenderArray &operator=(const RenderArray &) = delete;
        RenderArray &operator=(RenderArray &&) = delete;

        ~RenderArray() {}

        uint32_t MaxSize() const
        {
            return size;
        }
        uint32_t CurrentSize() const
        {
            return pos;
        }
        bool Empty() const
        {
            return !CurrentSize();
        }

        void ChangeSize(uint32_t l, StorageType acc = StorageType::draw_dynamic)
        {
            if (l == 0)
                Sys::Error("Invalid rendering array size.");
            size = l;
            pos = 0;
            arr.resize(l);
            vao.NewData({(const uint8_t *)0, sizeof (L) * l}, acc);
        }

        L *Add(uint32_t amount)
        {
            if (pos + amount > size)
                Exceptions::Graphics::RenderArrayOverflow(Str(size));
            L *ret = arr + pos;
            pos += amount;
            return ret;
        }

        void Insert1(const L &a)
        {
            L *dst = Add(1);
            dst[0] = a;
        }
        void Insert2(const L &a, const L &b)
        {
            L *dst = Add(2);
            dst[0] = a;
            dst[1] = b;
        }
        void Insert3(const L &a, const L &b, const L &c)
        {
            L *dst = Add(3);
            dst[0] = a;
            dst[1] = b;
            dst[2] = c;
        }
        void Insert4as3x2(const L &a, const L &b, const L &c, const L &d) // a b d  b c d
        {
            L *dst = Add(6);
            dst[0] = a;
            dst[1] = b;
            dst[2] = d;
            dst[3] = b;
            dst[4] = c;
            dst[5] = d;
        }

        operator const L *() const
        {
            return arr;
        }

        void Send() // You shall not call Push*() between Send() and Draw*()
        {
            vao.Set({arr.data(), pos}, 0);
        }
        void Clear()
        {
            pos = 0;
        }

        void Draw(RenderMode m = RenderMode::triangles) const
        {
            vao.Draw(pos, m);
        }
    };

    template <typename L, int Dim>
    class RenderQueue : RenderArray<L>
    {
        static_assert(Dim >= 1 && Dim <= 3, "RenderQueue dim is invalid.");
      public:
        RenderQueue(const RenderQueue &) = delete;
        RenderQueue(RenderQueue &&) = delete;
        RenderQueue &operator=(const RenderQueue &) = delete;
        RenderQueue &operator=(RenderQueue &&) = delete;

        RenderQueue(uint32_t l, StorageType acc = StorageType::draw_dynamic) : RenderArray<L>(l * Dim, acc) {}

        using RenderArray<L>::Empty;

        uint32_t MaxSize()
        {
            return RenderArray<L>::MaxSize() / Dim;
        }
        uint32_t CurrentSize()
        {
            return RenderArray<L>::CurrentSize() / Dim;
        }
        void ChangeSize(uint32_t l, StorageType acc = StorageType::draw_dynamic)
        {
            if (!Empty())
                Flush();
            RenderArray<L>::ChangeSize(l * Dim, acc);
        }

        void Discard()
        {
            RenderArray<L>::Clear();
        }
        void Flush() // Draws and clears the queue.
        {
            RenderArray<L>::Send();
            switch (Dim)
            {
                case 1: RenderArray<L>::Draw(points   ); break;
                case 2: RenderArray<L>::Draw(lines    ); break;
                case 3: RenderArray<L>::Draw(triangles); break;
            }
            RenderArray<L>::Clear();
        }

        L *Add(uint32_t primitives)
        {
            if (RenderArray<L>::pos + primitives * Dim > RenderArray<L>::size)
            {
                if (primitives * Dim > RenderArray<L>::size)
                    Exceptions::Graphics::RenderArrayOverflow(Str(RenderArray<L>::size));
                Flush();
            }
            L *ret = RenderArray<L>::arr.data() + RenderArray<L>::pos;
            RenderArray<L>::pos += primitives * Dim;
            return ret;
        }

        void Insert(const L &a)
        {
            static_assert(Dim == 1, "This function only works for point queues.");
            L *dst = Add(1);
            dst[0] = a;
        }
        void Insert(const L &a, const L &b)
        {
            static_assert(Dim == 2, "This function only works for line queues.");
            L *dst = Add(2);
            dst[0] = a;
            dst[1] = b;
        }
        void Insert(const L &a, const L &b, const L &c)
        {
            static_assert(Dim == 3, "This function only works for triangle queues.");
            L *dst = Add(3);
            dst[0] = a;
            dst[1] = b;
            dst[2] = c;
        }
        void Insert(const L &a, const L &b, const L &c, const L &d) // a b d  b c d
        {
            static_assert(Dim == 3, "This function only works for triangle queues.");
            L *dst = Add(6);
            dst[0] = a;
            dst[1] = b;
            dst[2] = d;
            dst[3] = b;
            dst[4] = c;
            dst[5] = d;
        }
    };

    enum class Mirror
    {
        no = 0b00,
        x  = 0b01,
        y  = 0b10,
        xy = 0b11,
    };

    class ImageData
    {
        std::vector<u8vec4> data;
        ivec2 size;
      public:
        void LoadTGA(Utils::BinaryInput io, Mirror mirror = Mirror::no);
        void LoadCompressed(Utils::BinaryInput io);
        void LoadPNG(Utils::BinaryInput io, Mirror mirror = Mirror::no);

        void SaveTGA(Utils::BinaryOutput io, Mirror mirror = Mirror::no);
        void SaveCompressed(Utils::BinaryOutput io);
        void SavePNG(Utils::BinaryOutput io, Mirror mirror = Mirror::no);

        static ImageData FromTGA(Utils::BinaryInput io, Mirror mirror = Mirror::no)
        {
            ImageData ret;
            ret.LoadTGA(io.Move(), mirror);
            return ret;
        }
        static ImageData FromCompressed(Utils::BinaryInput io)
        {
            ImageData ret;
            ret.LoadCompressed(io.Move());
            return ret;
        }
        static ImageData FromPNG(Utils::BinaryInput io, Mirror mirror = Mirror::no)
        {
            ImageData ret;
            ret.LoadPNG(io.Move(), mirror);
            return ret;
        }

        void Premultiply()
        {
            for (auto &it : data)
            {
                float f = float(it.a) / 255.0f;
                it.r = std::round(it.r * f);
                it.g = std::round(it.g * f);
                it.b = std::round(it.b * f);
            }
        }
        void Demultiply()
        {
            for (auto &it : data)
            {
                float f = float(it.a) / 255.0f;
                if (!std::isnormal(f))
                    continue;
                it.r = std::max((int)std::round(it.r / f), 255);
                it.g = std::max((int)std::round(it.g / f), 255);
                it.b = std::max((int)std::round(it.b / f), 255);
            }
        }
        void Empty(ivec2 new_size)
        {
            Clear();

            size = new_size;
            data.resize(size.product());
        }
        void LoadFromMem(ivec2 new_size, uint8_t *mem)
        {
            Empty(new_size);
            std::memcpy(data.data(), mem, ByteSize());
        }
        void Clear()
        {
            size = {0,0};
            data.clear();
        }
        void Fill(u8vec4 color)
        {
            for (u8vec4 &it : data)
                it = color;
        }
        void *Data()
        {
            return data.data();
        }
        const void *Data() const
        {
            return data.data();
        }
        ivec2 Size() const
        {
            return size;
        }
        uint32_t ByteSize() const
        {
            return size.product() * sizeof (u8vec4);
        }
        u8vec4 &At(uvec2 pos)
        {
            return data[size.x * pos.y + pos.x];
        }
        u8vec4 At(uvec2 pos) const
        {
            return data[size.x * pos.y + pos.x];
        }
        ImageData()
        {
            size = {0,0};
        }
        ImageData(uvec2 new_size)
        {
            size = new_size;
            data.resize(size.product());
        }
        ~ImageData()
        {
            Clear();
        }
    };

    class Font;

    class FontData
    {
        friend class Font;

        struct Glyph
        {
            bool exists;
            ivec2 pos, size;
            ivec2 offset;
            int advance;
        };

        static constexpr int total_glyph_count = 0x10000; // This must be equal to the range of uint16_t.
        static constexpr int sub_buffer_count = 1024;
        static constexpr int sub_buffer_size = total_glyph_count / sub_buffer_count;

        std::vector<std::vector<Glyph>> glyph_map;
        int height, ascent, descent, line_skip;
        Font *font_ptr;

        void Alloc(Font *font, int he, int asc, int lskip)
        {
            glyph_map.resize(sub_buffer_count);
            font_ptr = font;
            height = he;
            ascent = asc;
            descent = he - asc;
            line_skip = lskip;
        }

        void AddGlyph(uint16_t glyph, ivec2 pos, ivec2 size, ivec2 offset, int advance)
        {
            int sub_buffer = glyph / sub_buffer_size;
            if (!glyph_map[sub_buffer].size())
            {
                glyph_map[sub_buffer].resize(sub_buffer_size);
                for (int i = 0; i < sub_buffer_size; i++)
                    glyph_map[sub_buffer][i] = Glyph{};
            }
            glyph_map[sub_buffer][glyph % sub_buffer_size] = {1, pos, size, offset, advance};
        }

      public:
        FontData() {}

        FontData(ArrayView<uint16_t> enc, ivec2 src, ivec2 glyph_sz, int row_len, int asc, int adv, int lskip) // Computes font data for monospaced font placed directly on the texture.
        {
            glyph_map.resize(sub_buffer_count);
            font_ptr = 0;
            height = glyph_sz.y;
            ascent = asc;
            descent = height - ascent;
            line_skip = lskip;

            for (std::size_t i = 0; i < enc.size(); i++)
            {
                AddGlyph(enc[i], {src.x + int(i) % row_len * glyph_sz.x, src.y + int(i) / row_len * glyph_sz.y}, glyph_sz, {0, -asc}, adv);
            }
        }
        /*
        FontData(ArrayView<uint16_t> enc, ivec2 src, ivec2 glyph_sz, int row_len, int asc, int adv)
            : FontData(enc, src, glyph_sz, row_len, asc, adv, glyph_sz.y) {}

        FontData(ArrayView<uint16_t> enc, ivec2 src, ivec2 glyph_sz, int row_len, int asc)
            : FontData(enc, src, glyph_sz, row_len, asc, glyph_sz.x, glyph_sz.y) {}
        FontData(ArrayView<uint16_t> enc, ivec2 src, ivec2 glyph_sz, int row_len)
            : FontData(enc, src, glyph_sz, row_len, (glyph_sz.y+1)/2, glyph_sz.x, glyph_sz.y) {}*/

        bool HasGlyph(uint16_t glyph) const
        {
            if (glyph_map[glyph / sub_buffer_size].size() == 0)
                return 0;
            return glyph_map[glyph / sub_buffer_size][glyph % sub_buffer_size].exists;
        }
        ivec2 Pos(uint16_t glyph) const
        {
            if (glyph_map[glyph / sub_buffer_size].size() == 0)
                return {0,0};
            return glyph_map[glyph / sub_buffer_size][glyph % sub_buffer_size].pos;
        }
        ivec2 Size(uint16_t glyph) const
        {
            if (glyph_map[glyph / sub_buffer_size].size() == 0)
                return {0,0};
            return glyph_map[glyph / sub_buffer_size][glyph % sub_buffer_size].size;
        }
        ivec2 Offset(uint16_t glyph) const
        {
            if (glyph_map[glyph / sub_buffer_size].size() == 0)
                return {0,0};
            return glyph_map[glyph / sub_buffer_size][glyph % sub_buffer_size].offset;
        }
        int Advance(uint16_t glyph) const // Horisontal offset to the next glyph.
        {
            if (glyph_map[glyph / sub_buffer_size].size() == 0)
                return 0;
            return glyph_map[glyph / sub_buffer_size][glyph % sub_buffer_size].advance;
        }
        int Kerning(uint16_t a, uint16_t b) const; // This function relies on the original Font object which created the current instance. It must be alive and opened.
        int Height() const {return height;}
        int Ascent() const {return ascent;}
        int Descent() const {return descent;}
        int LineSkip() const {return line_skip;} // This is an Y offset between lines. Height of the font is already added to it. Using this is not mandratory.
    };

    class Font
    {
        TTF_Font *handle;
        Utils::BinaryInput stream;

      public:
        void Open(Utils::BinaryInput input, int ptsize, int index = 0) // Warning: The file will be used while the font object is alive and opened.
        {
            Close();
            stream = (Utils::BinaryInput &&) input;
            handle = TTF_OpenFontIndexRW((SDL_RWops *)stream.RWops(), 0, ptsize, index);
            if (!handle)
                Exceptions::IO::CantParse(stream.Name(), Str("SDL ttf plugin is unable to parse font: ", TTF_GetError()));
        }
        void Close()
        {
            if (handle)
            {
                TTF_CloseFont(handle);
                handle = 0;
                stream.Close();
            }
        }

        Font()
        {
            handle = 0;
        }

        Font(Utils::BinaryInput input, int ptsize, int index = 0) : Font()
        {
            Open((Utils::BinaryInput &&) input, ptsize, index);
        }



        Font(const Font &) = delete;
        Font &operator=(const Font &) = delete;

        Font(Font &&o) : stream((Utils::BinaryInput &&) o.stream)
        {
            handle = o.handle;
            o.handle = 0;
        }
        Font &operator=(Font &&o)
        {
            if (&o == this)
                return *this;
            handle = o.handle;
            o.handle = 0;
            stream = (Utils::BinaryInput &&) o.stream;
            return *this;
        }

        ~Font()
        {
            if (handle)
                TTF_CloseFont(handle);
        }

        enum Style
        {
            // Those are bit flags and can be combined with bitwise or. `normal` is equal to 0.
            normal        = TTF_STYLE_NORMAL,
            bold          = TTF_STYLE_BOLD,
            italic        = TTF_STYLE_ITALIC,
            underline     = TTF_STYLE_UNDERLINE,
            strikethrough = TTF_STYLE_STRIKETHROUGH,
        };

        Style GetStyle() const
        {
            return (Style)TTF_GetFontStyle(handle);
        }
        void SetStyle(Style style)
        {
            if (GetStyle() == style)
                return
            TTF_SetFontStyle(handle, style);
        }

        // Outline is a contour width measured in pixels.

        int GetOutline() const
        {
            return TTF_GetFontOutline(handle);
        }
        void SetOutline(int outline)
        {
            if (GetOutline() == outline)
                return;
            TTF_SetFontOutline(handle, outline);
        }

        // Hinting is a rasterizer setting which affects the contour.

        enum class Hinting
        {
            normal = TTF_HINTING_NORMAL,
            light  = TTF_HINTING_LIGHT,
            mono   = TTF_HINTING_MONO,
            none   = TTF_HINTING_NONE,
        };

        Hinting GetHinting() const
        {
            return (Hinting)TTF_GetFontHinting(handle);
        }
        void SetHinting(Hinting hinting)
        {
            if (GetHinting() == hinting)
                return;
            TTF_SetFontHinting(handle, (int)hinting);
        }

        // Kerning means adjacent letters are more tightly packed together resulting in overlapping bounding boxes.

        bool GetKerning() const
        {
            return TTF_GetFontKerning(handle);
        }
        void SetKerning(bool kerning)
        {
            if (GetKerning() == kerning)
                return;
            TTF_SetFontHinting(handle, kerning);
        }

        /*
         * ' ##    ## ' ' ' ' ' ' ' ' ' ' ' ' ' ' \ ' ' ' ' ' ' ' \
         *   ##    ##                             |               |
         *    ##  ##    ##    ##                  | Ascent        |
         *     ####     ##    ##                  |               | Height
         *      ##      ##    ##                  |               |
         * ---- ## ----- ####### ------ Baseline  /   \           |
         *                    ##                      | Descent   |
         * . . . . . .  ####### . . . . . . . . . . . / . . . . . /
         */

        int Height() const
        {
            // Usually equal to point size specified at font creation.
            return TTF_FontHeight(handle);
        }
        int Ascent() const
        {
            return TTF_FontAscent(handle);
        }
        int Descent() const
        {
            // FontDescent(handle);
            return Height() - Ascent();
        }

        int LineSkip() const // Expected spacing between lines including glyph height. Using this is not mandratory.
        {
            return TTF_FontLineSkip(handle);
        }

        bool Monospaced() const // True if every glyph has a same width
        {
            return TTF_FontFaceIsFixedWidth(handle);
        }

        bool HasGlyph(uint16_t glyph) const
        {
            return TTF_GlyphIsProvided(handle, glyph);
        }

        /*
         *  Y
         *  |
         *  | minX   maxX
         *  |..o.......o...  maxY
         *  |  : #### #:  :
         *  |  :##  ## :  :
         *  |  :##   ##:  :  advance (horisontal)
         *  |  : ##### :  : /
         *  |  :##     :  :/
         *  0----#####----o----X  (baseline)
         *  :  :#    ##:  :
         *  :  :###### :  :
         *  '''o'''''''o'''  minY
         */

        void GlyphRawMetrics(uint16_t glyph, int *minx, int *maxx, int *miny, int *maxy, int *advance) const // Pointer args may be null.
        {
            TTF_GlyphMetrics(handle, glyph, minx, maxx, miny, maxy, advance);
        }
        int GlyphAdvance(uint16_t glyph) const // Horisontal offset to the next glyph. Current glyph width is already added to it. Using this is not mandratory.
        {
            int ret;
            GlyphRawMetrics(glyph, 0, 0, 0, 0, &ret);
            return ret;
        }
        ivec2 GlyphOffset(uint16_t glyph) const // Minimal X and Y related to the baseline.
        {
            ivec2 ret;
            GlyphRawMetrics(glyph, &ret.x, 0, 0, &ret.y, 0);
            ret.y = -ret.y;
            return ret;
        }
        ivec2 GlyphRectOffset(uint16_t glyph) const // Minimal X and Y related to the bounding box.
        {
            ivec2 ret;
            GlyphRawMetrics(glyph, &ret.x, 0, 0, &ret.y, 0);
            ret.y = Ascent()-ret.y;
            return ret;
        }
        ivec2 GlyphSize(uint16_t glyph) const
        {
            ivec2 min, max;
            GlyphRawMetrics(glyph, &min.x, &max.x, &min.y, &max.y, 0);
            return max - min;
        }
        int GlyphKerning(uint16_t a, uint16_t b) const
        {
            return TTF_GetFontKerningSizeGlyphs(handle, a, b);
        }

        std::string Name() const
        {
            const char *ret = TTF_FontFaceFamilyName(handle);
            return ret ? ret : "";
        }
        std::string StyleName() const
        {
            const char *ret = TTF_FontFaceStyleName(handle);
            return ret ? ret : "";
        }

        enum Quality {fast, fancy};

        // Uses UTF-16 for glyphs.
        void RenderGlyphs(FontData &font_data, ImageData &img, ivec2 dst, ivec2 dstsz, ArrayView<uint16_t> glyphs, bool outline = 0, Quality quality = fancy, u8vec4 color = {255,255,255,255})
        {
            SDL_Surface *surface;
            if (Utils::big_endian)
                surface = SDL_CreateRGBSurfaceFrom(img.Data(), img.Size().x, img.Size().y, 32, img.Size().x*4, 0xff << 8*3, 0xff << 8*2, 0xff << 8*1, 0xff << 8*0);
            else
                surface = SDL_CreateRGBSurfaceFrom(img.Data(), img.Size().x, img.Size().y, 32, img.Size().x*4, 0xff << 8*0, 0xff << 8*1, 0xff << 8*2, 0xff << 8*3);
            if (!surface)
                Sys::Error("Can't create a temporary surface for the font renderer.");

            font_data.Alloc(this, Height(), Ascent(), LineSkip());

            SDL_Surface *glyph_surface;

            ivec2 pixel_pos(0,0);

            int asc = Ascent();

            int column_h = 1; // 1 instead of 0 to avoid a stupid bug when no single glyph can fit into a destination rect.

            /*
             * Corner points:
             * img.At(dst + ivec2(dstsz.x-2, 0)) = {  0,  0,  0,255};
             * img.At(dst + ivec2(dstsz.x-1, 0)) = {255,255,255,255};
             * img.At(dst + ivec2(dstsz.x-1, 1)) = {  0,  0,  0,255};
             * img.At(dst + ivec2(0, dstsz.y-2)) = {  0,  0,  0,255};
             * img.At(dst + ivec2(0, dstsz.y-1)) = {255,255,255,255};
             * img.At(dst + ivec2(1, dstsz.y-1)) = {  0,  0,  0,255};
             * img.At(dst + dstsz + ivec2(-1,-2)) = {  0,  0,  0,255};
             * img.At(dst + dstsz + ivec2(-2,-1)) = {  0,  0,  0,255};
             * img.At(dst + dstsz + ivec2(-1,-1)) = {255,255,255,255};
             */

            dst += outline;
            dstsz -= outline;

            for (uint16_t it : glyphs)
            {
                if (!HasGlyph(it))
                    continue;
                glyph_surface = (quality == fancy ? TTF_RenderGlyph_Blended : TTF_RenderGlyph_Solid)(handle, it, {color.r, color.g, color.b, color.a});
                if (!glyph_surface)
                    continue;
                SDL_SetSurfaceBlendMode(glyph_surface, SDL_BLENDMODE_NONE);
                if (quality == fast)
                {
                    SDL_SetColorKey(glyph_surface, 0, 0);
                    glyph_surface->format->palette->colors[0] = {0,0,0,0};
                }

                int minx, maxx, miny, maxy, advance;
                GlyphRawMetrics(it, &minx, &maxx, &miny, &maxy, &advance);
                ivec2 tex_sz(maxx-minx, maxy-miny);

                while (pixel_pos.y + tex_sz.y+outline > dstsz.y)
                {
                    pixel_pos.y = 0;
                    pixel_pos.x += column_h;
                    column_h = 1; // 1 instead of 0 to avoid a stupid bug when no single glyph can fit into a destination rect.
                }

                ivec2 tex_pos = dst + pixel_pos;

                if (column_h < tex_sz.x+outline)
                    column_h = tex_sz.x+outline;

                if (pixel_pos.x + column_h > dstsz.x)
                {
                    SDL_FreeSurface(surface);
                    SDL_FreeSurface(glyph_surface);
                    Exceptions::Graphics::FontAtlasOverflow(Name(), Str(dstsz+1));
                }

                font_data.AddGlyph(it, tex_pos, tex_sz, {minx, -maxy}, advance);
                SDL_Rect src_rect{minx, asc-maxy, tex_sz.x, tex_sz.y};
                SDL_Rect dst_rect{tex_pos.x, tex_pos.y, tex_sz.x, tex_sz.y};
                if (SDL_BlitSurface(glyph_surface, &src_rect, surface, &dst_rect))
                {
                    SDL_FreeSurface(surface);
                    SDL_FreeSurface(glyph_surface);
                    Sys::Error(Str("Can't blit glyph #", it, " for font ", Name(), '.'));
                }
                if (outline)
                {
                    static constexpr u8vec4 outline_color(0,0,0,0);
                    for (int i = -1; i < tex_sz.x+1; i++)
                    {
                        img.At(tex_pos + ivec2(i,-1)) = outline_color;
                        img.At(tex_pos + ivec2(i,tex_sz.y)) = outline_color;
                    }
                    for (int i = -1; i < tex_sz.y+1; i++)
                    {
                        img.At(tex_pos + ivec2(-1,i)) = outline_color;
                        img.At(tex_pos + ivec2(tex_sz.x,i)) = outline_color;
                    }
                }
                pixel_pos.y += tex_sz.y+outline;
                SDL_FreeSurface(glyph_surface);
            }
            SDL_FreeSurface(surface);
        }

        // Uses UTF-8 for glyphs.
        void RenderGlyphs(FontData &font_data, ImageData &img, ivec2 dst, ivec2 dstsz, ArrayView<char> glyphs, bool outline = 0, Quality quality = fancy, u8vec4 color = {255,255,255,255})
        {
            std::size_t len = u8strlen(glyphs);
            std::cout << len;
            std::vector<uint16_t> arr(len);
            const char *ptr = glyphs;
            for (std::size_t i = 0; i < len; i++)
            {
                arr[i] = u8decode(ptr, &ptr);
            }
            RenderGlyphs(font_data, img, dst, dstsz, {arr.data(), len}, outline, quality, color);
        }
    };

    enum class Format : GLenum {};

    namespace GetFormat
    {
        inline Format RGB()
        {
            #if !defined(GL_RGB8)
            # define GL_RGB8 0
            #endif
            static GLenum ret = (Window::OpenGL::ES() ? GL_RGB565 : GL_RGB8);
            return (Format)ret;
        }
        inline Format RGBA(bool single_alpha_bit = 0)
        {
            #if !defined(GL_RGBA8)
            # define GL_RGBA8 0
            #endif
            static GLenum ret = (Window::OpenGL::ES() ? (single_alpha_bit ? GL_RGB5_A1 : GL_RGBA4) : GL_RGBA8);
            return (Format)ret;
        }
        inline Format Depth16()
        {
            return (Format)GL_DEPTH_COMPONENT16;
        }
        inline Format Depth24()
        {
            #if !defined(GL_DEPTH_COMPONENT24)
            # if defined(GL_DEPTH_COMPONENT24_OES)
            #  define GL_DEPTH_COMPONENT24 GL_DEPTH_COMPONENT24_OES
            # else
            #  define GL_DEPTH_COMPONENT24 0x81A6
            # endif
            #endif
            static bool supported = !Window::OpenGL::ES() || ExtensionSupported("GL_OES_depth24");
            static GLenum ret = (supported ? GL_DEPTH_COMPONENT24 : (GLenum)Depth16());
            return (Format)ret;
        }
        inline Format Depth32()
        {
            #if !defined(GL_DEPTH_COMPONENT32)
            # if defined(GL_DEPTH_COMPONENT32_OES)
            #  define GL_DEPTH_COMPONENT32 GL_DEPTH_COMPONENT32_OES
            # else
            #  define GL_DEPTH_COMPONENT32 0x81A7
            # endif
            #endif
            #if !defined(GL_DEPTH_COMPONENT32F)
            # define GL_DEPTH_COMPONENT32F GL_DEPTH_COMPONENT32
            #endif
            static bool supported = !Window::OpenGL::ES() || ExtensionSupported("GL_OES_depth32");
            static GLenum ret = (supported ? (Window::OpenGL::ES() ? GL_DEPTH_COMPONENT32 : GL_DEPTH_COMPONENT32F) : (GLenum)Depth24());
            return (Format)ret;
        }
        inline Format DepthBest()
        {
            return Depth32();
        }
    }


    enum WrapMode
    {
        clamp  = GL_CLAMP_TO_EDGE,
        mirror = GL_MIRRORED_REPEAT,
        repeat = GL_REPEAT,
        fill   = ForPC(GL_CLAMP_TO_BORDER) ForMobile(GL_CLAMP_TO_EDGE),
    };

    unsigned int ActiveTextureSlot();
    void SetActiveTextureSlot(unsigned int n); // You need to call this only if you use GL functions manually. Always use this instead of glActiveTexture() if you use any of Texture classes!

    template <typename Derived> class TextureBase
    {
      protected:
        int tex_id;
        GLuint handle;
        ivec2 size;
      public:
        TextureBase()
        {
            size = {0,0};
            if (!Derived::GetPool().Alloc(&tex_id))
                Sys::Error("Can't create a new texture because the texture pool is empty.");
            glGenTextures(1, &handle);
            if (!handle)
                Sys::Error("Failed to create a new texture.");
            Activate();
            glBindTexture(Derived::GetTargetName(), handle);
        }
        TextureBase(const TextureBase &) = delete;
        TextureBase(TextureBase &&) = delete;
        TextureBase &operator=(const TextureBase &) = delete;
        TextureBase &operator=(TextureBase &&) = delete;
        ~TextureBase()
        {
            glDeleteTextures(1, &handle);
            Derived::GetPool().Free(tex_id);
        }
        static uint32_t MaxCount()     {return Derived::GetPool().MaxSize();}
        static uint32_t CurrentCount() {return Derived::GetPool().CurrentSize();}

        void Activate() const {SetActiveTextureSlot(tex_id);}

        void MinLinearInterpolation(bool n) {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_MIN_FILTER, n ? GL_LINEAR : GL_NEAREST);} // This is 1 by default.
        void MagLinearInterpolation(bool n) {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_MAG_FILTER, n ? GL_LINEAR : GL_NEAREST);}
        void LinearInterpolation(bool n)    {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_MIN_FILTER, n ? GL_LINEAR : GL_NEAREST);
                                                         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, n ? GL_LINEAR : GL_NEAREST);}
        void WrapModeX(WrapMode mode) {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_WRAP_S, (GLuint)mode);}
        void WrapModeY(WrapMode mode) {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_WRAP_T, (GLuint)mode);}
        void WrapMode(WrapMode mode)  {Activate(); glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_WRAP_S, (GLuint)mode);
                                                   glTexParameteri(Derived::GetTargetName(), GL_TEXTURE_WRAP_T, (GLuint)mode);}
        GLuint Handle() const {return handle;}
        int Slot() const {return tex_id;}
        ivec2 Size() const {return size;}
    };

    class Texture : public TextureBase<Texture>
    {
        static Utils::PoolManager<int> &GetPool();
        static GLint GetTargetName();
        friend class TextureBase<Texture>;
      public:
        Texture() {}
        Texture(const ImageData &data)     {SetData(data);}
        Texture(ivec2 size, void *ptr = 0) {SetData(size, ptr);}
        void SetData(const ImageData &data)
        {
            Activate();
            size = data.Size();
            glTexImage2D(GetTargetName(), 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.Data());
        }
        void SetData(ivec2 new_size, void *ptr = 0)
        {
            Activate();
            size = new_size;
            glTexImage2D(GetTargetName(), 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
        }
    };

    class TextureCube : public TextureBase<TextureCube>
    {
        static Utils::PoolManager<int> &GetPool();
        static GLint GetTargetName();
        friend class TextureBase<TextureCube>;
      public:
        enum class Side
        {
            x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            _x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            _y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            _z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
        };

        TextureCube() {}
        TextureCube(Side side, const ImageData &data)     {SetData(side, data);}
        TextureCube(Side side, int size, void *ptr = 0) {SetData(side, size, ptr);}
        TextureCube(const ImageData *sides) {SetData(sides);}
        TextureCube(int sz, void **sides = 0)   {SetData(sz, sides);}
        void SetData(Side side, const ImageData &data)
        {
            Activate();
            size = data.Size();
            if (size.x != size.y)
                Exceptions::Graphics::BadCubeMapImage(Str(size));
            glTexImage2D((GLenum) side, 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.Data());
        }
        void SetData(Side side, int new_size, void *ptr = 0)
        {
            Activate();
            size = ivec2(new_size);
            glTexImage2D((GLenum) side, 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
        }
        void SetData(const ImageData *sides) // "sides" must point to six objects in the following order: +x -x +y -y +z -z
        {
            Activate();
            bool bad_size = 0;
            for (int i = 1; i < 6; i++) // i = 1 is here on purpose.
            {
                if (sides[i].Size() != sides[0].Size())
                {
                    bad_size = 1;
                    break;
                }
            }
            if (bad_size)
            {
                std::string err;
                for (int i = 0; i < 6; i++)
                {
                    if (i != 0) err += ", ";
                    err += Str("+-"[i%2], "xyz"[i/2], " = ", sides[i].Size());
                }
                Exceptions::Graphics::BadCubeMapImage(err);
            }
            size = sides[0].Size();
            for (int i = 0; i < 6; i++)
            {
                const Side names[6]{Side::x, Side::_x,
                                    Side::y, Side::_y,
                                    Side::z, Side::_z};
                glTexImage2D((GLenum) names[i], 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, sides[i].Data());
            }
        }
        void SetData(int new_size, void **sides = 0) // "sides" must point to six objects (data pointers) in the following order: +x -x +y -y +z -z. "sides" may be null.
        {
            Activate();
            size = ivec2(new_size);
            void *nulls[6]{};
            if (!sides) sides = nulls;
            for (int i = 0; i < 6; i++)
            {
                const Side names[6]{Side::x, Side::_x,
                                    Side::y, Side::_y,
                                    Side::z, Side::_z};
                glTexImage2D((GLenum) names[i], 0, ForPC(GL_RGBA8) ForMobile(GL_RGBA), size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, sides[i]);
            }
        }
    };

    namespace Internal
    {
        template <typename T> void SetUniform(GLint loc, const T &obj);
        #define LX_GEN_SINGLE(type, tag)       template <> P inline void SetUniform(GLint loc, const type &obj)            {C(glUniform1##tag(loc, obj);)}
        #define LX_GEN_VECTOR(type, tag, size) template <> P inline void SetUniform(GLint loc, const vec##size<type> &obj) {C(glUniform##size##tag##v(loc, 1, obj.as_array());)}
        #define LX_GEN_MATRIX(type, tag, size) template <> P inline void SetUniform(GLint loc, const mat##size<type> &obj) {C(glUniformMatrix##size##tag##v(loc, 1, 0, obj.as_array());)}
        #define C(...) __VA_ARGS__
        #define P
        LX_GEN_SINGLE(float, f)
        LX_GEN_VECTOR(float, f, 2)
        LX_GEN_VECTOR(float, f, 3)
        LX_GEN_VECTOR(float, f, 4)
        LX_GEN_SINGLE(int,   i)
        LX_GEN_VECTOR(int,   i, 2)
        LX_GEN_VECTOR(int,   i, 3)
        LX_GEN_VECTOR(int,   i, 4)
        LX_GEN_MATRIX(float, f, 2)
        LX_GEN_MATRIX(float, f, 3)
        LX_GEN_MATRIX(float, f, 4)
        #undef C
        #undef P
        #define C(...) ForPC(__VA_ARGS__) ForMobile((void)loc; (void)obj;)
        #define P WarningForMobile("This does not work on mobile platforms (matrices with different dimensions and unsigned uniforms are not supported).")
        LX_GEN_SINGLE(unsigned int, ui)
        LX_GEN_VECTOR(unsigned int, ui, 2)
        LX_GEN_VECTOR(unsigned int, ui, 3)
        LX_GEN_VECTOR(unsigned int, ui, 4)
        LX_GEN_MATRIX(float, f, 2x3)
        LX_GEN_MATRIX(float, f, 2x4)
        LX_GEN_MATRIX(float, f, 3x2)
        LX_GEN_MATRIX(float, f, 3x4)
        LX_GEN_MATRIX(float, f, 4x2)
        LX_GEN_MATRIX(float, f, 4x3)
        #undef C
        #undef P
        #undef LX_GEN_SINGLE
        #undef LX_GEN_VECTOR
        #undef LX_GEN_MATRIX
        template <> inline void SetUniform(GLint loc, const Texture &ref)     {SetUniform<GLint>(loc, ref.Slot());}
        template <> inline void SetUniform(GLint loc, const TextureCube &ref) {SetUniform<GLint>(loc, ref.Slot());}
    }

    template <typename T> void SetUniform(GLint loc, const Utils::DisableDeduction<T> &obj) {Internal::SetUniform<T>(loc, obj);}

    struct ShaderSource
    {
        std::vector<std::string> attribs, uniforms;
        std::string vertex, fragment;
    };

    class Shader
    {
        GLuint prog, vsh, fsh;
        std::vector<GLint> uniform_locs;

        static GLuint binding;

        GLint GetUniformLocation(unsigned int n) const
        {
            return uniform_locs[n];
        }
      public:
        Shader(std::string name, ShaderSource source); // Can throw ShaderCompilationError and ShaderLinkingError.

        Shader(const Shader &) = delete;
        Shader(Shader &&) = delete;
        Shader &operator=(const Shader &) = delete;
        Shader &operator=(Shader &&) = delete;

        GLuint Handle() const
        {
            return prog;
        }

        bool Enabled() const
        {
            return binding == prog;
        }
        void Use() const
        {
            if (prog == binding)
                return;
            binding = prog;
            glUseProgram(prog);
        }
        static void UseNothing()
        {
            glUseProgram(0);
        }

        template <typename T> void SetUniform(int loc,             const Utils::DisableDeduction<T> &obj) const {Use(); Graphics::SetUniform<T>(GetUniformLocation(loc),          obj);}
        template <typename T> void SetUniform(int loc, int offset, const Utils::DisableDeduction<T> &obj) const {Use(); Graphics::SetUniform<T>(GetUniformLocation(loc) + offset, obj);}

        ~Shader()
        {
            if (binding == prog)
                binding = 0;
            glDeleteShader(vsh);
            glDeleteShader(fsh);
            glDeleteProgram(prog);
        }
    };

    #if OnMobile
    #ifdef GL_DRAW_FRAMEBUFFER
    #undef GL_DRAW_FRAMEBUFFER
    #endif
    #define GL_DRAW_FRAMEBUFFER GL_FRAMEBUFFER
    #endif

    class Renderbuffer
    {
        GLuint handle;

        static GLuint binding;

      public:
        static void BindBuffer(GLuint handle)
        {
            if (handle == binding)
                return;
            binding = handle;
            glBindRenderbuffer(GL_RENDERBUFFER, handle);
        }

        GLuint Handle() const
        {
            return handle;
        }

        void Bind() const
        {
            BindBuffer(handle);
        }
        static void Unbind()
        {
            BindBuffer(0);
        }

        void AttachStorage(ivec2 sz, Format format) // Binds renderbuffer too.
        {
            Bind();
            glRenderbufferStorage(GL_RENDERBUFFER, (GLenum)format, sz.x, sz.y);
        }


        Renderbuffer()
        {
            glGenRenderbuffers(1, &handle);
        }
        Renderbuffer(ivec2 sz, Format format) // Binds renderbuffer and attaches a storage to it.
        {
            glGenRenderbuffers(1, &handle);
            AttachStorage(sz, format);
        }

        ~Renderbuffer()
        {
            if (handle == binding)
                binding = 0;
            glDeleteRenderbuffers(1, &handle);
        }

        Renderbuffer(const Renderbuffer &) = delete;
        Renderbuffer(Renderbuffer &&) = delete;
        Renderbuffer &operator=(const Renderbuffer &) = delete;
        Renderbuffer &operator=(Renderbuffer &&) = delete;
    };

    class Framebuffer
    {
        GLuint handle;

        static GLuint binding_draw;
        ForPC(static GLuint binding_read;)

        class Attachment
        {
            GLuint handle;
            GLenum type; // GL_RENDERBUFFER or GL_TEXTURE_2D or GL_TEXTURE_CUBE_MAP_*
          public:
            void Attach(GLenum slot)
            {
                if (type == GL_RENDERBUFFER)
                    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, slot, GL_RENDERBUFFER, handle);
                else
                    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, slot, type, handle, 0);
            }

            Attachment(const Texture &tex)
            {
                handle = tex.Handle();
                type = GL_TEXTURE_2D;
            }
            Attachment(const TextureCube &tex, TextureCube::Side side)
            {
                handle = tex.Handle();
                type = (GLenum)side;
            }
            Attachment(const Renderbuffer &rb)
            {
                handle = rb.Handle();
                type = GL_RENDERBUFFER;
            }
        };
      public:
        static void BindBufferRead(GLuint id)
        {
            ForPC
            (
                if (id != binding_read)
                {
                    binding_read = id;
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
                }
            )

            ForMobile
            (
                BindBufferDraw(id);
            )
        }
        static void BindBufferDraw(GLuint id)
        {
            if (id != binding_draw)
            {
                binding_draw = id;
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
            }
        }

        WarningForMobile("On mobile platforms this is replaced with BindDraw()!")
        void BindRead() const
        {
            BindBufferRead(handle);
        }
        void BindDraw() const
        {
            BindBufferDraw(handle);
        }
        WarningForMobile("On mobile platforms this is replaced with UnbindDraw()!")
        static void UnbindRead()
        {
            BindBufferRead(0);
        }
        static void UnbindDraw()
        {
            BindBufferDraw(0);
        }

        // For functions below set `att_id` to -1 to attach depth.
        void BindDrawAndAttach(Attachment att, int att_id = 0)
        {
            BindDraw();
            if (att_id < 0)
                att_id = GL_DEPTH_ATTACHMENT;
            else
                att_id = GL_COLOR_ATTACHMENT0 + att_id;
            att.Attach(att_id);
        }

        void BindDrawAndClear() const
        {
            BindDraw();
            glClear(GL_COLOR_BUFFER_BIT);
        }

        GLuint Handle() const
        {
            return handle;
        }

        Framebuffer()
        {
            glGenFramebuffers(1, &handle);
            if (!handle)
                Sys::Error("Can't create a framebuffer.");
        }
        ~Framebuffer()
        {
            if (handle == binding_draw)
                binding_draw = 0;
            ForPC
            (
                if (handle == binding_read)
                    binding_read = 0;
            )
            glDeleteFramebuffers(1, &handle);
        }

        Framebuffer(const Framebuffer &) = delete;
        Framebuffer(Framebuffer &&) = delete;
        Framebuffer &operator=(const Framebuffer &) = delete;
        Framebuffer &operator=(Framebuffer &&) = delete;
    };
}

#endif
