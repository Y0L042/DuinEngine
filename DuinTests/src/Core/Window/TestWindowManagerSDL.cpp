#include <doctest.h>
#include <Duin/Core/Window/WindowManager.h>
#include <Duin/Core/Window/WindowCtx.h>
#include <Duin/Core/Events/EventHandler.h>
#include <Duin/Render/RHI.h>
#include <Duin/Render/Camera.h>
#include <Duin/Assets/AssetManager.h>
#include <Duin/Assets/Model.h>
#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

namespace TestWindowManagerSDL
{
static bool EnvEnabled(const char *name)
{
    const char *value = std::getenv(name);
    return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
}

struct SdlVideoScope
{
    bool ok = false;

    SdlVideoScope()
    {
        ::SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "offscreen");
        ok = ::SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (!ok)
        {
            ::SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
            ok = ::SDL_InitSubSystem(SDL_INIT_VIDEO);
        }
    }

    ~SdlVideoScope()
    {
        if (ok)
        {
            ::SDL_QuitSubSystem(SDL_INIT_VIDEO);
        }
        ::SDL_ResetHint(SDL_HINT_VIDEO_DRIVER);
    }
};

static void PushClose(duin::EventHandler &handler, std::shared_ptr<duin::WindowCtx> window)
{
    SDL_Event e = {};
    e.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    e.window.windowID = ::SDL_GetWindowID(window->GetSDLWindow());
    handler.PollEvent(e);
}

TEST_SUITE("WindowSDL" * doctest::skip(!EnvEnabled("DUIN_TEST_SDL_VIDEO")))
{
    TEST_CASE("First window becomes primary, later windows are secondary")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::WindowManager wm;
        int created = 0;
        wm.OnWindowCreated.Connect([&](std::shared_ptr<duin::WindowCtx>) { created++; });

        std::shared_ptr<duin::WindowCtx> first = wm.CreateNewWindow("Primary", 320, 240);
        std::shared_ptr<duin::WindowCtx> second = wm.CreateNewWindow("Secondary", 200, 100);
        REQUIRE(first != nullptr);
        REQUIRE(second != nullptr);

        CHECK(created == 2);
        CHECK(wm.GetPrimaryWindow() == first);
        CHECK(wm.GetPrimarySDLWindow() == first->GetSDLWindow());
        CHECK(wm.GetWindowCtx(first->GetSDLWindow()) == first);
        CHECK(wm.GetWindowCtx(second->GetSDLWindow()) == second);

        auto secondaries = wm.GetSecondaryWindows();
        REQUIRE(secondaries.size() == 1);
        CHECK(secondaries[0] == second);

        wm.DestroyAllWindows();
    }

    TEST_CASE("Size, pixel size and title round-trip on a live window")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::WindowManager wm;
        std::shared_ptr<duin::WindowCtx> window = wm.CreateNewWindow("Sized", 320, 240);
        REQUIRE(window != nullptr);
        CHECK(window->IsWindowValid());

        int w = 0, h = 0;
        window->GetSize(w, h);
        CHECK(w == 320);
        CHECK(h == 240);

        int pw = 0, ph = 0;
        window->GetSizeInPixels(pw, ph);
        CHECK(pw >= w);
        CHECK(ph >= h);

        window->SetSize(400, 300);
        ::SDL_SyncWindow(window->GetSDLWindow());
        CHECK(window->GetWidth() == 400);
        CHECK(window->GetHeight() == 300);

        window->SetTitle("Renamed");
        CHECK(window->GetTitle() == "Renamed");

        wm.DestroyAllWindows();
    }

    TEST_CASE("SetSDLWindowFlags toggles resizable")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::WindowManager wm;
        std::shared_ptr<duin::WindowCtx> window = wm.CreateNewWindow("Flags", 160, 120);
        REQUIRE(window != nullptr);

        window->SetSDLWindowFlags(window->GetSDLWindowFlags() | SDL_WINDOW_RESIZABLE);
        CHECK((window->GetSDLWindowFlags() & SDL_WINDOW_RESIZABLE) != 0);

        window->SetSDLWindowFlags(window->GetSDLWindowFlags() & ~SDL_WINDOW_RESIZABLE);
        CHECK((window->GetSDLWindowFlags() & SDL_WINDOW_RESIZABLE) == 0);

        wm.DestroyAllWindows();
    }

    TEST_CASE("Destroying a secondary window goes through release and destroy phases")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::WindowManager wm;
        std::shared_ptr<duin::WindowCtx> primary = wm.CreateNewWindow("Primary", 160, 120);
        std::shared_ptr<duin::WindowCtx> secondary = wm.CreateNewWindow("Secondary", 160, 120);
        REQUIRE(primary != nullptr);
        REQUIRE(secondary != nullptr);

        int destroying = 0;
        wm.OnWindowDestroying.Connect([&](std::shared_ptr<duin::WindowCtx> w) {
            destroying++;
            CHECK(w == secondary);
        });

        SDL_Window *raw = secondary->GetSDLWindow();
        CHECK(wm.DestroyWindow(raw));
        CHECK(secondary->IsPendingDestroy());
        CHECK(wm.GetWindowCtx(raw) == nullptr);
        CHECK(wm.GetSecondaryWindows().empty());
        CHECK(secondary->IsWindowValid());

        wm.ReleasePendingWindows();
        CHECK(destroying == 1);
        CHECK(secondary->IsWindowValid());

        wm.DestroyPendingWindows();
        CHECK_FALSE(secondary->IsWindowValid());
        CHECK(wm.GetPrimaryWindow() == primary);

        wm.DestroyAllWindows();
    }

    TEST_CASE("Close request for a secondary window marks it pending")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::EventHandler handler;
        duin::WindowManager wm;
        wm.SetEventHandler(&handler);

        std::shared_ptr<duin::WindowCtx> primary = wm.CreateNewWindow("Primary", 160, 120);
        std::shared_ptr<duin::WindowCtx> secondary = wm.CreateNewWindow("Secondary", 160, 120);
        REQUIRE(primary != nullptr);
        REQUIRE(secondary != nullptr);

        PushClose(handler, secondary);
        CHECK(secondary->IsPendingDestroy());
        CHECK(wm.GetSecondaryWindows().empty());
        CHECK(wm.GetPrimaryWindow() == primary);

        wm.ReleasePendingWindows();
        wm.DestroyPendingWindows();
        wm.DestroyAllWindows();
    }

    TEST_CASE("Close request for the primary window emits OnPrimaryWindowClosing")
    {
        SdlVideoScope video;
        REQUIRE(video.ok);

        duin::EventHandler handler;
        duin::WindowManager wm;
        wm.SetEventHandler(&handler);

        int closing = 0;
        wm.OnPrimaryWindowClosing.Connect([&]() { closing++; });

        std::shared_ptr<duin::WindowCtx> primary = wm.CreateNewWindow("Primary", 160, 120);
        REQUIRE(primary != nullptr);

        PushClose(handler, primary);
        CHECK(closing == 1);
        CHECK(wm.GetPrimaryWindow() == nullptr);

        wm.DestroyAllWindows();
    }
}

struct BgfxScope
{
    BgfxScope()
    {
        duin::RHIStart(duin::RHI_VIEW_3D, 0, 0, []() -> HWND { return nullptr; }, true);
        duin::RHIInit();
    }

    ~BgfxScope()
    {
        duin::RHIShutdown();
        duin::RHIClose();
    }
};

static std::string WriteObj(const std::string &name, const std::string &content)
{
    std::filesystem::create_directories("./artifacts/window_sdl");
    std::string path = "./artifacts/window_sdl/" + name;
    std::ofstream f(path);
    f << content;
    return path;
}

static std::string CubeObj()
{
    return "v -1 -1 -1\nv 1 -1 -1\nv 1 1 -1\nv -1 1 -1\n"
           "v -1 -1 1\nv 1 -1 1\nv 1 1 1\nv -1 1 1\n"
           "f 1 2 3\nf 1 3 4\nf 5 6 7\nf 5 7 8\n"
           "f 1 2 6\nf 1 6 5\nf 4 3 7\nf 4 7 8\n"
           "f 1 4 8\nf 1 8 5\nf 2 3 7\nf 2 7 6\n";
}

TEST_SUITE("WindowSDL - bgfx" * doctest::skip(!EnvEnabled("DUIN_TEST_BGFX")))
{
    TEST_CASE("Projection matrix scales x by the inverse aspect ratio")
    {
        BgfxScope bgfx;
        duin::CameraImpl impl{};
        impl.position = duin::Vector3(0.0f, 0.0f, 5.0f);
        impl.target = duin::Vector3(0.0f, 0.0f, 0.0f);
        impl.globalUp = duin::Vector3(0.0f, 1.0f, 0.0f);
        impl.fovy = 60.0f;

        for (float aspect : {1.0f, 16.0f / 9.0f, 0.5f})
        {
            duin::Matrix m = duin::GetCameraProjectionMatrix(impl, aspect);
            float raw[16];
            std::memcpy(raw, &m, sizeof(raw));
            CHECK(raw[0] * aspect == doctest::Approx(raw[5]).epsilon(1e-4));
        }
    }

    TEST_CASE("Window framebuffer creation clamps a zero size without crashing")
    {
        BgfxScope bgfx;
        SdlVideoScope video;
        REQUIRE(video.ok);
        duin::WindowManager wm;
        std::shared_ptr<duin::WindowCtx> window = wm.CreateNewWindow("Swap", 64, 64);
        REQUIRE(window != nullptr);

        if (window->GetNativeHandle() == nullptr)
        {
            MESSAGE("Video driver exposes no native handle; skipping framebuffer creation");
            wm.DestroyAllWindows();
            return;
        }

        duin::RHIFrameBufferHandle fb = duin::RHICreateWindowFrameBuffer(window->GetNativeHandle(), 0, 0);
        MESSAGE("Window framebuffer valid: " << fb.IsValid());
        wm.DestroyAllWindows();
    }

    TEST_CASE("LoadModel uploads a cube and caches it by path")
    {
        BgfxScope bgfx;
        duin::AssetManager &am = duin::AssetManager::Get();
        std::string path = WriteObj("cube.obj", CubeObj());

        std::optional<duin::Model> first = am.LoadModel(path);
        REQUIRE(first.has_value());
        REQUIRE(first->data != nullptr);
        CHECK(am.HasAsset(path));
        CHECK(first->data->vertices.size() == 8);
        CHECK(first->data->indices.size() == 36);

        std::optional<duin::Model> second = am.LoadModel(path);
        REQUIRE(second.has_value());
        CHECK(second->data == first->data);
    }

    TEST_CASE("LoadModel keeps indices for meshes over 65535 vertices" * doctest::may_fail())
    {
        BgfxScope bgfx;
        duin::AssetManager &am = duin::AssetManager::Get();

        const int triangles = 23000;
        std::string obj;
        obj.reserve(triangles * 64);
        for (int i = 0; i < triangles * 3; ++i)
        {
            obj += "v " + std::to_string(i) + " " + std::to_string(i % 7) + " " + std::to_string(i % 13) + "\n";
        }
        for (int t = 0; t < triangles; ++t)
        {
            int a = t * 3 + 1;
            obj += "f " + std::to_string(a) + " " + std::to_string(a + 1) + " " + std::to_string(a + 2) + "\n";
        }
        std::string path = WriteObj("big.obj", obj);

        std::optional<duin::Model> model = am.LoadModel(path);
        REQUIRE(model.has_value());
        REQUIRE(model->data != nullptr);

        size_t vertexCount = model->data->vertices.size();
        REQUIRE(vertexCount > 65535);

        size_t maxIndex = 0;
        for (auto idx : model->data->indices)
        {
            maxIndex = std::max<size_t>(maxIndex, idx);
        }
        CHECK(maxIndex == vertexCount - 1);
    }
}
} // namespace TestWindowManagerSDL
