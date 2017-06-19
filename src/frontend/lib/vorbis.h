#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>

namespace Internal // This removes warnings about unused stuff
{
    inline void dummy()
    {
        (void)OV_CALLBACKS_DEFAULT;
        (void)OV_CALLBACKS_NOCLOSE;
        (void)OV_CALLBACKS_STREAMONLY;
        (void)OV_CALLBACKS_STREAMONLY_NOCLOSE;
    }
}
