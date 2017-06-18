#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <cstdint>
#include <string>
#include <list>
#include <unordered_map>

#include "input_enums.h"
#include "math.h"
#include "os.h"

#define MarkLocation(x) ::Sys::CodeLocation CODE_LOCATION_OBJECT(x)

namespace Sys
{
    namespace Config
    {
        void ApplicationName(std::string name);
        void MessageNames(std::string info, std::string warning, std::string error);
        void NoCleanup(bool nc);
        void ExtraInitFlagsForSDL(int flags);
    }

    void SetFps(double fps); // It makes sense only when vsync is disabled. Default is 60. This function can be called at any time, even inside of PreInit().

    double FrameDelta(); // Length of last frame in seconds.
    uint64_t FrameDeltaClockTicks(); // Same, but measured in clock ticks.
    double TickDelta(); // Length of last tick in seconds.
    uint64_t TickDeltaClockTicks(); // Same, but measured in clock ticks.

    bool NewSecond(); // Returns 1 once every second.

    int Fps(); // These are updated once every second.
    int Tps();

    uint64_t FrameStartTime(); // Returns time point at which a current frame started.
    uint64_t TickStartTime(); // Returns time point at which a current frame started.

    uint64_t FrameCounter();
    uint64_t TickCounter();

    const char *FileName();

    namespace ExitRequestType
    {
        enum Enum
        {
            no,
            normal,
            self,
            signal_abort,
            signal_interrupt,
            signal_termination,
        };
    }

    ExitRequestType::Enum ExitRequested(); // Clears the flag when called. If the app gets exit request, you have one tick to handle it or the app will close itself.

    namespace Args
    {
        int Count();
        const char *const *Array(); // -1 th item is the executable name.
    }

    enum class MessageType {info = 0, warning = 1, error = 2};
    void Message(std::string title, std::string text, MessageType type = MessageType::info);
    void Message(std::string text, MessageType type = MessageType::info);

    class CodeLocation final
    {
      public:
        CodeLocation(const char *name); // YOU MUST STORE POINTED MEMORY BY YOURSELF
        CodeLocation(const CodeLocation &) = delete;
        CodeLocation(CodeLocation &&) = delete;
        CodeLocation &operator=(const CodeLocation &) = delete;
        CodeLocation &operator=(CodeLocation &&) = delete;
        ~CodeLocation();
    };

    void RequestExit();
    [[noreturn]] void Exit();
    [[noreturn]] void Error(std::string text); // If text == "", the app will be closed silently.

    void SetCurrentFunction(void (*ptr)());
    void (*CurrentFunction())();

    void BeginFrame();
    void EndFrame();
    void Tick();
}

#endif
