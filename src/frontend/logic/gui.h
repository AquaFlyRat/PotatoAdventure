#ifndef LOGIC_GUI_H_INCLUDED
#define LOGIC_GUI_H_INCLUDED

#include "../renderer2d.h"

namespace GUI
{
    // Low level

    Renderer2D &Renderer();

    namespace Fonts
    {
        const Graphics::FontData &Main();
        const Graphics::FontData &Mono();

    }

    namespace FontStyleVectors
    {
        /* Text styles:
         * \r - default
         * \1 - bold
         * \2 - italic
         * \3 - green
         * \4 - yellow
         * \5 - blue
         */
        const Renderer2D::Text::StyleVector &Main();
    }


    // High level

    void WriteLine(std::string_view line); // Line feed is automatically added at the end.

    template <typename ...P> void WriteLine(P &&... p) // Uses `Str()` (which uses `std::stringstream`) to concat it's arguments and then calls `WriteLine(std::string)`.
    {
        WriteLine(std::string_view(Str((P &&)p...)));
    }
    template <typename ...P> void WriteLine_KeepFlags(P &&... p) // Same as above, but uses `Str_()`, which keeps stream flags from previous `WriteLine(...)` or `Str()` call.
    {
        WriteLine(std::string_view(Str_((P &&)p...)));
    }
}

#endif
