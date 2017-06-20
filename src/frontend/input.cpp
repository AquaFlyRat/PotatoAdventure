#include <cstring>
#include <vector>

#define E0INTERNAL_INPUT_H_SPECIAL_ACCESS
#include "input.h"

#include "system.h"
#include "window.h"


namespace Input
{
    static std::vector<uint32_t> board, board_pr, board_re;
    static KeyID any_key_down_id, any_key_pr_id, any_key_re_id;
    static ivec2 mouse_pos, mouse_pos_prev, mouse_shift;
    static uint32_t mouse_buttons, mouse_buttons_pr, mouse_buttons_re;
    static MouseButtonID any_button_down_id, any_button_pr_id, any_button_re_id;
    static bool mouse_wheel_up, mouse_wheel_down, mouse_wheel_left, mouse_wheel_right;
    static bool mouse_focus, keyboard_focus;

    static unsigned int board_size, board_int_size;

    static ivec2 mouse_mapping_offset{0,0};
    static float mouse_mapping_scale = 1;

    static bool mmode_show = 1, mmode_show_needed = 1;
    static bool mmode_rel = 0, mmode_rel_needed = 0;
    static bool mouse_movement_needed;
    static ivec2 mouse_movement_dst;

    namespace Init
    {
        bool separate_mouse_and_touch = 0;
        void SeparateMouseAndTouch(bool s) {separate_mouse_and_touch = s;}
    }

    static void SetBit(uint32_t *p, unsigned int pos)
    {
        p[pos/32] |= 1 << (pos % 32);
    }

    static bool GetBit(const uint32_t *p, unsigned int pos)
    {
        return (p[pos/32] >> (pos % 32)) & 1;
    }

    static void ClearBits(uint32_t *p)
    {
        std::memset(p, 0, board_int_size * 4);
    }

    static int WhatBitIsSet(uint32_t n)
    {
        uint32_t tmp = (~n + 1) & n, ret = 0;
        while (tmp != 1) {tmp >>= 1; ret++;};
        return ret;
    }

    void ResetKeyboardBuffer()
    {
        int tmp;
        SDL_GetKeyboardState(&tmp);
        if (tmp <= (int)board_size)
            return;
        board_size = tmp;
        board_int_size = (board_size + 31) / 32;
        for (auto *it : {&board, &board_pr, &board_re})
        {
            it->resize(board_int_size);
            ClearBits(it->data());
        }
    }

    void Initialize()
    {
        ExecuteThisOnce();

        ResetKeyboardBuffer();

        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        mouse_pos = ((mouse_pos + mouse_mapping_offset) * mouse_mapping_scale).apply((long(*)(double))lround);

        SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, Init::separate_mouse_and_touch ? "1" : "0");
    }
    void Cleanup()
    {
        ExecuteThisOnce();
        board.clear();
        board_pr.clear();
        board_re.clear();
    }

    void MoveKeyDown(KeyID id)
    {
        SetBit(board_pr.data(), id);
        any_key_pr_id = id;
    }
    void MoveKeyUp(KeyID id)
    {
        SetBit(board_re.data(), id);
        any_key_re_id = id;
    }

    void MoveMouseButtonDown(MouseButtonID id)
    {
        uint32_t val = uint32_t(1) << (id-1);
        mouse_buttons_pr |= val;
        mouse_buttons    |= val;
        any_button_pr_id = id;
    }
    void MoveMouseButtonUp(MouseButtonID id)
    {
        uint32_t val = uint32_t(1) << (id-1);
        mouse_buttons_re |= val;
        mouse_buttons    &= ~val;
        any_button_re_id = id;
    }

    void MoveMouseWheel(ivec2 shift)
    {
        if (shift.x)
            (shift.x > 0 ? mouse_wheel_right : mouse_wheel_left) = 1;
        if (shift.y)
            (shift.y > 0 ? mouse_wheel_up : mouse_wheel_down) = 1;
    }

    void PreEventsTick()
    {
        if (mmode_show != mmode_show_needed)
            SDL_ShowCursor((mmode_show = mmode_show_needed));
        if (mmode_rel != mmode_rel_needed)
            SDL_SetRelativeMouseMode((SDL_bool)(mmode_rel = mmode_rel_needed));
        if (mouse_movement_needed)
        {
            mouse_movement_needed = 0;
            SDL_WarpMouseInWindow(Window::Handle(), mouse_movement_dst.x, mouse_movement_dst.y);
        }

        ClearBits(board_pr.data());
        ClearBits(board_re.data());
        any_key_down_id = 0;
        any_key_pr_id = 0;
        any_key_re_id = 0;
        any_button_down_id = 0;
        any_button_pr_id = 0;
        any_button_re_id = 0;
        mouse_buttons_pr = 0;
        mouse_buttons_re = 0;
        mouse_wheel_up = 0;
        mouse_wheel_down = 0;
        mouse_wheel_left = 0;
        mouse_wheel_right = 0;

        mouse_focus = (SDL_GetMouseFocus() == Window::Handle());
        keyboard_focus = (SDL_GetKeyboardFocus() == Window::Handle());
    }
    void PostEventsTick()
    {
        for (unsigned int i = 0; i < board_int_size; i++)
        {
            board[i] |= board_pr[i];
            board[i] &= ~board_re[i];
            if (board[i] && !any_key_down_id)
                any_key_down_id = WhatBitIsSet(board[i]) + i * 32;
        }

        mouse_pos_prev = mouse_pos;

        SDL_GetRelativeMouseState(&mouse_shift.x, &mouse_shift.y);
        SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

        mouse_pos = ((mouse_pos + mouse_mapping_offset) * mouse_mapping_scale).apply((long(*)(double))lround);

        if (mouse_buttons)
            any_button_down_id = WhatBitIsSet(mouse_buttons)+1;
    }


    bool KeyDown    (KeyID id) {return GetBit(board.data(), id);}
    bool KeyPressed (KeyID id) {return GetBit(board_pr.data(), id);}
    bool KeyReleased(KeyID id) {return GetBit(board_re.data(), id);}
    KeyID KeyCount() {return board_size;}
    KeyID AnyKeyDown    () {return any_key_down_id;}
    KeyID AnyKeyPressed () {return any_key_pr_id;}
    KeyID AnyKeyReleased() {return any_key_re_id;}

    std::string KeyName(KeyID id)
    {
        return SDL_GetScancodeName((SDL_Scancode)id);
    }


    void ShowMouse(bool n)
    {
        mmode_show_needed = n;
    }

    void ShowMouseImmediate(bool n)
    {
        SDL_ShowCursor((mmode_show = mmode_show_needed = n));
    }

    void RelativeMouseMode(bool n)
    {
        mmode_rel_needed = n;
    }

    void RelativeMouseModeImmediate(bool n)
    {
        SDL_SetRelativeMouseMode((SDL_bool)(mmode_rel = mmode_rel_needed = n));
    }

    bool MouseFocus()
    {
        return mouse_focus;
    }
    bool KeyboardFocus()
    {
        return keyboard_focus;
    }

    void SetMouseMapping(ivec2 offset, float scale)
    {
        mouse_mapping_offset = offset;
        mouse_mapping_scale = scale;
    }
    void ResetMouseMapping()
    {
        SetMouseMapping({0,0}, 1);
    }

    ivec2 MousePos()
    {
        return mouse_pos;
    }

    ivec2 MousePosDelta()
    {
        return mouse_pos - mouse_pos_prev;
    }

    ivec2 MouseShift()
    {
        return mouse_shift;
    }

    bool MouseInRect(ivec2 pos, ivec2 size) {return mouse_pos >= pos && mouse_pos < pos + size;}

    bool MouseButtonDown    (MouseButtonID id) {return (mouse_buttons >> (id-1)) & 1;}
    bool MouseButtonPressed (MouseButtonID id) {return (mouse_buttons_pr >> (id-1)) & 1;}
    bool MouseButtonReleased(MouseButtonID id) {return (mouse_buttons_re >> (id-1)) & 1;}

    MouseButtonID MouseButtonCount() {return 32;}
    MouseButtonID AnyMouseButtonDown    () {return any_button_down_id;}
    MouseButtonID AnyMouseButtonPressed () {return any_button_pr_id;}
    MouseButtonID AnyMouseButtonReleased() {return any_button_re_id;}
    bool MouseWheelUp   () {return mouse_wheel_up;}
    bool MouseWheelDown () {return mouse_wheel_down;}
    bool MouseWheelLeft () {return mouse_wheel_left;}
    bool MouseWheelRight() {return mouse_wheel_right;}

    void SetMousePos(ivec2 pos)
    {
        mouse_movement_needed = 1;
        mouse_movement_dst = (pos / mouse_mapping_scale - mouse_mapping_offset).apply((long(*)(double))lround);
    }
    void SetMousePosImmediate(ivec2 pos)
    {
        pos = (pos / mouse_mapping_scale - mouse_mapping_offset).apply((long(*)(double))lround);
        SDL_WarpMouseInWindow(Window::Handle(), pos.x, pos.y);
    }
}
