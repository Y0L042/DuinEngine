/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/**
 * # CategoryScancode
 *
 * Defines keyboard scancodes.
 *
 * Please refer to the Best Keyboard Practices document for details on what
 * this information means and how best to use it.
 *
 * https://wiki.libsdl.org/SDL3/BestKeyboardPractices
 */

#ifndef _ScanCodes_h_
#define _ScanCodes_h_

#include <SDL3/SDL_stdinc.h>

/**
 * The SDL keyboard scancode representation.
 *
 * An SDL scancode is the physical representation of a key on the keyboard,
 * independent of language and keyboard mapping.
 *
 * Values of this type are used to represent keyboard keys, among other places
 * in the `scancode` field of the SDL_KeyboardEvent structure.
 *
 * The values in this enumeration are based on the USB usage page standard:
 * https://usb.org/sites/default/files/hut1_5.pdf
 *
 * \since This enum is available since SDL 3.2.0.
 */

/*
* All of this is directly from SDL. I simply renamed it to 
* remain consistent with my engine naming conventions.
*/

typedef enum DN_Scancode
{
    DN_SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    DN_SCANCODE_A = 4,
    DN_SCANCODE_B = 5,
    DN_SCANCODE_C = 6,
    DN_SCANCODE_D = 7,
    DN_SCANCODE_E = 8,
    DN_SCANCODE_F = 9,
    DN_SCANCODE_G = 10,
    DN_SCANCODE_H = 11,
    DN_SCANCODE_I = 12,
    DN_SCANCODE_J = 13,
    DN_SCANCODE_K = 14,
    DN_SCANCODE_L = 15,
    DN_SCANCODE_M = 16,
    DN_SCANCODE_N = 17,
    DN_SCANCODE_O = 18,
    DN_SCANCODE_P = 19,
    DN_SCANCODE_Q = 20,
    DN_SCANCODE_R = 21,
    DN_SCANCODE_S = 22,
    DN_SCANCODE_T = 23,
    DN_SCANCODE_U = 24,
    DN_SCANCODE_V = 25,
    DN_SCANCODE_W = 26,
    DN_SCANCODE_X = 27,
    DN_SCANCODE_Y = 28,
    DN_SCANCODE_Z = 29,

    DN_SCANCODE_1 = 30,
    DN_SCANCODE_2 = 31,
    DN_SCANCODE_3 = 32,
    DN_SCANCODE_4 = 33,
    DN_SCANCODE_5 = 34,
    DN_SCANCODE_6 = 35,
    DN_SCANCODE_7 = 36,
    DN_SCANCODE_8 = 37,
    DN_SCANCODE_9 = 38,
    DN_SCANCODE_0 = 39,

    DN_SCANCODE_RETURN = 40,
    DN_SCANCODE_ESCAPE = 41,
    DN_SCANCODE_BACKSPACE = 42,
    DN_SCANCODE_TAB = 43,
    DN_SCANCODE_SPACE = 44,

    DN_SCANCODE_MINUS = 45,
    DN_SCANCODE_EQUALS = 46,
    DN_SCANCODE_LEFTBRACKET = 47,
    DN_SCANCODE_RIGHTBRACKET = 48,
    DN_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    DN_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate DN_SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because DN
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    DN_SCANCODE_SEMICOLON = 51,
    DN_SCANCODE_APOSTROPHE = 52,
    DN_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    DN_SCANCODE_COMMA = 54,
    DN_SCANCODE_PERIOD = 55,
    DN_SCANCODE_SLASH = 56,

    DN_SCANCODE_CAPSLOCK = 57,

    DN_SCANCODE_F1 = 58,
    DN_SCANCODE_F2 = 59,
    DN_SCANCODE_F3 = 60,
    DN_SCANCODE_F4 = 61,
    DN_SCANCODE_F5 = 62,
    DN_SCANCODE_F6 = 63,
    DN_SCANCODE_F7 = 64,
    DN_SCANCODE_F8 = 65,
    DN_SCANCODE_F9 = 66,
    DN_SCANCODE_F10 = 67,
    DN_SCANCODE_F11 = 68,
    DN_SCANCODE_F12 = 69,

    DN_SCANCODE_PRINTSCREEN = 70,
    DN_SCANCODE_SCROLLLOCK = 71,
    DN_SCANCODE_PAUSE = 72,
    DN_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    DN_SCANCODE_HOME = 74,
    DN_SCANCODE_PAGEUP = 75,
    DN_SCANCODE_DELETE = 76,
    DN_SCANCODE_END = 77,
    DN_SCANCODE_PAGEDOWN = 78,
    DN_SCANCODE_RIGHT = 79,
    DN_SCANCODE_LEFT = 80,
    DN_SCANCODE_DOWN = 81,
    DN_SCANCODE_UP = 82,

    DN_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    DN_SCANCODE_KP_DIVIDE = 84,
    DN_SCANCODE_KP_MULTIPLY = 85,
    DN_SCANCODE_KP_MINUS = 86,
    DN_SCANCODE_KP_PLUS = 87,
    DN_SCANCODE_KP_ENTER = 88,
    DN_SCANCODE_KP_1 = 89,
    DN_SCANCODE_KP_2 = 90,
    DN_SCANCODE_KP_3 = 91,
    DN_SCANCODE_KP_4 = 92,
    DN_SCANCODE_KP_5 = 93,
    DN_SCANCODE_KP_6 = 94,
    DN_SCANCODE_KP_7 = 95,
    DN_SCANCODE_KP_8 = 96,
    DN_SCANCODE_KP_9 = 97,
    DN_SCANCODE_KP_0 = 98,
    DN_SCANCODE_KP_PERIOD = 99,

    DN_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Z.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    DN_SCANCODE_APPLICATION = 101,    /**< windows contextual menu, compose */
    DN_SCANCODE_POWER = 102,          /**< The USB document says this is a status flag,
                                        *   not a physical key - but some Mac keyboards
                                        *   do have a power key. */
    DN_SCANCODE_KP_EQUALS = 103,
    DN_SCANCODE_F13 = 104,
    DN_SCANCODE_F14 = 105,
    DN_SCANCODE_F15 = 106,
    DN_SCANCODE_F16 = 107,
    DN_SCANCODE_F17 = 108,
    DN_SCANCODE_F18 = 109,
    DN_SCANCODE_F19 = 110,
    DN_SCANCODE_F20 = 111,
    DN_SCANCODE_F21 = 112,
    DN_SCANCODE_F22 = 113,
    DN_SCANCODE_F23 = 114,
    DN_SCANCODE_F24 = 115,
    DN_SCANCODE_EXECUTE = 116,
    DN_SCANCODE_HELP = 117, /**< AL Integrated Help Center */
    DN_SCANCODE_MENU = 118, /**< Menu (show menu) */
    DN_SCANCODE_SELECT = 119,
    DN_SCANCODE_STOP = 120,  /**< AC Stop */
    DN_SCANCODE_AGAIN = 121, /**< AC Redo/Repeat */
    DN_SCANCODE_UNDO = 122,  /**< AC Undo */
    DN_SCANCODE_CUT = 123,   /**< AC Cut */
    DN_SCANCODE_COPY = 124,  /**< AC Copy */
    DN_SCANCODE_PASTE = 125, /**< AC Paste */
    DN_SCANCODE_FIND = 126,  /**< AC Find */
    DN_SCANCODE_MUTE = 127,
    DN_SCANCODE_VOLUMEUP = 128,
    DN_SCANCODE_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     DN_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     DN_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     DN_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    DN_SCANCODE_KP_COMMA = 133,
    DN_SCANCODE_KP_EQUALSAS400 = 134,

    DN_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    DN_SCANCODE_INTERNATIONAL2 = 136,
    DN_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    DN_SCANCODE_INTERNATIONAL4 = 138,
    DN_SCANCODE_INTERNATIONAL5 = 139,
    DN_SCANCODE_INTERNATIONAL6 = 140,
    DN_SCANCODE_INTERNATIONAL7 = 141,
    DN_SCANCODE_INTERNATIONAL8 = 142,
    DN_SCANCODE_INTERNATIONAL9 = 143,
    DN_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    DN_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    DN_SCANCODE_LANG3 = 146, /**< Katakana */
    DN_SCANCODE_LANG4 = 147, /**< Hiragana */
    DN_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    DN_SCANCODE_LANG6 = 149, /**< reserved */
    DN_SCANCODE_LANG7 = 150, /**< reserved */
    DN_SCANCODE_LANG8 = 151, /**< reserved */
    DN_SCANCODE_LANG9 = 152, /**< reserved */

    DN_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
    DN_SCANCODE_SYSREQ = 154,
    DN_SCANCODE_CANCEL = 155, /**< AC Cancel */
    DN_SCANCODE_CLEAR = 156,
    DN_SCANCODE_PRIOR = 157,
    DN_SCANCODE_RETURN2 = 158,
    DN_SCANCODE_SEPARATOR = 159,
    DN_SCANCODE_OUT = 160,
    DN_SCANCODE_OPER = 161,
    DN_SCANCODE_CLEARAGAIN = 162,
    DN_SCANCODE_CRSEL = 163,
    DN_SCANCODE_EXSEL = 164,

    DN_SCANCODE_KP_00 = 176,
    DN_SCANCODE_KP_000 = 177,
    DN_SCANCODE_THOUSANDSSEPARATOR = 178,
    DN_SCANCODE_DECIMALSEPARATOR = 179,
    DN_SCANCODE_CURRENCYUNIT = 180,
    DN_SCANCODE_CURRENCYSUBUNIT = 181,
    DN_SCANCODE_KP_LEFTPAREN = 182,
    DN_SCANCODE_KP_RIGHTPAREN = 183,
    DN_SCANCODE_KP_LEFTBRACE = 184,
    DN_SCANCODE_KP_RIGHTBRACE = 185,
    DN_SCANCODE_KP_TAB = 186,
    DN_SCANCODE_KP_BACKSPACE = 187,
    DN_SCANCODE_KP_A = 188,
    DN_SCANCODE_KP_B = 189,
    DN_SCANCODE_KP_C = 190,
    DN_SCANCODE_KP_D = 191,
    DN_SCANCODE_KP_E = 192,
    DN_SCANCODE_KP_F = 193,
    DN_SCANCODE_KP_XOR = 194,
    DN_SCANCODE_KP_POWER = 195,
    DN_SCANCODE_KP_PERCENT = 196,
    DN_SCANCODE_KP_LESS = 197,
    DN_SCANCODE_KP_GREATER = 198,
    DN_SCANCODE_KP_AMPERSAND = 199,
    DN_SCANCODE_KP_DBLAMPERSAND = 200,
    DN_SCANCODE_KP_VERTICALBAR = 201,
    DN_SCANCODE_KP_DBLVERTICALBAR = 202,
    DN_SCANCODE_KP_COLON = 203,
    DN_SCANCODE_KP_HASH = 204,
    DN_SCANCODE_KP_SPACE = 205,
    DN_SCANCODE_KP_AT = 206,
    DN_SCANCODE_KP_EXCLAM = 207,
    DN_SCANCODE_KP_MEMSTORE = 208,
    DN_SCANCODE_KP_MEMRECALL = 209,
    DN_SCANCODE_KP_MEMCLEAR = 210,
    DN_SCANCODE_KP_MEMADD = 211,
    DN_SCANCODE_KP_MEMSUBTRACT = 212,
    DN_SCANCODE_KP_MEMMULTIPLY = 213,
    DN_SCANCODE_KP_MEMDIVIDE = 214,
    DN_SCANCODE_KP_PLUSMINUS = 215,
    DN_SCANCODE_KP_CLEAR = 216,
    DN_SCANCODE_KP_CLEARENTRY = 217,
    DN_SCANCODE_KP_BINARY = 218,
    DN_SCANCODE_KP_OCTAL = 219,
    DN_SCANCODE_KP_DECIMAL = 220,
    DN_SCANCODE_KP_HEXADECIMAL = 221,

    DN_SCANCODE_LCTRL = 224,
    DN_SCANCODE_LSHIFT = 225,
    DN_SCANCODE_LALT = 226, /**< alt, option */
    DN_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    DN_SCANCODE_RCTRL = 228,
    DN_SCANCODE_RSHIFT = 229,
    DN_SCANCODE_RALT = 230, /**< alt gr, option */
    DN_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    DN_SCANCODE_MODE = 257, /**< I'm not sure if this is really not covered
                              *   by any of the above, but since there's a
                              *   special DN_KMOD_MODE for it I'm adding it here
                              */

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage.
     */
    /* @{ */

    DN_SCANCODE_SLEEP = 258, /**< Sleep */
    DN_SCANCODE_WAKE = 259,  /**< Wake */

    DN_SCANCODE_CHANNEL_INCREMENT = 260, /**< Channel Increment */
    DN_SCANCODE_CHANNEL_DECREMENT = 261, /**< Channel Decrement */

    DN_SCANCODE_MEDIA_PLAY = 262,           /**< Play */
    DN_SCANCODE_MEDIA_PAUSE = 263,          /**< Pause */
    DN_SCANCODE_MEDIA_RECORD = 264,         /**< Record */
    DN_SCANCODE_MEDIA_FAST_FORWARD = 265,   /**< Fast Forward */
    DN_SCANCODE_MEDIA_REWIND = 266,         /**< Rewind */
    DN_SCANCODE_MEDIA_NEXT_TRACK = 267,     /**< Next Track */
    DN_SCANCODE_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
    DN_SCANCODE_MEDIA_STOP = 269,           /**< Stop */
    DN_SCANCODE_MEDIA_EJECT = 270,          /**< Eject */
    DN_SCANCODE_MEDIA_PLAY_PAUSE = 271,     /**< Play / Pause */
    DN_SCANCODE_MEDIA_SELECT = 272,         /* Media Select */

    DN_SCANCODE_AC_NEW = 273,        /**< AC New */
    DN_SCANCODE_AC_OPEN = 274,       /**< AC Open */
    DN_SCANCODE_AC_CLOSE = 275,      /**< AC Close */
    DN_SCANCODE_AC_EXIT = 276,       /**< AC Exit */
    DN_SCANCODE_AC_SAVE = 277,       /**< AC Save */
    DN_SCANCODE_AC_PRINT = 278,      /**< AC Print */
    DN_SCANCODE_AC_PROPERTIES = 279, /**< AC Properties */

    DN_SCANCODE_AC_SEARCH = 280,    /**< AC Search */
    DN_SCANCODE_AC_HOME = 281,      /**< AC Home */
    DN_SCANCODE_AC_BACK = 282,      /**< AC Back */
    DN_SCANCODE_AC_FORWARD = 283,   /**< AC Forward */
    DN_SCANCODE_AC_STOP = 284,      /**< AC Stop */
    DN_SCANCODE_AC_REFRESH = 285,   /**< AC Refresh */
    DN_SCANCODE_AC_BOOKMARKS = 286, /**< AC Bookmarks */

    /* @} */ /* Usage page 0x0C */

    /**
     *  \name Mobile keys
     *
     *  These are values that are often used on mobile phones.
     */
    /* @{ */

    DN_SCANCODE_SOFTLEFT = 287,  /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom left
                                       of the display. */
    DN_SCANCODE_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom right
                                       of the display. */
    DN_SCANCODE_CALL = 289,      /**< Used for accepting phone calls. */
    DN_SCANCODE_ENDCALL = 290,   /**< Used for rejecting phone calls. */

    /* @} */ /* Mobile keys */

    /* Add any other keys here. */

    DN_SCANCODE_RESERVED = 400, /**< 400-500 reserved for dynamic keycodes */

    DN_SCANCODE_COUNT = 512 /**< not a key, just marks the number of scancodes for array bounds */

} DN_Scancode;

#endif /* _ScanCodes_h_ */
