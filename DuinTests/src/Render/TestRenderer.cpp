#include <doctest.h>
#include <Duin/Render/Renderer.h>
#include <Duin/Render/Color.h>
#include <Duin/Core/Utils/UUID.h>
#include <cstdint>

namespace TestRenderer
{
TEST_SUITE("Color")
{
    TEST_CASE("Constructor - Float values")
    {
        SUBCASE("Full RGBA")
        {
            duin::Color color(0.5f, 0.25f, 0.75f, 1.0f);
            CHECK(color.r == doctest::Approx(0.5f));
            CHECK(color.g == doctest::Approx(0.25f));
            CHECK(color.b == doctest::Approx(0.75f));
            CHECK(color.a == doctest::Approx(1.0f));
        }

        SUBCASE("Default alpha")
        {
            duin::Color color(0.5f, 0.25f, 0.75f);
            CHECK(color.a == doctest::Approx(1.0f));
        }

        SUBCASE("Black")
        {
            duin::Color color(0.0f, 0.0f, 0.0f);
            CHECK(color.r == doctest::Approx(0.0f));
            CHECK(color.g == doctest::Approx(0.0f));
            CHECK(color.b == doctest::Approx(0.0f));
        }

        SUBCASE("White")
        {
            duin::Color color(1.0f, 1.0f, 1.0f);
            CHECK(color.r == doctest::Approx(1.0f));
            CHECK(color.g == doctest::Approx(1.0f));
            CHECK(color.b == doctest::Approx(1.0f));
        }
    }

    TEST_CASE("Constructor - Integer values [0-255]")
    {
        SUBCASE("Full RGBA")
        {
            duin::Color color(128, 64, 192, 255);
            CHECK(color.r == doctest::Approx(128 / 255.0f));
            CHECK(color.g == doctest::Approx(64 / 255.0f));
            CHECK(color.b == doctest::Approx(192 / 255.0f));
            CHECK(color.a == doctest::Approx(1.0f));
        }

        SUBCASE("Default alpha")
        {
            duin::Color color(255, 0, 0);
            CHECK(color.a == doctest::Approx(1.0f));
        }

        SUBCASE("Pure red")
        {
            duin::Color color(255, 0, 0);
            CHECK(color.r == doctest::Approx(1.0f));
            CHECK(color.g == doctest::Approx(0.0f));
            CHECK(color.b == doctest::Approx(0.0f));
        }

        SUBCASE("Pure green")
        {
            duin::Color color(0, 255, 0);
            CHECK(color.r == doctest::Approx(0.0f));
            CHECK(color.g == doctest::Approx(1.0f));
            CHECK(color.b == doctest::Approx(0.0f));
        }

        SUBCASE("Pure blue")
        {
            duin::Color color(0, 0, 255);
            CHECK(color.r == doctest::Approx(0.0f));
            CHECK(color.g == doctest::Approx(0.0f));
            CHECK(color.b == doctest::Approx(1.0f));
        }
    }

    TEST_CASE("Constructor - Hex value")
    {
        SUBCASE("RGB hex (0xRRGGBB)")
        {
            duin::Color color(0xFF8040);
            CHECK(color.r == doctest::Approx(1.0f));
            CHECK(color.g == doctest::Approx(128 / 255.0f));
            CHECK(color.b == doctest::Approx(64 / 255.0f));
            CHECK(color.a == doctest::Approx(1.0f));
        }

        SUBCASE("RGBA hex (0xRRGGBBAA)")
        {
            duin::Color color(0xFF804080);
            CHECK(color.r == doctest::Approx(1.0f));
            CHECK(color.g == doctest::Approx(128 / 255.0f));
            CHECK(color.b == doctest::Approx(64 / 255.0f));
            CHECK(color.a == doctest::Approx(128 / 255.0f));
        }

        SUBCASE("Black hex")
        {
            duin::Color color(0x000000);
            CHECK(color.r == doctest::Approx(0.0f));
            CHECK(color.g == doctest::Approx(0.0f));
            CHECK(color.b == doctest::Approx(0.0f));
        }

        SUBCASE("White hex")
        {
            duin::Color color(0xFFFFFF);
            CHECK(color.r == doctest::Approx(1.0f));
            CHECK(color.g == doctest::Approx(1.0f));
            CHECK(color.b == doctest::Approx(1.0f));
        }

        SUBCASE("Fully transparent")
        {
            duin::Color color(0xFFFFFF00);
            CHECK(color.a == doctest::Approx(0.0f));
        }

        SUBCASE("Fully opaque")
        {
            duin::Color color(0x000000FF);
            CHECK(color.a == doctest::Approx(1.0f));
        }
    }

    TEST_CASE("ToFloatArray")
    {
        duin::Color color(0.1f, 0.2f, 0.3f, 0.4f);
        auto arr = color.ToFloatArray();

        CHECK(arr[0] == doctest::Approx(0.1f));
        CHECK(arr[1] == doctest::Approx(0.2f));
        CHECK(arr[2] == doctest::Approx(0.3f));
        CHECK(arr[3] == doctest::Approx(0.4f));
    }

    TEST_CASE("ToByteArray")
    {
        SUBCASE("Full intensity")
        {
            duin::Color color(1.0f, 1.0f, 1.0f, 1.0f);
            auto arr = color.ToByteArray();

            CHECK(arr[0] == 255);
            CHECK(arr[1] == 255);
            CHECK(arr[2] == 255);
            CHECK(arr[3] == 255);
        }

        SUBCASE("Zero intensity")
        {
            duin::Color color(0.0f, 0.0f, 0.0f, 0.0f);
            auto arr = color.ToByteArray();

            CHECK(arr[0] == 0);
            CHECK(arr[1] == 0);
            CHECK(arr[2] == 0);
            CHECK(arr[3] == 0);
        }

        SUBCASE("Mid intensity")
        {
            duin::Color color(0.5f, 0.5f, 0.5f, 0.5f);
            auto arr = color.ToByteArray();

            // 0.5 * 255 = 127.5, clamped to 127 or 128
            CHECK(arr[0] >= 127);
            CHECK(arr[0] <= 128);
        }
    }

    TEST_CASE("PackedABGR")
    {
        SUBCASE("Pure red")
        {
            duin::Color color(255, 0, 0, 255);
            uint32_t packed = color.PackedABGR();
            // ABGR format: 0xAABBGGRR
            CHECK(packed == 0xFF0000FF);
        }

        SUBCASE("Pure green")
        {
            duin::Color color(0, 255, 0, 255);
            uint32_t packed = color.PackedABGR();
            CHECK(packed == 0xFF00FF00);
        }

        SUBCASE("Pure blue")
        {
            duin::Color color(0, 0, 255, 255);
            uint32_t packed = color.PackedABGR();
            CHECK(packed == 0xFFFF0000);
        }

        SUBCASE("White opaque")
        {
            duin::Color color(255, 255, 255, 255);
            uint32_t packed = color.PackedABGR();
            CHECK(packed == 0xFFFFFFFF);
        }

        SUBCASE("Black transparent")
        {
            duin::Color color(0, 0, 0, 0);
            uint32_t packed = color.PackedABGR();
            CHECK(packed == 0x00000000);
        }

        SUBCASE("Mixed color")
        {
            duin::Color color(0x12, 0x34, 0x56, 0x78);
            uint32_t packed = color.PackedABGR();
            // ABGR: A=0x78, B=0x56, G=0x34, R=0x12
            CHECK(packed == 0x78563412);
        }
    }

    TEST_CASE("Predefined colors")
    {
        SUBCASE("BLACK")
        {
            CHECK(duin::BLACK.r == doctest::Approx(0.0f));
            CHECK(duin::BLACK.g == doctest::Approx(0.0f));
            CHECK(duin::BLACK.b == doctest::Approx(0.0f));
        }

        SUBCASE("WHITE")
        {
            // Note: WHITE is defined as 225, not 255
            CHECK(duin::WHITE.r == doctest::Approx(225 / 255.0f));
            CHECK(duin::WHITE.g == doctest::Approx(225 / 255.0f));
            CHECK(duin::WHITE.b == doctest::Approx(225 / 255.0f));
        }

        SUBCASE("GRAY")
        {
            CHECK(duin::GRAY.r == doctest::Approx(0.5f));
            CHECK(duin::GRAY.g == doctest::Approx(0.5f));
            CHECK(duin::GRAY.b == doctest::Approx(0.5f));
        }

        SUBCASE("RED")
        {
            CHECK(duin::RED.r == doctest::Approx(1.0f));
            CHECK(duin::RED.g == doctest::Approx(0.0f));
            CHECK(duin::RED.b == doctest::Approx(0.0f));
        }

        SUBCASE("GREEN")
        {
            CHECK(duin::GREEN.r == doctest::Approx(0.0f));
            CHECK(duin::GREEN.g == doctest::Approx(1.0f));
            CHECK(duin::GREEN.b == doctest::Approx(0.0f));
        }

        SUBCASE("BLUE")
        {
            CHECK(duin::BLUE.r == doctest::Approx(0.0f));
            CHECK(duin::BLUE.g == doctest::Approx(0.0f));
            CHECK(duin::BLUE.b == doctest::Approx(1.0f));
        }

        SUBCASE("PINK")
        {
            CHECK(duin::PINK.r == doctest::Approx(1.0f));
            CHECK(duin::PINK.g == doctest::Approx(192 / 255.0f));
            CHECK(duin::PINK.b == doctest::Approx(203 / 255.0f));
        }
    }
}

TEST_SUITE("RenderState")
{
    TEST_CASE("Default construction")
    {
        duin::RenderState state;

        CHECK(state.target == nullptr);
        CHECK(state.camera == nullptr);
        CHECK(state.in3DMode == false);
        CHECK(state.inTextureMode == false);
        CHECK(state.viewID == 0);
    }

    TEST_CASE("State modification")
    {
        duin::RenderState state;

        state.in3DMode = true;
        state.inTextureMode = true;
        state.viewID = 5;

        CHECK(state.in3DMode == true);
        CHECK(state.inTextureMode == true);
        CHECK(state.viewID == 5);
    }

    TEST_CASE("State copy")
    {
        duin::RenderState state1;
        state1.in3DMode = true;
        state1.viewID = 10;

        duin::RenderState state2 = state1;

        CHECK(state2.in3DMode == state1.in3DMode);
        CHECK(state2.viewID == state1.viewID);
    }
}

TEST_SUITE("RenderTexture")
{
    TEST_CASE("Default construction")
    {
        duin::RenderTexture rt;

        CHECK(rt.viewID == 0);
    }

    TEST_CASE("View ID assignment")
    {
        duin::RenderTexture rt;

        rt.viewID = 1;
        CHECK(rt.viewID == 1);

        rt.viewID = 255;
        CHECK(rt.viewID == 255);
    }

    TEST_CASE("Multiple render textures with different view IDs")
    {
        duin::RenderTexture rt1;
        duin::RenderTexture rt2;
        duin::RenderTexture rt3;

        rt1.viewID = 1;
        rt2.viewID = 2;
        rt3.viewID = 3;

        CHECK(rt1.viewID != rt2.viewID);
        CHECK(rt2.viewID != rt3.viewID);
        CHECK(rt1.viewID != rt3.viewID);
    }

    TEST_CASE("Dimension storage")
    {
        duin::RenderTexture rt;

        rt.width = 1920;
        rt.height = 1080;

        CHECK(rt.width == 1920);
        CHECK(rt.height == 1080);
    }

    TEST_CASE("UUID assignment")
    {
        duin::RenderTexture rt;
        duin::UUID testUUID;

        rt.textureUUID = testUUID;

        CHECK(rt.textureUUID == testUUID);
        CHECK(rt.textureUUID != duin::UUID::INVALID);
    }
}

TEST_SUITE("ShaderProgram")
{
    TEST_CASE("Default construction")
    {
        duin::ShaderProgram sp;

        CHECK(sp.isValid == 0);
    }

    TEST_CASE("UUID assignment")
    {
        duin::ShaderProgram sp;
        duin::UUID testUUID;

        sp.uuid = testUUID;

        CHECK(sp.uuid == testUUID);
        CHECK(sp.uuid != duin::UUID::INVALID);
    }

    TEST_CASE("Validity flag")
    {
        duin::ShaderProgram sp;

        CHECK(sp.isValid == 0);

        sp.isValid = 1;
        CHECK(sp.isValid == 1);
    }
}

TEST_SUITE("DebugDrawState")
{
    TEST_CASE("Default construction")
    {
        duin::DebugDrawState state;

        CHECK(state.isActive == false);
        CHECK(state.bgfxEncoder == nullptr);
    }

    TEST_CASE("State modification")
    {
        duin::DebugDrawState state;

        state.isActive = true;
        CHECK(state.isActive == true);
    }
}

TEST_SUITE("RenderStateManagement")
{
    TEST_CASE("Initial render state")
    {
        duin::ResetRenderStateForTesting();

        duin::RenderState state = duin::GetCurrentRenderState();

        CHECK(state.target == nullptr);
        CHECK(state.camera == nullptr);
        CHECK(state.in3DMode == false);
        CHECK(state.inTextureMode == false);
        CHECK(state.viewID == 0);
        CHECK(duin::GetRenderStateStackDepth() == 0);
    }

    TEST_CASE("BeginTextureModeForTesting pushes state")
    {
        duin::ResetRenderStateForTesting();

        duin::RenderTexture rt;
        rt.viewID = 5;
        rt.width = 800;
        rt.height = 600;

        CHECK(duin::GetRenderStateStackDepth() == 0);

        duin::BeginTextureModeForTesting(rt);

        CHECK(duin::GetRenderStateStackDepth() == 1);

        duin::RenderState state = duin::GetCurrentRenderState();
        CHECK(state.inTextureMode == true);
        CHECK(state.in3DMode == false);
        CHECK(state.viewID == 5);
        CHECK(state.target == &rt);
    }

    TEST_CASE("EndTextureModeForTesting pops state")
    {
        duin::ResetRenderStateForTesting();

        duin::RenderTexture rt;
        rt.viewID = 3;

        duin::BeginTextureModeForTesting(rt);
        CHECK(duin::GetRenderStateStackDepth() == 1);

        duin::EndTextureModeForTesting();
        CHECK(duin::GetRenderStateStackDepth() == 0);

        duin::RenderState state = duin::GetCurrentRenderState();
        CHECK(state.inTextureMode == false);
        CHECK(state.target == nullptr);
        CHECK(state.viewID == 0);
    }

    TEST_CASE("Nested texture modes")
    {
        duin::ResetRenderStateForTesting();

        duin::RenderTexture rt1;
        rt1.viewID = 1;

        duin::RenderTexture rt2;
        rt2.viewID = 2;

        // Begin first texture mode
        duin::BeginTextureModeForTesting(rt1);
        CHECK(duin::GetRenderStateStackDepth() == 1);
        CHECK(duin::GetCurrentRenderState().viewID == 1);
        CHECK(duin::GetCurrentRenderState().target == &rt1);

        // Begin nested texture mode
        duin::BeginTextureModeForTesting(rt2);
        CHECK(duin::GetRenderStateStackDepth() == 2);
        CHECK(duin::GetCurrentRenderState().viewID == 2);
        CHECK(duin::GetCurrentRenderState().target == &rt2);

        // End nested texture mode - should restore rt1
        duin::EndTextureModeForTesting();
        CHECK(duin::GetRenderStateStackDepth() == 1);
        CHECK(duin::GetCurrentRenderState().viewID == 1);
        CHECK(duin::GetCurrentRenderState().target == &rt1);

        // End first texture mode - should restore initial state
        duin::EndTextureModeForTesting();
        CHECK(duin::GetRenderStateStackDepth() == 0);
        CHECK(duin::GetCurrentRenderState().viewID == 0);
        CHECK(duin::GetCurrentRenderState().target == nullptr);
    }

    TEST_CASE("EndTextureModeForTesting on empty stack resets state")
    {
        duin::ResetRenderStateForTesting();

        // Calling End without Begin should reset to default
        duin::EndTextureModeForTesting();

        duin::RenderState state = duin::GetCurrentRenderState();
        CHECK(state.inTextureMode == false);
        CHECK(state.in3DMode == false);
        CHECK(state.target == nullptr);
        CHECK(state.viewID == 0);
    }

    TEST_CASE("ResetRenderStateForTesting clears everything")
    {
        duin::RenderTexture rt;
        rt.viewID = 10;

        // Set up some state
        duin::BeginTextureModeForTesting(rt);
        duin::BeginTextureModeForTesting(rt);
        duin::BeginTextureModeForTesting(rt);

        CHECK(duin::GetRenderStateStackDepth() == 3);

        // Reset should clear all
        duin::ResetRenderStateForTesting();

        CHECK(duin::GetRenderStateStackDepth() == 0);
        duin::RenderState state = duin::GetCurrentRenderState();
        CHECK(state.inTextureMode == false);
        CHECK(state.target == nullptr);
    }

    TEST_CASE("Different view IDs are preserved correctly")
    {
        duin::ResetRenderStateForTesting();

        duin::RenderTexture textures[5];
        for (int i = 0; i < 5; ++i)
        {
            textures[i].viewID = static_cast<bgfx::ViewId>(i + 1);
        }

        // Push all textures
        for (int i = 0; i < 5; ++i)
        {
            duin::BeginTextureModeForTesting(textures[i]);
            CHECK(duin::GetCurrentRenderState().viewID == static_cast<size_t>(i + 1));
        }

        CHECK(duin::GetRenderStateStackDepth() == 5);

        // Pop all textures in reverse order
        for (int i = 4; i >= 0; --i)
        {
            CHECK(duin::GetCurrentRenderState().viewID == static_cast<size_t>(i + 1));
            duin::EndTextureModeForTesting();
        }

        CHECK(duin::GetRenderStateStackDepth() == 0);
        CHECK(duin::GetCurrentRenderState().viewID == 0);
    }

    TEST_CASE("Main display uses view ID 0")
    {
        duin::ResetRenderStateForTesting();

        // Default state should use view ID 0 (main display)
        CHECK(duin::GetCurrentRenderState().viewID == 0);

        // Render texture should NOT use view ID 0 to avoid conflict
        duin::RenderTexture rt;
        rt.viewID = 1; // Must be different from main display

        duin::BeginTextureModeForTesting(rt);
        CHECK(duin::GetCurrentRenderState().viewID == 1);
        CHECK(duin::GetCurrentRenderState().viewID != 0); // Should not conflict with main

        duin::EndTextureModeForTesting();
        CHECK(duin::GetCurrentRenderState().viewID == 0); // Back to main display
    }
}

} // namespace TestRenderer
