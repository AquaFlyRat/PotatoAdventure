#include "network.h"

#include "utils.h"

namespace Network
{
    static bool network_init_ok = 0;
    void Initialize()
    {
        ExecuteThisOnce();
        if (SDLNet_Init())
            Sys::Error(Str("SDL network plugin init failed. Error message: `", SDLNet_GetError(), "`."));
        network_init_ok = 1;
    }
    void Cleanup()
    {
        ExecuteThisOnce();
        if (network_init_ok)
            SDLNet_Quit();
    }
}