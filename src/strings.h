#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED

#include <algorithm>
#include <string>
#include <string_view>
#include <sstream>

namespace Strings
{
    namespace Internal
    {
        extern std::stringstream ss;
        extern const std::stringstream::fmtflags stdfmt;
    }

    // Str() resets stringstream flags first.
    // Str_() does not.

    template <typename ...P> [[nodiscard]] std::string Str(P &&... params)
    {
        Internal::ss.clear();
        Internal::ss.str("");
        Internal::ss.flags(Internal::stdfmt);
        // (Internal::ss << ... << params);
        using dummy_array = int[];
        dummy_array{(Internal::ss << params, 0)...};
        return Internal::ss.str();
    }
    template <typename ...P> [[nodiscard]] std::string Str_(P &&... params)
    {
        Internal::ss.clear();
        Internal::ss.str("");
        // (Internal::ss << ... << params);
        using dummy_array = int[];
        dummy_array{(Internal::ss << params, 0)...};
        return Internal::ss.str();
    }

    [[nodiscard]] inline std::string NormalizeStr(std::string_view str)
    {
        static constexpr char chars_to_remove[] = " \n\r\t\0";
        str.remove_prefix(std::min(str.size(), str.find_first_not_of(chars_to_remove)));
        if (str.size())
            str.remove_suffix(str.size() - str.find_last_not_of(chars_to_remove) - 1);

        auto Unprintable = [](char c) {return c < 32 || c == 127;};

        std::string ret;
        ret.reserve(str.size() + 5*std::count_if(str.begin(), str.end(), Unprintable)); // Sic! `5` is used as the factor, but unprintable chars are replaced with 6 chars.

        (void)Str(std::hex);

        for (char it : str)
        {
            if (Unprintable(it))
            {
                ret += Str_("<0x", int((unsigned char)it / 16), int((unsigned char)it % 16), '>');
            }
            else
                ret += it;
        }

        return ret;
    }

    inline namespace UTF8
    {
        inline bool u8firstbyte(const char *ptr) // Check if a pointed byte is a first byte of a symbol.
        {
            return !(*ptr & 0x80) || (*ptr & 0xc0) == 0xc0;
        }

        inline std::size_t u8strlen(const char *ptr)
        {
            std::size_t ret = 0;
            while (*ptr)
            {
                if (u8firstbyte(ptr))
                    ret++;
                ptr++;
            }
            return ret;
        }

        inline const char *u8next(const char *ptr)
        {
            while (*ptr)
            {
                ptr++;
                if (u8firstbyte(ptr))
                    break;
            }
            return ptr;
        }

        constexpr uint16_t u8invalidchar = 0xffff;

        inline uint16_t u8decode(const char *ptr, const char **next = 0) // 0xffff is returned if the value is out of range.
        {
            static constexpr uint8_t bits[5]{0b10000000,
                                             0b11000000,
                                             0b11100000,
                                             0b11110000,
                                             0b11111000};
            static constexpr uint8_t inv_bits[5]{0b01111111,
                                                 0b00111111,
                                                 0b00011111,
                                                 0b00001111,
                                                 0b00000111};

            if ((*ptr & bits[0]) == 0)
            {
                if (next) *next = ptr + 1;
                return *ptr & inv_bits[0];
            }

            uint16_t ret;
            for (int i = 1; i < 4; i++)
            {
                if ((*ptr & bits[i+1]) == bits[i])
                {
                    ret = *ptr & inv_bits[i+1];
                    for (int j = 0; j < i; j++)
                    {
                        ptr++;
                        if (!*ptr)
                        {
                            if (next) *next = ptr;
                            return u8invalidchar;
                        }
                        ret = (ret << 6) | (*ptr & inv_bits[1]);
                    }
                    if (next) *next = ptr + 1;
                    return ret;
                }
            }

            if (next) *next = u8next(ptr);

            return u8invalidchar;
        }
    }
}

using namespace Strings;

#endif
