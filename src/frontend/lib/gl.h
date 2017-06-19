#include "../os.h"

#ifdef ASSUME_ANDROID
#include <GLFL/glfl.h>
#else
#include ForWindows(<GLFL/glfl.h>) ForLinux(<GLFL/glfl.h>) ForMac(<OpenGL/gl3.h>) ForMobile(<GLES2/gl2.h>)
#endif
