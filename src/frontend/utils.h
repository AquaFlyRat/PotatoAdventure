#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <type_traits>

#include "lib/sdl.h"
#include "exceptions.h"
#include "strings.h"
#include "system.h"

#define ExecuteThisOnce() do {static bool flag = 0; if (flag) ::Sys::Error(::Strings::Str("At function ", __func__, ": Statement at " __FILE__ ":", __LINE__, " was illegally executed twice.")); flag = 1;} while (0)
#define ExecuteThisOnceQuiet() do {static bool flag = 0; if (flag) ::Sys::Exit(); flag = 1;} while (0)

#ifdef NDEBUG
#  define Assert(text, ...) (void(0))
#else
#  define Assert(text, ...) (bool(__VA_ARGS__) || (::Sys::Error(::Strings::Str("Assertion failed at `" __FILE__ "`:", __LINE__, " in `", __func__, "()`.\nMessage: `", text, "`\nExpression: `" #__VA_ARGS__, '`')), 0), void(0))
#endif

namespace Utils
{
    inline namespace Templates
    {
        template <typename T> using DisableDeduction = typename std::common_type<T>::type;

        template <unsigned int X, typename T, typename ...P> struct TypeAt {using type = typename TypeAt<X-1, P...>::type;};
        template <typename T, typename ...P> struct TypeAt<0, T, P...> {using type = T;};
    }

    template <typename T> class Object
    {
        static_assert(noexcept(std::declval<T>().~T()), "Object destructor must be noexcept.");

        bool alive;
        std::aligned_storage_t<sizeof(T), alignof(T)> data;

      public:
        using type = T;

        constexpr Object() : alive(0) {}
        Object(const Object &o)
        {
            alive = 0;
            if (o.alive)
            {
                ::new (&data) T(*o);
                alive = 1;
            }
        }
        Object(Object &&o)
        {
            alive = 0;
            if (o.alive)
            {
                ::new (&data) T((T &&)*o);
                alive = 1;
            }
        }
        Object &operator=(const Object &o)
        {
            if (&o == this)
                return *this;

            if (!alive)
            {
                ::new (&data) T(*o);
                alive = 1;
            }
            else
            {
                **this = *o;
            }

            return *this;
        }
        Object &operator=(Object &&o)
        {
            if (&o == this)
                return *this;

            if (!alive)
            {
                ::new (&data) T((T &&)*o);
                alive = 1;
            }
            else
            {
                **this = (T &&)*o;
            }

            return *this;
        }
        ~Object()
        {
            if (alive)
                (*this)->~T();
        }

        operator       T *()       {Assert("Deferencing Utils::Object which is not alive.", alive); return (      T *)&data;}
        operator const T *() const {Assert("Deferencing Utils::Object which is not alive.", alive); return (const T *)&data;}

              T *operator->()       {Assert("Deferencing Utils::Object which is not alive.", alive); return (      T *)&data;}
        const T *operator->() const {Assert("Deferencing Utils::Object which is not alive.", alive); return (const T *)&data;}

        template <typename ...P> void create(P &&... p)
        {
            destroy();
            ::new (&data) T((P &&)p...);
            alive = 1;
        }
        void destroy()
        {
            if (alive)
                (*this)->~T();
            alive = 0;
        }
    };

    namespace Clock
    {
        inline uint64_t Time()                         {return SDL_GetPerformanceCounter();}
        inline uint64_t Tps()                          {static uint64_t ret = SDL_GetPerformanceFrequency(); return ret;}
        inline uint64_t Tpms()                         {static uint64_t ret = Tps()/1000; return ret;}
        inline uint64_t SecsToTicks(long double secs)  {return secs * Tps();}
        inline long double TicksToSecs(uint64_t units) {return (long double)units / (long double)Tps();}

        inline void WaitTicks(uint64_t delay)
        {
            uint64_t begin = Time();
            while (Time() - begin < delay) {}
        }
        inline void WaitSecs(long double secs)
        {
            WaitTicks(SecsToTicks(secs));
        }
    }

    inline namespace SysInfo
    {
        constexpr bool char_is_signed = std::numeric_limits<char>::is_signed;
        constexpr bool big_endian = SDL_BYTEORDER == SDL_BIG_ENDIAN;
    }

    namespace Encodings
    {
        const uint16_t (&cp1251())[256];
    }

    template <typename ID = uint32_t, typename Index = ID> class PoolManager
    {
        static_assert(sizeof (ID) >= sizeof (Index), "`sizeof (ID)` must me larger than or equal to `sizeof (Index)`.");
        static_assert(std::is_integral<ID>::value && std::is_integral<Index>::value, "Integral types must be used.");

        Index size, pos;
        std::vector<ID> pool;
        std::vector<Index> locs;

        PoolManager(const PoolManager &) = delete;
        PoolManager(PoolManager &&) = delete;
        PoolManager &operator=(const PoolManager &) = delete;
        PoolManager &operator=(PoolManager &&) = delete;

      public:
        PoolManager(Index pool_size = 0)
        {
            Resize(pool_size);
        }
        ~PoolManager() {}

        void Resize(Index pool_size)
        {
            if ((unsigned long long)pool_size > ipow<unsigned long long>(2, sizeof (Index) * 8))
                Sys::Error(Str("Requested size (", pool_size, ") of a pool manager is larger than the pool type (", sizeof (Index)," bytes, ", ipow<unsigned long long>(2, sizeof (Index) * 8), " possible values) can support."));

            size = pool_size;
            pos = 0;
            pool.resize(size);
            locs.resize(size);
            for (Index i = 0; i < pool_size; i++)
                pool[i] = locs[i] = (Index)i;
        }

        bool Alloc(ID *ptr) // Returns 0 on fail.
        {
            if (pos >= size)
                return 0;
            *ptr = pool[pos++];
            return 1;
        }
        bool Free(ID id) // Returns 0 if such id was not allocated.
        {
            if (locs[id] >= pos)
                return 0;
            ID last_id = pool[pos-1];
            std::swap(pool[locs[id]], pool[pos-1]);
            std::swap(locs[id], locs[last_id]);
            pos--;
            return 1;
        }
        void AllocEverything()
        {
            pos = size;
        }
        void FreeEverything()
        {
            pos = 0;
        }
        Index MaxSize() const
        {
            return size;
        }
        Index CurrentSize() const
        {
            return pos;
        }
        const std::vector<ID> &Pool() const // First CurrentSize() ids in it are allocated, next (MaxSize()-CurrentSize()) ids in it are free.
        {
            return pool;
        }
        const std::vector<Index> &Indexes() const // Use Indexes()[id] to get a position of id inside of the Pool(). 0 <= id < MaxSize().
        {
            return locs;
        }
    };

    inline namespace Random
    {
        uint32_t Noise32(uint32_t in);
    }

    inline namespace SmartFlags
    {
        template <typename Tag, typename T = unsigned int> class Flag
        {
          public:
            T value;

            [[nodiscard]] constexpr operator T() const {return value;}
        };

        template <typename Tag, typename T> [[nodiscard]] constexpr Flag<T,Tag> operator|(Flag<T,Tag> a, Flag<T,Tag> b) {return {a.value | b.value};}
        template <typename Tag, typename T> [[nodiscard]] constexpr Flag<T,Tag> operator&(Flag<T,Tag> a, Flag<T,Tag> b) {return {a.value & b.value};}
        template <typename Tag, typename T> constexpr Flag<T,Tag> &operator|=(Flag<T,Tag> &a, Flag<T,Tag> b) {a.value |= b.value; return a;}
        template <typename Tag, typename T> constexpr Flag<T,Tag> &operator&=(Flag<T,Tag> &a, Flag<T,Tag> b) {a.value &= b.value; return a;}
        template <typename Tag, typename T> [[nodiscard]] constexpr Flag<T,Tag> operator*(Flag<T,Tag> a, bool b) {return {a.value * b};}
        template <typename Tag, typename T> [[nodiscard]] constexpr Flag<T,Tag> operator*(bool a, Flag<T,Tag> b) {return {a * b.value};}
        template <typename Tag, typename T> constexpr Flag<T,Tag> &operator*=(Flag<T,Tag> &a, Flag<T,Tag> b) {a.value *= b.value; return a;}
    }

    class TickStabilizer
    {
        uint64_t tick_len;
        int max_ticks;

        uint64_t accumulator;
        bool new_frame;

        bool lag;

      public:
        TickStabilizer() : TickStabilizer(60) {}
        TickStabilizer(double freq, int max_ticks_per_frame = 8)
        {
            SetFrequency(freq);
            SetMaxTicksPerFrame(max_ticks_per_frame);
            Reset();
        }

        void SetFrequency(double freq)
        {
            tick_len = Clock::Tps() / freq;
        }
        void SetMaxTicksPerFrame(int n) // Set to 0 to disable the limit.
        {
            max_ticks = n;
        }
        void Reset()
        {
            accumulator = 0;
            new_frame = 1;
            lag = 0;
        }

        bool Lag() // Flag resets after this function is called. The flag is set to 1 if the amount of ticks per last frame was limited due to reaching the limit.
        {
            if (lag)
            {
                lag = 0;
                return 1;
            }
            return 0;
        }

        double Frequency() const
        {
            return Utils::Clock::Tps() / double(tick_len);
        }
        uint64_t ClockTicksPerTick() const
        {
            return tick_len;
        }
        int MaxTicksPerFrame() const
        {
            return max_ticks;
        }

        bool Tick(uint64_t delta = Sys::FrameDeltaClockTicks())
        {
            if (new_frame)
                accumulator += delta;

            if (accumulator >= tick_len)
            {
                if (max_ticks && accumulator > tick_len * max_ticks)
                {
                    accumulator = tick_len * max_ticks;
                    lag = 1;
                }
                accumulator -= tick_len;
                new_frame = 0;
                return 1;
            }
            else
            {
                new_frame = 1;
                return 0;
            }
        }

        double Time() const
        {
            return accumulator / double(tick_len);
        }
    };

    class IO
    {
        mutable SDL_RWops *rwops;
        std::string name;
      public:

        // These work same as vanilla C file modes: {r|w|a}[+][b].
        enum class Type {text, binary};
        enum class Mode {read,      write,      append,
                         read_plus, write_plus, append_plus}; // Do not reorder these: r, w, a, r+, w+, a+

        void OpenMemory(void *mem, int size)
        {
            Close();
            name = "Memory location at ";
            name += Str("0x", std::hex, std::setw(2 * sizeof (void *)), std::setfill('0'), (std::uintptr_t)mem);
            name += Str(" of size ", size, " opened for reading/writing");
            rwops = SDL_RWFromMem(mem, size);
            if (!rwops)
                Exceptions::IO::CantOpen(name, SDL_GetError());
        }
        void OpenConstMemory(const void *mem, int size)
        {
            Close();
            name = "Memory location at ";
            name += Str("0x", std::hex, std::setw(2 * sizeof (void *)), std::setfill('0'), (std::uintptr_t)mem);
            name += Str(" of size ", size, " opened for reading");
            rwops = SDL_RWFromConstMem(mem, size);
            if (!rwops)
                Exceptions::IO::CantOpen(name, SDL_GetError());
        }
        void OpenFile(std::string fname, Type type, Mode mode)
        {
            #if OnWindows

            // Disclaimer: On Windows vanilla SDL_RWops writes to files very slowly, 64 to 80 times slower than standard IO.
            // This is true for SDL 2.0.4. On later versions this may be fixed. If so, remove an entire #if #endif section.
            // Following code replaces WinAPI-based io with cstdio-based io.
            // The code was mostly taken from `SDL_rwops.c` and tweaked to work with C++ compiler.
            // The changes included lambda-fying, replacing Sint64 with int64_t, adding `std::` here and there.

            // BUT MORE IMPORTANTLY it uses different structure field names than the original.
            // context->hidden.stdio.fp        was replaced with hidden_stdio_fp
            // context->hidden.stdio.autoclose was replaced with hidden_stdio_autoclose
            // In cases when it was not `context`->hidden..., a variable was RENAMED to `context`.

            #define hidden_stdio_fp        ((FILE * &)context->hidden.windowsio.h)
            #define hidden_stdio_autoclose ((SDL_bool &)context->hidden.windowsio.append)

            // ** Begin copied code **

            static auto stdio_size = [](SDL_RWops * context) -> int64_t
            {
                int64_t pos, size;

                pos = SDL_RWseek(context, 0, RW_SEEK_CUR);
                if (pos < 0) {
                    return -1;
                }
                size = SDL_RWseek(context, 0, RW_SEEK_END);

                SDL_RWseek(context, pos, RW_SEEK_SET);
                return size;
            };

            static auto stdio_seek = [](SDL_RWops * context, int64_t offset, int whence) -> int64_t
            {
                #ifdef HAVE_FSEEKO64
                if (std::fseeko64(hidden_stdio_fp, (off64_t)offset, whence) == 0) {
                    return std::ftello64(hidden_stdio_fp);
                }
                #elif defined(HAVE_FSEEKO)
                if (std::fseeko(hidden_stdio_fp, (off_t)offset, whence) == 0) {
                    return std::ftello(hidden_stdio_fp);
                }
                #elif defined(HAVE__FSEEKI64)
                if (std::_fseeki64(hidden_stdio_fp, offset, whence) == 0) {
                    return std::_ftelli64(hidden_stdio_fp);
                }
                #else
                if (std::fseek(hidden_stdio_fp, offset, whence) == 0) {
                    return std::ftell(hidden_stdio_fp);
                }
                #endif
                return SDL_Error(SDL_EFSEEK);
            };

            static auto stdio_read = [](SDL_RWops * context, void *ptr, std::size_t size, std::size_t maxnum) -> std::size_t
            {
                std::size_t nread;

                nread = std::fread(ptr, size, maxnum, hidden_stdio_fp);
                if (nread == 0 && std::ferror(hidden_stdio_fp)) {
                    SDL_Error(SDL_EFREAD);
                }
                return nread;
            };

            static auto stdio_write = [](SDL_RWops * context, const void *ptr, std::size_t size, std::size_t num) -> std::size_t
            {
                std::size_t nwrote;

                nwrote = std::fwrite(ptr, size, num, hidden_stdio_fp);
                if (nwrote == 0 && std::ferror(hidden_stdio_fp)) {
                    SDL_Error(SDL_EFWRITE);
                }
                return nwrote;
            };

            static auto stdio_close = [](SDL_RWops * context) -> int
            {
                int status = 0;
                if (context) {
                    if (hidden_stdio_autoclose) {
                        /* WARNING:  Check the return value here! */
                        if (std::fclose(hidden_stdio_fp) != 0) {
                            status = SDL_Error(SDL_EFWRITE);
                        }
                    }
                    SDL_FreeRW(context);
                }
                return status;
            };

            static auto RWFromFP = [](FILE * fp, SDL_bool autoclose) -> SDL_RWops *
            {
                SDL_RWops *context = 0;

                context = SDL_AllocRW();
                if (context != 0) {
                    context->size = stdio_size;
                    context->seek = stdio_seek;
                    context->read = stdio_read;
                    context->write = stdio_write;
                    context->close = stdio_close;
                    hidden_stdio_fp = fp;
                    hidden_stdio_autoclose = autoclose;
                    context->type = SDL_RWOPS_STDFILE;
                }
                return context;
            };

            static auto SDL_RWFromFile = [](const char *file, const char *mode) -> SDL_RWops *
            {
                SDL_RWops *context = 0;
                if (!file || !*file || !mode || !*mode) {
                    SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
                    return 0;
                }

                FILE *fp = std::fopen(file, mode);

                if (fp == 0) {
                    SDL_SetError("Couldn't open %s", file);
                } else {
                    context = RWFromFP(fp, (SDL_bool)1);
                }

                return context;
            };

            // ** End copied code **

            #undef hidden_stdio_fp
            #undef hidden_stdio_autoclose

            #endif

            Close();
            bool plus = int(mode) >= 3;
            bool bin = type == Type::binary;
            int mode_id = int(mode)%3;
            char mode_ch = "rwa"[mode_id];
            const char *mode_names[] {"reading","writing","appending"};
            name = Str("File \"", fname, "\" opened for ", mode_names[mode_id], (bin ? " binary" : " text"), (plus ? " extended." : "."));
            char m[4]{mode_ch,
                      (plus || bin ? (plus ? '+' : 'b') : '\0'),
                      (plus && bin ? 'b' : '\0'),
                      '\0'};
            rwops = SDL_RWFromFile(fname.c_str(), m);
            if (!rwops)
                Exceptions::IO::CantOpen(name, SDL_GetError());
        }

        void OpenTextFile(std::string name, Mode mode)
        {
            OpenFile(name, Type::text, mode);
        }
        void OpenBinaryFile(std::string name, Mode mode)
        {
            OpenFile(name, Type::binary, mode);
        }

        static IO FromMemory(void *mem, int size)
        {
            IO ret;
            ret.OpenMemory(mem, size);
            return ret;
        }
        static IO FromConstMemory(const void *mem, int size)
        {
            IO ret;
            ret.OpenConstMemory(mem, size);
            return (IO &&)ret;
        }
        static IO FromFile(std::string name, Type type, Mode mode)
        {
            IO ret;
            ret.OpenFile(name, type, mode);
            return (IO &&)ret;
        }
        static IO FromTextFile(std::string name, Mode mode)
        {
            return FromFile(name, Type::text, mode);
        }
        static IO FromBinaryFile(std::string name, Mode mode)
        {
            return FromFile(name, Type::binary, mode);
        }

        void Close()
        {
            if (rwops)
            {
                rwops->close(rwops); // It should also free pointed memory.
                rwops = 0;
                name = "";
            }
        }


        int64_t Size() const // Returns size in bytes. -1 if not available.
        {
            return rwops->size(rwops);
        }

        // For bytes
        std::size_t Read(void *dst, std::size_t obj_sz, std::size_t obj_c) const // Reads obj_c objects of size obj_sz. Returns a number of objects read or 0 on EOF/error.
        {
            return rwops->read(rwops, dst, obj_sz, obj_c);
        }
        std::size_t Write(const void *src, std::size_t obj_sz, std::size_t obj_c) const // Writes obj_c objects of size obj_sz. Returns a number of objects written or 0 on error.
        {
            return rwops->write(rwops, src, obj_sz, obj_c);
        }

        // Templated.
        template <typename T> std::size_t Read(T *dst, std::size_t count) const
        {
            return Read(dst, sizeof (T), count);
        }
        template <typename T> std::size_t Write(const T *src, std::size_t count) const
        {
            return Write(src, sizeof (T), count);
        }

        // For singular objects.
        template <typename T> bool Read(T &dst) const
        {
            return Read<T>(&dst, 1);
        }
        template <typename T> bool Write(const T &src) const
        {
            return Write<T>(&src, 1);
        }

        // These throw an exception in case of error/EOF.
        template <typename T> void ReadEx(T *dst, std::size_t count) const
        {
            if (Read<T>(dst, count) != count)
                Exceptions::IO::BadOperation(name, "Reading", SDL_GetError());
        }
        template <typename T> void WriteEx(const T *src, std::size_t count) const
        {
            if (Write<T>(src, count) != count)
                Exceptions::IO::BadOperation(name, "Writing", SDL_GetError());
        }

        // For singular objects.
        template <typename T> void ReadEx(T &dst) const
        {
            ReadEx<T>(&dst, 1);
        }
        template <typename T> void WriteEx(const T &src) const
        {
            WriteEx<T>(&src, 1);
        }
        template <typename T> T ReadEx() const
        {
            T dst;
            ReadEx<T>(&dst, 1);
            return dst;
        }

        int64_t SeekRel(int64_t offset) const // Shifts cursor, returns new position in bytes or -1 on error. Some types of streams don't support moving cursor.
        {
            return rwops->seek(rwops, offset, RW_SEEK_CUR);
        }

        int64_t SeekAbs(int64_t offset) const // Sets cursor position, returns new position in bytes or -1 on error. Some types of streams don't support moving cursor.
        {
            return rwops->seek(rwops, offset, RW_SEEK_SET);
        }

        int64_t SeekEnd(int64_t offset) const // Sets cursor relative to the end of stream, returns new position in bytes or -1 on error. Some types of streams don't support moving cursor.
        {
            return rwops->seek(rwops, offset, RW_SEEK_END);
        }

        int64_t Tell() const // Returns cursor location.
        {
            return rwops->seek(rwops, 0, RW_SEEK_CUR);
        }

        const SDL_RWops *RWops() const {return rwops;}
        std::string Name() const {return name;}
        bool Opened() const {return rwops;}
        IO &&Move() {return (IO &&)*this;}

        IO()
        {
            rwops = 0;
        }

        IO(const IO &o) = delete;

        IO(IO &&o)
        {
            rwops = o.rwops;
            name = (std::string &&)o.name;
            o.rwops = 0;
        }

        IO &operator=(const IO &o) = delete;

        IO &operator=(IO &&o)
        {
            if (&o == this)
                return *this;
            Close();
            rwops = o.rwops;
            name = (std::string &&)o.name;
            o.rwops = 0;
            return *this;
        }

        ~IO()
        {
            Close();
        }
    };

    struct TextInput : IO
    {
        TextInput() {}
        TextInput(std::string fname) : IO(IO::FromTextFile(fname, IO::Mode::read)) {}
        TextInput(const char *fname) : IO(IO::FromTextFile(fname, IO::Mode::read)) {}
        TextInput(const void *mem, int size) : IO(IO::FromConstMemory(mem, size)) {}
        TextInput &&Move() {return (TextInput &&)*this;}
    };
    struct BinaryInput : IO
    {
        BinaryInput() {}
        BinaryInput(std::string fname) : IO(IO::FromBinaryFile(fname, IO::Mode::read)) {}
        BinaryInput(const char *fname) : IO(IO::FromBinaryFile(fname, IO::Mode::read)) {}
        BinaryInput(const void *mem, int size) : IO(IO::FromConstMemory(mem, size)) {}
        BinaryInput &&Move() {return (BinaryInput &&)*this;}
    };
    struct TextOutput : IO
    {
        TextOutput() {}
        TextOutput(std::string fname) : IO(IO::FromTextFile(fname, IO::Mode::write)) {}
        TextOutput(const char *fname) : IO(IO::FromTextFile(fname, IO::Mode::write)) {}
        TextOutput(void *mem, int size) : IO(IO::FromMemory(mem, size)) {}
        TextOutput &&Move() {return (TextOutput &&)*this;}
    };
    struct BinaryOutput : IO
    {
        BinaryOutput() {}
        BinaryOutput(std::string fname) : IO(IO::FromBinaryFile(fname, IO::Mode::write)) {}
        BinaryOutput(const char *fname) : IO(IO::FromBinaryFile(fname, IO::Mode::write)) {}
        BinaryOutput(void *mem, int size) : IO(IO::FromMemory(mem, size)) {}
        BinaryOutput &&Move() {return (BinaryOutput &&)*this;}
    };

    inline namespace Proxy
    {
        template <typename T> class ArrayProxy
        {
            T *first;
            std::size_t length;

          public:
            using type = typename std::remove_const<T>::type;
            static constexpr bool readonly = std::is_const<T>::value;

            T &operator*() const {return *first;}
            operator T *() const {return first;}

            T *begin() const {return first;}
            T *end() const {return first+length;}

            typename std::conditional<readonly, const void *, void *>::type data() const {return first;}
            std::size_t size() const {return length;}

            ArrayProxy() // Null.
            {
                first = 0;
                length = 0;
            }
            ArrayProxy(T &ptr) // From a single object.
            {
                first = &ptr;
                length = 1;
            }
            ArrayProxy(T *a, T *b) // From a pair of pointers.
            {
                first = a;
                length = b - a;
            }
            ArrayProxy(T *ptr, std::size_t len) // From a pointer and a length.
            {
                first = ptr;
                length = len;
            }
            template <typename TT, typename = std::enable_if_t<std::is_same<std::remove_const_t<T>, std::remove_const_t<std::remove_reference_t<decltype(*std::begin(std::declval<TT&&>()))>>>::value>>
            ArrayProxy(TT && o) // From an array or a container which uses pointers as iterators. Use this with caution on temporary containers.
            {
                static_assert(std::is_pointer<decltype(std::begin(o))>::value, "Underlying container must use pointers as iterators or must use contiguous storage.");
                static_assert(readonly || (!std::is_const<std::remove_pointer_t<decltype(std::begin(o))>>::value), "Attempt to bind read-write proxy to a const object.");
                first = std::begin(o);
                length = std::end(o) - std::begin(o);
            }
            ArrayProxy(std::initializer_list<type> list) // From std::initializer_list (which is always const). Use this with caution on temporary lists.
            {
                first = list.begin();
                length = list.size();
            }
            template <std::size_t S> ArrayProxy(std::array<type, S> &arr) // From std::array.
            {
                // This function may be redundant on some systems, but this is not guaranteed.
                first = &*arr.begin();
                length = arr.size();
            }
            template <std::size_t S> ArrayProxy(const std::array<type, S> &arr) // From const std::array. Use this with caution on temporary arrays.
            {
                // This function may be redundant on some systems, but this is not guaranteed.
                static_assert(readonly, "Attempt to bind read-write proxy to a const object.");
                first = &*arr.begin();
                length = arr.size();
            }
            ArrayProxy(std::vector<type> &arr) // From std::vector.
            {
                first = &*arr.begin();
                length = arr.size();
            }
            ArrayProxy(const std::vector<type> &arr) // From const std::vector. Use this with caution on temporary vectors.
            {
                static_assert(readonly, "Attempt to bind read-write proxy to a const object.");
                first = &*arr.begin();
                length = arr.size();
            }
        };

        template <typename T> using ArrayView = ArrayProxy<const T>;
    }
}

using namespace Utils::Proxy;

#endif

