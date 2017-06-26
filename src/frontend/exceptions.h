#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED

#include <exception>
#include <string>
#include <string_view>
#include <array>
#include <functional>
#include "strings.h"

// Internal declarations
namespace Exceptions
{
    class Base : public std::exception
    {
      protected:
        std::string description;

        template <typename ...P>
        void generate_description(const char *desc, char *names, P &&... p)
        {
            //((p = NormalizeStr(p)) , ...);
            using dummy_array = int[];
            dummy_array{(p = NormalizeStr(p), 0)...};

            description = desc;
            std::string *fields[]{&p...};
            std::string **current_field = fields;
            const char *current_name = names;
            bool at_separator = 1;
            while (1)
            {
                if (*names == ' ' || *names == ',' || *names == '\0')
                {
                    bool end_of_str = *names == '\0';
                    if (!at_separator)
                    {
                        *names = '\0';
                        if ((**current_field)[0])
                        {
                            description += "\n - ";
                            description += current_name;
                            description += ": ";
                            description += **current_field;
                        }
                        current_field++;
                        at_separator = 1;
                    }
                    if (end_of_str)
                        break;
                }
                else
                {
                    if (at_separator)
                    {
                        current_name = names;
                        at_separator = 0;
                    }
                }

                if (*names == '_')
                    *names = ' ';
                else if (names == current_name && *names >= 'a' && *names <= 'z')
                    *names -= 'a' - 'A';

                names++;
            }
        }

      public:
        virtual const char *what() const noexcept override final {return description.c_str();};
        virtual void update_description() = 0;
        virtual ~Base() {}
    };

    using impl_Base = Base;

    #define INTERNAL_EXCEPTION_FOR_EACH_a1  std::string p0
    #define INTERNAL_EXCEPTION_FOR_EACH_a2  std::string p0 , std::string p1
    #define INTERNAL_EXCEPTION_FOR_EACH_a3  std::string p0 , std::string p1 , std::string p2
    #define INTERNAL_EXCEPTION_FOR_EACH_a4  std::string p0 , std::string p1 , std::string p2 , std::string p3
    #define INTERNAL_EXCEPTION_FOR_EACH_a5  std::string p0 , std::string p1 , std::string p2 , std::string p3 , std::string p4
    #define INTERNAL_EXCEPTION_FOR_EACH_a6  std::string p0 , std::string p1 , std::string p2 , std::string p3 , std::string p4 , std::string p5
    #define INTERNAL_EXCEPTION_FOR_EACH_a7  std::string p0 , std::string p1 , std::string p2 , std::string p3 , std::string p4 , std::string p5 , std::string p6
    #define INTERNAL_EXCEPTION_FOR_EACH_a8  std::string p0 , std::string p1 , std::string p2 , std::string p3 , std::string p4 , std::string p5 , std::string p6 , std::string p7
    #define INTERNAL_EXCEPTION_FOR_EACH_b1  p0
    #define INTERNAL_EXCEPTION_FOR_EACH_b2  p0 , p1
    #define INTERNAL_EXCEPTION_FOR_EACH_b3  p0 , p1 , p2
    #define INTERNAL_EXCEPTION_FOR_EACH_b4  p0 , p1 , p2 , p3
    #define INTERNAL_EXCEPTION_FOR_EACH_b5  p0 , p1 , p2 , p3 , p4
    #define INTERNAL_EXCEPTION_FOR_EACH_b6  p0 , p1 , p2 , p3 , p4 , p5
    #define INTERNAL_EXCEPTION_FOR_EACH_b7  p0 , p1 , p2 , p3 , p4 , p5 , p6
    #define INTERNAL_EXCEPTION_FOR_EACH_b8  p0 , p1 , p2 , p3 , p4 , p5 , p6 , p7

    #define NEW_EXCEPTION(name, desc, field_c, ...) \
        struct name##_t final : Base \
        { \
            std::string __VA_ARGS__; \
            \
            name##_t() {} \
            \
            void update_description() override \
            { \
                char field_names[] = #__VA_ARGS__; \
                generate_description(desc, field_names, __VA_ARGS__); \
            } \
            std::array<std::reference_wrapper<std::string>, field_c> impl_get_field_refs__() {return {__VA_ARGS__};} \
        }; \
        \
        [[noreturn]] inline void name(INTERNAL_EXCEPTION_FOR_EACH_a##field_c) \
        { \
            name##_t ret; \
            auto fields = ret.impl_get_field_refs__(); \
            std::reference_wrapper<std::string> params[] {INTERNAL_EXCEPTION_FOR_EACH_b##field_c}; \
            for (int i = 0; i < field_c; i++) fields[i] = params[i].get(); \
            ret.update_description(); \
            throw ret; \
        }
    #define EXCEPTION_CATEGORY(name) namespace name {struct Base : impl_Base {void update_description() override = 0;}; using impl_Base = Base;} namespace name
}

// Exception definitions
namespace Exceptions
{
    EXCEPTION_CATEGORY(IO)
    {
        NEW_EXCEPTION(CantOpen, "Unable to open IO stream.", 2, object, message)
        NEW_EXCEPTION(BadOperation, "IO operation failure.", 3, object, action, message)
        NEW_EXCEPTION(CantParse, "Data parsing failed.", 2, source, error)
        NEW_EXCEPTION(BadData, "Data parsing failed.", 3, source, expected, received)
    }
    EXCEPTION_CATEGORY(Graphics)
    {
        NEW_EXCEPTION(RenderArrayOverflow, "Render array overflow.", 1, queue_length)
        NEW_EXCEPTION(BadCubeMapImage, "Attempt to use incorrectly sized image as a cubemap side.", 1, size)
        NEW_EXCEPTION(CantGenFontAtlas, "Not enough space in a font atlas to store all requested glyphs.", 3, font, progress, message)
        NEW_EXCEPTION(ShaderCompilationError, "Shader compilation error.", 5, shader_name, vertex_shader_status, fragment_shader_status, vertex_shader_log, fragment_shader_log)
        NEW_EXCEPTION(ShaderLinkingError, "Shader linking error.", 2, shader_name, log)
    }
    EXCEPTION_CATEGORY(Network)
    {
        NEW_EXCEPTION(CantConnect, "Can't connect to a remote host.", 1, address)
        NEW_EXCEPTION(CantResolve, "Can't resolve a host name.", 1, address)
        NEW_EXCEPTION(CantCreateServer, "Can't create a server.", 1, port)
    }
}

#endif
