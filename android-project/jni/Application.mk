
APP_ABI := armeabi armeabi-v7a x86 mips

APP_STL := c++_shared

LOCAL_SHARED_LIBRARIES := c++_shared
APP_CFLAGS += -w
APP_CPPFLAGS += -fexceptions -frtti -I../lib/include
APP_CPPFLAGS += -std=c++14 -O3 -s
NDK_TOOLCHAIN_VERSION := clang