#ifndef INPUT_ENUMS_H_INCLUDED
#define INPUT_ENUMS_H_INCLUDED

#include "lib/sdl.h"

namespace Input
{
    using KeyID = uint16_t;
    using MouseButtonID = uint8_t;

    /* How to get KeyIDs:
     *  Key<'#'>()          # = A..Z || a..z || 0..9 || one_of( `~!@#$%^&*()-=_+,.<>/?;:'"[]{}\|)
     *  Key_Num<'#'>()      # = 0..9 || one_of(+*-/.,=)
     *  Key_Num_Enter()
     *  Key_F<#>()          # = 1..24
     *  Key_#()             # = one_of(Space,Menu,Esc,Enter,Tab,Backspace,Insert,Delete,Up,Down,Left,Right,PageUp,PageDown,Home,End,CapsLock,ScrollLock,NumLock,PrintScreen,Pause)
     *  Key_#_$()           # = one_of(Alt,Ctrl,Shift,GUI)      $ = one_of(L,R)
     */

    #define E0INTERNAL_S(x, y) template <> constexpr inline KeyID Key<(x)>() {return SDL_SCANCODE_##y;}
    #define E0INTERNAL_SS(x, y, z) E0INTERNAL_S(x, z) E0INTERNAL_S(y, z)
    template <char N> constexpr inline KeyID Key();
    E0INTERNAL_S(' ', SPACE)
    E0INTERNAL_SS('0', ')', 0) //
    E0INTERNAL_SS('1', '!', 1) //
    E0INTERNAL_SS('2', '@', 2) //
    E0INTERNAL_SS('3', '#', 3) //
    E0INTERNAL_SS('4', '$', 4) //
    E0INTERNAL_SS('5', '%', 5)  //
    E0INTERNAL_SS('6', '^', 6)  //
    E0INTERNAL_SS('7', '&', 7)  //
    E0INTERNAL_SS('8', '*', 8)  //
    E0INTERNAL_SS('9', '(', 9)  //
    E0INTERNAL_SS('A', 'a', A) //
    E0INTERNAL_SS('B', 'b', B) //
    E0INTERNAL_SS('C', 'c', C) //
    E0INTERNAL_SS('D', 'd', D) //
    E0INTERNAL_SS('E', 'e', E) //
    E0INTERNAL_SS('F', 'f', F)  //
    E0INTERNAL_SS('G', 'g', G)  //
    E0INTERNAL_SS('H', 'h', H)  //
    E0INTERNAL_SS('I', 'i', I)  //
    E0INTERNAL_SS('J', 'j', J)  //
    E0INTERNAL_SS('K', 'k', K) //
    E0INTERNAL_SS('L', 'l', L) //
    E0INTERNAL_SS('M', 'm', M) //
    E0INTERNAL_SS('N', 'n', N) //
    E0INTERNAL_SS('O', 'o', O) //
    E0INTERNAL_SS('P', 'p', P)  //
    E0INTERNAL_SS('Q', 'q', Q)  //
    E0INTERNAL_SS('R', 'r', R)  //
    E0INTERNAL_SS('S', 's', S)  //
    E0INTERNAL_SS('T', 't', T)  //
    E0INTERNAL_SS('U', 'u', U) //
    E0INTERNAL_SS('V', 'v', V) //
    E0INTERNAL_SS('W', 'w', W) //
    E0INTERNAL_SS('X', 'x', X) //
    E0INTERNAL_SS('Y', 'y', Y) //
    E0INTERNAL_SS('Z', 'z', Z)  //
    E0INTERNAL_SS('\'','"', APOSTROPHE)
    E0INTERNAL_SS(',', '<', COMMA)
    E0INTERNAL_SS('.', '>', PERIOD)
    E0INTERNAL_SS('-', '_', MINUS)
    E0INTERNAL_SS('=', '+', EQUALS)
    E0INTERNAL_SS(';', ':', SEMICOLON)
    E0INTERNAL_SS('/', '?', SLASH)
    E0INTERNAL_SS('[', '{', LEFTBRACKET)
    E0INTERNAL_SS(']', '}', RIGHTBRACKET)
    E0INTERNAL_SS('\\','|', BACKSLASH)
    E0INTERNAL_SS('`', '~', GRAVE)
    #undef E0INTERNAL_S
    #undef E0INTERNAL_SS
    template <char N> constexpr inline KeyID Key_Num();
    #define E0INTERNAL_S(x, y) template <> constexpr inline KeyID Key_Num<(x)>() {return SDL_SCANCODE_KP_##y;}
    E0INTERNAL_S('=', EQUALS)
    E0INTERNAL_S('0', 0)
    E0INTERNAL_S('1', 1)
    E0INTERNAL_S('2', 2)
    E0INTERNAL_S('3', 3)
    E0INTERNAL_S('4', 4)
    E0INTERNAL_S('5', 5)
    E0INTERNAL_S('6', 6)
    E0INTERNAL_S('7', 7)
    E0INTERNAL_S('8', 8)
    E0INTERNAL_S('9', 9)
    E0INTERNAL_S('.', PERIOD)
    E0INTERNAL_S(',', PERIOD)
    E0INTERNAL_S('+', PLUS)
    E0INTERNAL_S('-', MINUS)
    E0INTERNAL_S('*', MULTIPLY)
    E0INTERNAL_S('/', DIVIDE)
    #undef E0INTERNAL_S
    #define E0INTERNAL_F(x) template <> constexpr inline KeyID Key_F<(x)>() {return SDL_SCANCODE_F##x;}
    template <unsigned int N> constexpr inline KeyID Key_F();
    E0INTERNAL_F(1 ) E0INTERNAL_F(2 ) E0INTERNAL_F(3 ) E0INTERNAL_F(4 ) E0INTERNAL_F(5 ) E0INTERNAL_F(6 )
    E0INTERNAL_F(7 ) E0INTERNAL_F(8 ) E0INTERNAL_F(9 ) E0INTERNAL_F(10) E0INTERNAL_F(11) E0INTERNAL_F(12)
    E0INTERNAL_F(13) E0INTERNAL_F(14) E0INTERNAL_F(15) E0INTERNAL_F(16) E0INTERNAL_F(17) E0INTERNAL_F(18)
    E0INTERNAL_F(19) E0INTERNAL_F(20) E0INTERNAL_F(21) E0INTERNAL_F(22) E0INTERNAL_F(23) E0INTERNAL_F(24)
    #undef E0INTERNAL_F
    #define E0INTERNAL_C(x, y) constexpr inline KeyID Key_##x() {return SDL_SCANCODE_##y;}
    E0INTERNAL_C(Num_Enter, KP_ENTER)
    E0INTERNAL_C(Menu, MENU)
    E0INTERNAL_C(Esc, ESCAPE)
    E0INTERNAL_C(Enter, RETURN)
    E0INTERNAL_C(Tab, TAB)
    E0INTERNAL_C(Backspace, BACKSPACE)
    E0INTERNAL_C(Insert, INSERT)
    E0INTERNAL_C(Delete, DELETE)
    E0INTERNAL_C(Up, UP)
    E0INTERNAL_C(Down, DOWN)
    E0INTERNAL_C(Left, LEFT)
    E0INTERNAL_C(Right, RIGHT)
    E0INTERNAL_C(PageUp, PAGEUP)
    E0INTERNAL_C(PageDown, PAGEDOWN)
    E0INTERNAL_C(Home, HOME)
    E0INTERNAL_C(End, END)
    E0INTERNAL_C(CapsLock, CAPSLOCK)
    E0INTERNAL_C(ScrollLock, SCROLLLOCK)
    E0INTERNAL_C(NumLock, NUMLOCKCLEAR)
    E0INTERNAL_C(PrintScreen, PRINTSCREEN)
    E0INTERNAL_C(Pause, PAUSE)
    E0INTERNAL_C(Space, SPACE)
    #define E0INTERNAL_CMOD(x, y) E0INTERNAL_C(x##_L, L##y) E0INTERNAL_C(x##_R, R##y)
    E0INTERNAL_CMOD(Alt, ALT)
    E0INTERNAL_CMOD(Ctrl, CTRL)
    E0INTERNAL_CMOD(Shift, SHIFT)
    E0INTERNAL_CMOD(GUI, GUI)
    #undef E0INTERNAL_CMOD
    #undef E0INTERNAL_C
}

#endif
