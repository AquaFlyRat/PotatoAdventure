#define E0INTERNAL_WINDOW_H_SPECIAL_ACCESS
#define E0INTERNAL_INPUT_H_SPECIAL_ACCESS

#include "window.h"

#include "lib/gl.h"
#include "input.h"
#include "system.h"
#include "utils.h"

void Resize();

namespace Window
{
    static SDL_Window *handle = 0;
    static SDL_GLContext context_handle = 0;
    static ivec2 size, min_size, default_size;
    static bool fullscreen, maximized, resizable;
    static bool fix_window_when_fullscreen_is_disabled_once = 0;

    static bool resize_needed = 0;
    static ivec2 new_size;

    static ContextSwapMode swap_mode;

    static bool got_exit_request_at_this_tick = 0;

    namespace Init
    {
        static std::string name = "LX";
        static ivec2 size(800,600), min_size(0,0);
        static bool resizable = 0;
        static bool fullscreen = 0;
        static bool maximize = 0;
        static int display = 0;

        void Name(std::string txt)
        {
            name = txt;
        }
        void Size(ivec2 sz)
        {
            size = sz;
        }
        void MinimalSize(ivec2 sz)
        {
            min_size = sz;
        }
        void Resizable(bool r)
        {
            resizable = r;
        }
        void Fullscreen(bool f)
        {
            fullscreen = f;
        }
        void Maximize(bool m)
        {
            maximize = m;
        }
        void Display(int d)
        {
            display = d;
        }

        namespace OpenGL
        {
            ForWindows(static int major = 3, minor = 3;)
            ForMac    (static int major = 3, minor = 2;)
            ForLinux  (static int major = 3, minor = 3;)
            ForMobile (static int major = 2, minor = 0;)
            ForWindows(static ContextProfile profile = ContextProfile::compatibility;)
            ForMac    (static ContextProfile profile = ContextProfile::dont_care;)
            ForLinux  (static ContextProfile profile = ContextProfile::compatibility;)
            ForMobile (static ContextProfile profile = ContextProfile::es;)
            static ContextAcceleration acceleration = ContextAcceleration::dont_care;
            static int msaa = 0;
            static ContextCompatibility compatibility = ContextCompatibility::dont_care;
            static ivec4 color_bits = {8,8,8,0};
            static ContextSwapMode swap = ContextSwapMode::late_swap_tearing;
            static int depth_bits = 0;
            static int stencil_bits = 0;

            void Version(int maj, int min)
            {
                major = maj;
                minor = min;
            }
            void Profile(ContextProfile p)
            {
                profile = p;
            }
            void Acceleration(ContextAcceleration a)
            {
                acceleration = a;
            }
            void MSAA(int aa)
            {
                msaa = aa;
            }
            void Compatibility(ContextCompatibility c)
            {
                compatibility = c;
            }
            void ColorBits(ivec4 bits)
            {
                color_bits = bits;
            }
            void Vsync(ContextSwapMode s)
            {
                swap = s;
            }
            void DepthBits(int bits)
            {
                depth_bits = bits;
            }
            void StencilBits(int bits)
            {
                stencil_bits = bits;
            }
        }
    }

    namespace Config
    {
        static Input::KeyID fullscreen_switch_key = ForPC(Input::Key_F<12>()) ForMobile(0);
        void FullscreenSwitchKey(Input::KeyID id)
        {
            fullscreen_switch_key = id;
        }
    }

    namespace OpenGL
    {
        int Major() {return Init::OpenGL::major;}
        int Minor() {return Init::OpenGL::minor;}
        bool ES()   {return Init::OpenGL::profile == ContextProfile::embedded;}
    }

    void Initialize()
    {
        ExecuteThisOnce();

        { // Input
            MarkLocation("Input");
            Input::Initialize();
        }

        int flags = SDL_WINDOW_OPENGL;
        if (Init::resizable)
            flags |= SDL_WINDOW_RESIZABLE;
        if (Init::fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;
        if (Init::maximize)
            flags |= SDL_WINDOW_MAXIMIZED;

        fullscreen = Init::fullscreen;
        maximized = Init::maximize;
        resizable = Init::resizable;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Init::OpenGL::major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Init::OpenGL::minor);

        switch (Init::OpenGL::profile)
        {
          case ContextProfile::core:
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            break;
          case ContextProfile::compatibility:
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
            break;
          case ContextProfile::embedded:
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            break;
          case ContextProfile::dont_care:
            break;
        }
        switch (Init::OpenGL::acceleration)
        {
          case ContextAcceleration::hard:
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
            break;
          case ContextAcceleration::soft:
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);
            break;
          case ContextAcceleration::dont_care:
            break;
        }
        if (Init::OpenGL::msaa > 1)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Init::OpenGL::msaa);
        }
        if (Init::OpenGL::compatibility == ContextCompatibility::forward)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        if (Init::OpenGL::color_bits.r) SDL_GL_SetAttribute(SDL_GL_RED_SIZE    , Init::OpenGL::color_bits.r);
        if (Init::OpenGL::color_bits.g) SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  , Init::OpenGL::color_bits.g);
        if (Init::OpenGL::color_bits.b) SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   , Init::OpenGL::color_bits.b);
        if (Init::OpenGL::color_bits.a) SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  , Init::OpenGL::color_bits.a);
        if (Init::OpenGL::depth_bits  ) SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE  , Init::OpenGL::depth_bits);
        if (Init::OpenGL::stencil_bits) SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, Init::OpenGL::stencil_bits);

        default_size = Init::size;
        if (Init::resizable && Init::fullscreen)
        {
            size = Window::DisplaySize(Init::display);
            if (!Init::maximize)
                fix_window_when_fullscreen_is_disabled_once = 1;
        }
        else
            size = Init::size;

        min_size = Init::min_size;

        auto WindowError = [](std::string base_txt)
        {
            std::string profile;
            switch (Init::OpenGL::profile)
            {
                case ContextProfile::core:          profile = " Core profile"; break;
                case ContextProfile::compatibility: profile = " Compatibility profile"; break;
                case ContextProfile::embedded:      profile = " ES"; break;
                default: profile = ""; break;
            }

            std::string acc;
            switch (Init::OpenGL::acceleration)
            {
                case ContextAcceleration::hard: acc = " (hardware)"; break;
                case ContextAcceleration::soft: acc = " (software)"; break;
                default: acc = ""; break;
            }

            std::string forward;
            if (Init::OpenGL::compatibility == ContextCompatibility::forward)
                forward = " (forward compatible)";
            else
                forward = "";

            std::string msaa;
            if (Init::OpenGL::msaa > 1)
                msaa = Str(" with ", Init::OpenGL::msaa, "x MSAA");
            else
                msaa = "";

            std::string color_bits;
            if (Init::OpenGL::color_bits != ivec4(0,0,0,0))
                color_bits = Str(" with ", Init::OpenGL::color_bits, " bits per color plane");
            else
                color_bits = "";

            std::string depth;
            if (Init::OpenGL::depth_bits)
                depth = Str(" with ", Init::OpenGL::depth_bits, " depth bits");
            else
                depth = "";

            std::string stencil;
            if (Init::OpenGL::stencil_bits)
                stencil = Str(" with ", Init::OpenGL::stencil_bits, " stencil bits");
            else
                stencil = "";

            std::string sync;
            switch (Init::OpenGL::swap)
            {
                case ContextSwapMode::vsync:             sync = " with vsync enabled"; break;
                case ContextSwapMode::no_vsync:          sync = " with vsync disabled"; break;
                case ContextSwapMode::late_swap_tearing: sync = " with late swap tearing"; break;
                default: sync = ""; break;
            }


            Sys::Error(Str(base_txt, " Probably your system, video card or video driver does not support required the OpenGL version or settings.\n"
                           "Following settings were used:\n"
                           "OpenGL ", Init::OpenGL::major, '.', Init::OpenGL::minor, profile, acc, forward, msaa, color_bits, depth, stencil, sync, ".\n"
                           "Error message: `", NormalizeStr(SDL_GetError()), "`.\n"
                           "Update your video drivers or mess with GL setting using command line arguments. (Use `--help` to get a list of them.)\n"
                           "If you're using a laptop or have multiple videocards, go to your video driver settings and force it to use best available video card.\n"
                           "If all else fails, buy a new videocard or give up."));
        };

        handle = SDL_CreateWindow(Init::name.c_str(),
                                  SDL_WINDOWPOS_CENTERED_DISPLAY(Init::display), SDL_WINDOWPOS_CENTERED_DISPLAY(Init::display),
                                  size.x, size.y,
                                  flags);
        if (!handle)
            WindowError("Window creation failed.");

        if (!OnMobile && Init::min_size.any())
            SDL_SetWindowMinimumSize(handle, Init::min_size.x, Init::min_size.y);

        context_handle = SDL_GL_CreateContext(handle);
        if (!context_handle)
            WindowError("OpenGL context creation failed.");

        #if OnWindows || OnLinux || defined(ASSUME_ANDROID)
        glfl::set_function_loader(SDL_GL_GetProcAddress);
        glfl::load_gl(Init::OpenGL::major, Init::OpenGL::minor);
        #endif

        #if OnMobile && !defined(ASSUME_ANDROID)
        GLboolean status = 0;
        glGetBooleanv(GL_SHADER_COMPILER, &status);
        if (!status)
            Sys::Error("This device does not support shader compilation.");
        #endif

        while (glGetError()) {}

        swap_mode = Init::OpenGL::swap;
        switch (swap_mode)
        {
          case ContextSwapMode::no_vsync:
            if (SDL_GL_SetSwapInterval(0) != 0)
                SDL_GL_SetSwapInterval(1), swap_mode = ContextSwapMode::vsync;
            break;
          case ContextSwapMode::vsync:
            if (SDL_GL_SetSwapInterval(1) != 0)
                SDL_GL_SetSwapInterval(0), swap_mode = ContextSwapMode::no_vsync;
            break;
          case ContextSwapMode::late_swap_tearing:
            if (SDL_GL_SetSwapInterval(-1) != 0)
            {
                swap_mode = ContextSwapMode::vsync;
                if (SDL_GL_SetSwapInterval(1) != 0)
                    SDL_GL_SetSwapInterval(0), swap_mode = ContextSwapMode::no_vsync;
            }
            break;
          default:
            swap_mode = (ContextSwapMode)SDL_GL_GetSwapInterval();
            break;
        }

        // Forcing resize event.
        SDL_Event resize_event;
        resize_event.type = SDL_WINDOWEVENT;
        resize_event.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
        resize_event.window.data1 = size.x;
        resize_event.window.data2 = size.y;
        SDL_PushEvent(&resize_event);

        Tick();
    }
    void Cleanup()
    {
        ExecuteThisOnce();

        { // Input cleanup
            MarkLocation("Input");
            Input::Cleanup();
        }

        if (context_handle)
            SDL_GL_DeleteContext(context_handle);
        if (handle)
            SDL_DestroyWindow(handle);
    }

    bool GotExitRequestAtThisTick()
    {
        return got_exit_request_at_this_tick;
    }

    void Tick()
    {
        Input::PreEventsTick();

        got_exit_request_at_this_tick = 0;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
              case SDL_QUIT:
                got_exit_request_at_this_tick = 1;
                break;
              case SDL_KEYDOWN:
                if (event.key.repeat == 0)
                    Input::MoveKeyDown(event.key.keysym.scancode);
                break;
              case SDL_KEYUP:
                if (event.key.repeat == 0)
                    Input::MoveKeyUp(event.key.keysym.scancode);
                break;
              case SDL_MOUSEBUTTONDOWN:
                Input::MoveMouseButtonDown(event.button.button);
                break;
              case SDL_MOUSEBUTTONUP:
                Input::MoveMouseButtonUp(event.button.button);
                break;
              case SDL_MOUSEWHEEL:
                Input::MoveMouseWheel({event.wheel.x, event.wheel.y});
                break;
              case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:
                    new_size.x = event.window.data1;
                    new_size.y = event.window.data2;
                    resize_needed = 1;
                    break;
                  case SDL_WINDOWEVENT_MAXIMIZED:
                    maximized = 1;
                    break;
                  case SDL_WINDOWEVENT_RESTORED:
                    maximized = 0;
                    break;
                }
                break;
              case SDL_APP_TERMINATING: // This is used on mobile only. We want to exit gracefully.
              case SDL_RENDER_DEVICE_RESET: // This means that we've lost our GL context state. It's easier to just exit.
                Sys::Exit();
                break;
              case SDL_KEYMAPCHANGED: // The layout have changed, we need to resize keyboard buffer to avoid any issues.
                Input::ResetKeyboardBuffer();
                break;
            }
        }

        Input::PostEventsTick();

        if (Input::KeyPressed(Config::fullscreen_switch_key))
        {
            Fullscreen(!fullscreen);
            if (fullscreen == 0 && min_size.any())
                SDL_SetWindowMinimumSize(handle, min_size.x, min_size.y);
        }
    }

    void Update()
    {
        if (resize_needed)
        {
            resize_needed = 0;
            size = new_size;
            ::Resize();
        }
    }

    SDL_Window *Handle() {return handle;}
    SDL_GLContext ContextHandle() {return context_handle;}
    ivec2 Size() {return size;}
    bool Maximized() {return maximized;}

    void Fullscreen(bool on)
    {
        if (on == fullscreen)
            return;

        fullscreen = on;

        if (resizable && on)
        {
            SDL_DisplayMode mode = CurrentDisplayMode();
            size.x = mode.w;
            size.y = mode.h;
            SDL_SetWindowDisplayMode(handle, &mode);
        }

        SDL_SetWindowFullscreen(handle, on ? SDL_WINDOW_FULLSCREEN : 0);

        if (fix_window_when_fullscreen_is_disabled_once && !on)
        {
            fix_window_when_fullscreen_is_disabled_once = 0;
            SDL_SetWindowSize(handle, default_size.x, default_size.y);
            SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED_DISPLAY(CurrentDisplayNum()), SDL_WINDOWPOS_CENTERED_DISPLAY(CurrentDisplayNum()));
        }
    }

    void SetTitle(std::string txt)
    {
        SDL_SetWindowTitle(handle, txt.c_str());
    }
    void Resize(ivec2 new_size)
    {
        size = new_size;
        SDL_SetWindowSize(handle, new_size.x, new_size.y);
    }

    ivec2 DisplaySize(int num)
    {
        static int last_num = -1;
        static ivec2 last_size;
        if (num == last_num)
            return last_size;
        SDL_Rect tmp;
        SDL_GetDisplayBounds(num, &tmp);
        last_num = num;
        last_size = {(int)tmp.w, (int)tmp.h};
        return last_size;
    }

    ivec2 CurrentDisplaySize()
    {
        return DisplaySize(CurrentDisplayNum());
    }

    SDL_DisplayMode DisplayMode(int num)
    {
        static int last_num = -1;
        static SDL_DisplayMode last_mode;
        if (num == last_num)
            return last_mode;
        SDL_DisplayMode tmp;
        SDL_GetDesktopDisplayMode(num, &tmp);
        last_num = num;
        last_mode = tmp;
        return last_mode;
    }
    SDL_DisplayMode CurrentDisplayMode()
    {
        return DisplayMode(CurrentDisplayNum());
    }
    int CurrentDisplayNum()
    {
        return SDL_GetWindowDisplayIndex(handle);
    }

    ContextSwapMode SwapMode()
    {
        return (ContextSwapMode)swap_mode;
    }
}
