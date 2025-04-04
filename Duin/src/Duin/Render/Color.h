#pragma once

#include <cstdint>
#include <cmath>

#define LIMITRANGE(x) ((int)fmin((int)x, 255))
#define NORMTOCOLOR(x) (int)(x * 255.0f)
#define COLORTONORM(x) ((float)x / 255.0f)

namespace duin {
    struct Color {
        struct NormalizedColor { float r, b, g, a = 1.0f; };
        uint8_t r, g, b, a;

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : r(r), g(g), b(b), a(a) {}

        Color(int r, int g, int b, int a)
            : r(LIMITRANGE(r)), g(LIMITRANGE(g)), b(LIMITRANGE(b)), a(LIMITRANGE(a))
        {}

        Color(float r, float g, float b, float a)
            : r(NORMTOCOLOR(r)), g(NORMTOCOLOR(g)), b(NORMTOCOLOR(b)), a(NORMTOCOLOR(a))
        {}


        /**
         * @brief Gets color compatible with BGFX
         *
         * @return uint32_t abgr
         */
        uint32_t GetReverseColorInt()
        {
            return (((uint32_t)a << 24) |
                    ((uint32_t)b << 16) |
                    ((uint32_t)g << 8) |
                     (uint32_t)r);
        }

        NormalizedColor ColorToNorm()
        {
            return { COLORTONORM(r), COLORTONORM(g), COLORTONORM(b), COLORTONORM(a) };
        }
    };

    const Color COLOR_BLACK     = Color(0,   0,   0,   255);    // Black
    const Color COLOR_GREY      = Color(122, 122, 122, 255);    // Grey
    const Color COLOR_WHITE     = Color(255, 255, 255, 255);    // White
    const Color COLOR_RED       = Color(255, 0,   0,   255);    // Red
    const Color COLOR_GREEN     = Color(0,   128, 0,   255);    // Forest Green
    const Color COLOR_LIME      = Color(0,   255, 0,   255);    // Lime (Bright Green)
    const Color COLOR_BLUE      = Color(0,   0,   255, 255);    // Blue
    const Color COLOR_YELLOW    = Color(255, 255, 0,   255);    // Yellow
    const Color COLOR_CYAN      = Color(0,   255, 255, 255);    // Cyan
    const Color COLOR_MAGENTA   = Color(255, 0,   255, 255);    // Magenta
    const Color COLOR_GRAY      = Color(128, 128, 128, 255);    // Gray
    const Color COLOR_SILVER    = Color(192, 192, 192, 255);    // Silver
    const Color COLOR_ORANGE    = Color(255, 165, 0,   255);    // Orange
    const Color COLOR_PURPLE    = Color(128, 0,   128, 255);    // Purple
    const Color COLOR_BROWN     = Color(165, 42,  42,  255);    // Brown
    const Color COLOR_PINK      = Color(255, 192, 203, 255);    // Pink
    const Color COLOR_TEAL      = Color(0,   128, 128, 255);    // Teal
    const Color COLOR_NAVY      = Color(0,   0,   128, 255);    // Navy
    const Color COLOR_MAROON    = Color(128, 0,   0,   255);    // Maroon
    const Color COLOR_OLIVE     = Color(128, 128, 0,   255);    // Olive
    const Color COLOR_SKYBLUE   = Color(135, 206, 235, 255);    // Sky Blue
    const Color COLOR_GOLD      = Color(255, 215, 0,   255);    // Gold
    const Color COLOR_INDIGO    = Color(75,  0,   130, 255);    // Indigo
    const Color COLOR_VIOLET    = Color(238, 130, 238, 255);    // Violet
    const Color COLOR_CORAL     = Color(255, 127, 80,  255);    // Coral
    const Color COLOR_TURQUOISE = Color(64,  224, 208, 255);    // Turquoise
    const Color COLOR_CRIMSON   = Color(220, 20,  60,  255);    // Crimson
}
