#pragma once

#include <cstdint>
#include <array>
#include <algorithm>

namespace duin {

    struct Color
    {
        float r, g, b, a;

        // From normalized floats [0,1]
        Color(float _r, float _g, float _b, float _a = 1.0f)
          : r{_r}, g{_g}, b{_b}, a{_a}
        {}

        // From 0–255 ints
        Color(int _r, int _g, int _b, int _a = 255)
          : r{_r / 255.0f}
          , g{_g / 255.0f}
          , b{_b / 255.0f}
          , a{_a / 255.0f}
        {}

        // From hex 0xRRGGBB or 0xRRGGBBAA
        Color(uint32_t hex)
        {
            if (hex > 0xFFFFFFu)
            {
                // AARRGGBB in memory, but hex literal is 0xRRGGBBAA
                r = ((hex >> 24) & 0xFF) / 255.0f;
                g = ((hex >> 16) & 0xFF) / 255.0f;
                b = ((hex >>  8) & 0xFF) / 255.0f;
                a = ((hex >>  0) & 0xFF) / 255.0f;
            }
            else
            {
                r = ((hex >> 16) & 0xFF) / 255.0f;
                g = ((hex >>  8) & 0xFF) / 255.0f;
                b = ((hex >>  0) & 0xFF) / 255.0f;
                a = 1.0f;
            }
        }

        /// As 4-element array of floats [0,1], in RGBA order
        std::array<float,4> ToFloatArray() const
        {
            return { r, g, b, a };
        }

        /// As 0–255 bytes in RGBA order
        std::array<uint8_t,4> ToByteArray() const
        {
            auto clamp = [](float x){
                return uint8_t(std::clamp(x * 255.0f, 0.0f, 255.0f));
            };
            return { clamp(r), clamp(g), clamp(b), clamp(a) };
        }

        /// Packed ABGR as BGFX expects for clear color, etc:
        ///   [ byte 0 = A ] [ B ] [ G ] [ R ] (big-endian word: 0xAABBGGRR)
        uint32_t PackedABGR() const
        {
            auto bytes = ToByteArray();
            // bytes = { R, G, B, A }
            return  (uint32_t(bytes[3]) << 24)
                  | (uint32_t(bytes[2]) << 16)
                  | (uint32_t(bytes[1]) <<  8)
                  |  uint32_t(bytes[0]);
        }
    };

    // Common CSS-style colors you can tweak or expand:
    inline const Color BLACK   { 0,   0,   0   };
    inline const Color WHITE   { 255, 255, 255 };
    inline const Color RED     { 255, 0,   0   };
    inline const Color GREEN   { 0,   255, 0   };
    inline const Color BLUE    { 0,   0,   255 };
    inline const Color PINK    { 255, 192, 203 };

} // namespace duin
