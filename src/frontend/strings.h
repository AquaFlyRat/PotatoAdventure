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

        auto Unprintable = [](char c) {return (c < 32 && c != '\n') || c == 127;};

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

    [[nodiscard]] inline std::string EscapeStr(std::string_view str)
    {
        auto EscapeSeq = [](char ch) -> const char *
        {
            switch (ch)
            {
                case '\a': return "\\a";
                case '\b': return "\\b";
                case '\f': return "\\f";
                case '\n': return "\\n";
                case '\r': return "\\r";
                case '\t': return "\\t";
                case '\v': return "\\v";
                default: return 0;
            }
        };

        std::size_t len = 0;

        for (char it : str)
        {
            if (it < 32 || it >= 127)
            {
                if (EscapeSeq(it) != 0)
                    len += 2; // For example, '\a'.
                else
                    len += 4; // For example, '\x7f'.
            }
            else
                len++;
        }

        std::string ret;
        ret.reserve(len);

        (void)Str(std::hex);

        for (char it : str)
        {
            if (it < 32 || it >= 127)
            {
                auto seq = EscapeSeq(it);
                if (seq)
                    ret += seq;
                else
                    ret += Str_("\\x", int((unsigned char)it / 16), int((unsigned char)it % 16));
            }
            else
                ret += it;
        }

        return ret;
    }

    inline namespace UTF8
    {
        constexpr uint16_t u8invalidchar = 0xffff;

        [[nodiscard]] inline bool u8firstbyte(char ch) // Check if a pointed byte is a first byte of a symbol.
        {
            return (ch & 0b1100'0000) != 0b1000'0000;
        }
        template <typename Iter> [[nodiscard]] bool u8firstbyte(Iter it)
        {
            return u8firstbyte(*it);
        }

        [[nodiscard]] inline std::size_t u8strlen(std::string_view str)
        {
            std::size_t ret = 0;
            for (char it : str)
                if (u8firstbyte(it))
                    ret++;
            return ret;
        }

        [[nodiscard]] inline std::size_t u8charlen(char ch) // If `ch` is not a first byte of a sequence, returns 0.
        {
            if ((ch & 0b1000'0000) == 0)
                return 1;
            if (!u8firstbyte(ch))
                return 0;
            for (int i = 1; i < 8; i++)
                if ((((ch & (0xff00 >> (i+1))) ^ (0xff00 >> i)) & 0xff) == 0)
                    return i;
            return 8;
        }
        template <typename Iter> [[nodiscard]] std::size_t u8charlen(Iter it)
        {
            return u8charlen(*it);
        }

        template <typename Iter> [[nodiscard]] uint16_t u8decode(Iter str) // Decodes the first character only. Returns `u8invalidchar` on failure.
        {
            switch (u8charlen(*str))
            {
              default:
                return u8invalidchar;
              case 1:
                return *str;
              case 2:
                return ((*str & 0b0001'1111) << 6) | (str[1] & 0b0011'1111);
              case 3:
                return ((*str & 0b0000'1111) << 12) | ((str[1] & 0b0011'1111) << 6) | (str[2] & 0b0011'1111);
            }
        }
    }
}

using namespace Strings;

#endif
