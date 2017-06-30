#ifndef OS_H_INCLUDED
#define OS_H_INCLUDED

#if defined(ANDROID) || defined(__ANDROID__) || defined(ASSUME_ANDROID)
#define E0INTERNAL_OS_ANDROID
#define E0INTERNAL_OS_TYPE_MOBILE
#elif defined(_WIN32) || defined(__WINDOWS__)
#define E0INTERNAL_OS_WINDOWS
#define E0INTERNAL_OS_TYPE_PC
#elif defined(__APPLE__) && defined(__MACH__)
#define E0INTERNAL_OS_MAC
#define E0INTERNAL_OS_TYPE_PC
#else
#define E0INTERNAL_OS_LINUX
#define E0INTERNAL_OS_TYPE_PC
#endif


#if defined(E0INTERNAL_OS_WINDOWS)
#  define ForWindows(...) __VA_ARGS__
#  define ForMac(...)
#  define ForLinux(...)
#  define ForAndroid(...)
#  define OnWindows 1
#  define OnMac     0
#  define OnLinux   0
#  define OnAndroid 0
#elif defined(E0INTERNAL_OS_MAC)
#  define ForWindows(...)
#  define ForMac(...) __VA_ARGS__
#  define ForLinux(...)
#  define ForAndroid(...)
#  define OnWindows 0
#  define OnMac     1
#  define OnLinux   0
#  define OnAndroid 0
#elif defined(E0INTERNAL_OS_LINUX)
#  define ForWindows(...)
#  define ForMac(...)
#  define ForLinux(...) __VA_ARGS__
#  define ForAndroid(...)
#  define OnWindows 0
#  define OnMac     0
#  define OnLinux   1
#  define OnAndroid 0
#elif defined(E0INTERNAL_OS_ANDROID)
#  define ForWindows(...)
#  define ForMac(...)
#  define ForLinux(...)
#  define ForAndroid(...) __VA_ARGS__
#  define OnWindows 0
#  define OnMac     0
#  define OnLinux   0
#  define OnAndroid 1
#else
#  error No OS specified.
#endif

#if defined(E0INTERNAL_OS_TYPE_PC)
#  define ForPC(...) __VA_ARGS__
#  define ForMobile(...)
#  define OnPC     1
#  define OnMobile 0
#elif defined(E0INTERNAL_OS_TYPE_MOBILE)
#  define ForPC(...)
#  define ForMobile(...) __VA_ARGS__
#  define OnPC     0
#  define OnMobile 1
#else
#  error No platform specified.
#endif

#define WarningForMobile(txt) ForMobile([[deprecated(txt)]])

#ifndef NDEBUG
#  define DEBUG(...) __VA_ARGS__
#else
#  define DEBUG(...)
#endif

#endif
