#include <doctest.h>
#include <Duin/Core/Window/WindowCtx.h>
#include <Duin/Render/RHI.h>
#include <string>
#include <vector>

namespace TestWindowCtx
{
TEST_SUITE("WindowCtx - Defaults and plain state")
{
    TEST_CASE("Default-constructed context has documented defaults")
    {
        duin::WindowCtx ctx;
        CHECK(ctx.GetTargetRenderFramerate() == 60);
        CHECK(ctx.GetClearColor() == 0x303030ffu);
        CHECK(ctx.GetViewSlot() == -1);
        CHECK_FALSE(ctx.IsPendingDestroy());
        CHECK_FALSE(ctx.UsingCustomImguiPath());
        CHECK(ctx.GetImguiPath() == "./");
        CHECK_FALSE(ctx.GetPauseOnMinimize());
        CHECK_FALSE(ctx.GetAllowDockingInMain());
        CHECK(ctx.GetCamera() == nullptr);
        CHECK(ctx.GetImGuiContext() == nullptr);
        CHECK(ctx.GetSDLWindow() == nullptr);
        CHECK(ctx.GetSDLSurface() == nullptr);
        CHECK_FALSE(ctx.GetFrameBuffer().IsValid());

        int w = -1, h = -1;
        ctx.GetFrameBufferSize(w, h);
        CHECK(w == 0);
        CHECK(h == 0);
    }

    TEST_CASE("Plain setters round-trip")
    {
        duin::WindowCtx ctx;
        ctx.SetTargetRenderFramerate(144);
        ctx.SetClearColor(0x11223344u);
        ctx.SetViewSlot(3);
        ctx.SetPendingDestroy(true);
        ctx.UseCustomImguiPath(true);
        ctx.SetImguiPath("custom/imgui.ini");
        ctx.SetPauseOnMinimize(true);
        ctx.SetAllowDockingInMain(true);

        duin::Camera *fakeCamera = reinterpret_cast<duin::Camera *>(0x1);
        ::ImGuiContext *fakeImGui = reinterpret_cast<::ImGuiContext *>(0x2);
        ctx.SetCamera(fakeCamera);
        ctx.SetImGuiContext(fakeImGui);

        duin::RHIFrameBufferHandle fb;
        fb.idx = 7;
        ctx.SetFrameBuffer(fb, 640, 480);

        CHECK(ctx.GetTargetRenderFramerate() == 144);
        CHECK(ctx.GetClearColor() == 0x11223344u);
        CHECK(ctx.GetViewSlot() == 3);
        CHECK(ctx.IsPendingDestroy());
        CHECK(ctx.UsingCustomImguiPath());
        CHECK(ctx.GetImguiPath() == "custom/imgui.ini");
        CHECK(ctx.GetPauseOnMinimize());
        CHECK(ctx.GetAllowDockingInMain());
        CHECK(ctx.GetCamera() == fakeCamera);
        CHECK(ctx.GetImGuiContext() == fakeImGui);
        CHECK(ctx.GetFrameBuffer().idx == 7);

        int w = 0, h = 0;
        ctx.GetFrameBufferSize(w, h);
        CHECK(w == 640);
        CHECK(h == 480);

        ctx.SetCamera(nullptr);
        ctx.SetImGuiContext(nullptr);
    }
}

TEST_SUITE("WindowCtx - View ids")
{
    TEST_CASE("Scene and UI views are interleaved per slot")
    {
        duin::WindowCtx ctx;

        ctx.SetViewSlot(0);
        CHECK(ctx.GetSceneView() == duin::RHI_VIEW_WINDOW_BASE);
        CHECK(ctx.GetUIView() == duin::RHI_VIEW_WINDOW_BASE + 1);

        ctx.SetViewSlot(1);
        CHECK(ctx.GetSceneView() == duin::RHI_VIEW_WINDOW_BASE + 2);
        CHECK(ctx.GetUIView() == duin::RHI_VIEW_WINDOW_BASE + 3);

        ctx.SetViewSlot(duin::RHI_MAX_WINDOWS - 1);
        CHECK(ctx.GetSceneView() == duin::RHI_VIEW_WINDOW_BASE + 2 * (duin::RHI_MAX_WINDOWS - 1));
        CHECK(ctx.GetUIView() == duin::RHI_VIEW_WINDOW_BASE + 2 * (duin::RHI_MAX_WINDOWS - 1) + 1);
    }

    TEST_CASE("Window view range does not collide with fixed views")
    {
        duin::WindowCtx ctx;
        for (int slot = 0; slot < duin::RHI_MAX_WINDOWS; ++slot)
        {
            ctx.SetViewSlot(slot);
            CHECK(ctx.GetSceneView() != duin::RHI_VIEW_3D);
            CHECK(ctx.GetSceneView() != duin::RHI_VIEW_RENDERTEXTURE);
            CHECK(ctx.GetUIView() != duin::RHI_VIEW_IMGUI);
            CHECK(ctx.GetUIView() < duin::RHI_VIEW_IMGUI);
        }
    }

    TEST_CASE("Unassigned slot maps below the window base")
    {
        duin::WindowCtx ctx;
        CHECK(ctx.GetSceneView() == duin::RHI_VIEW_WINDOW_BASE - 2);
        CHECK(ctx.GetUIView() == duin::RHI_VIEW_WINDOW_BASE - 1);
    }

    TEST_CASE("Default framebuffer handle is invalid")
    {
        duin::RHIFrameBufferHandle fb;
        CHECK_FALSE(fb.IsValid());
        fb.idx = 0;
        CHECK(fb.IsValid());
    }
}

TEST_SUITE("WindowCtx - Without an SDL window")
{
    TEST_CASE("Queries report an invalid window")
    {
        duin::WindowCtx ctx;
        CHECK_FALSE(ctx.IsWindowValid());

        int w = -1, h = -1;
        ctx.GetSize(w, h);
        CHECK(w == 0);
        CHECK(h == 0);

        w = -1;
        h = -1;
        ctx.GetSizeInPixels(w, h);
        CHECK(w == 0);
        CHECK(h == 0);

        CHECK(ctx.GetWidth() == 0);
        CHECK(ctx.GetHeight() == 0);
        CHECK(ctx.GetTitle().empty());
        CHECK(ctx.GetSDLWindowFlags() == 0);
        CHECK(ctx.GetNativeHandle() == nullptr);
    }

    TEST_CASE("Setters and Destroy are safe no-ops")
    {
        duin::WindowCtx ctx;
        ctx.SetSize(320, 240);
        ctx.SetTitle("Ignored");
        ctx.SetSDLWindowFlags(SDL_WINDOW_RESIZABLE);
        ctx.Destroy();
        ctx.Destroy();

        CHECK_FALSE(ctx.IsWindowValid());
        CHECK(ctx.GetTitle().empty());
    }

    TEST_CASE("ProcessWindowEvent accepts any event")
    {
        duin::WindowCtx ctx;
        duin::WindowEvent event;
        event.sdlEvent = {};
        event.sdlEvent.type = SDL_EVENT_WINDOW_RESIZED;
        ctx.ProcessWindowEvent(event);
        CHECK_FALSE(ctx.IsWindowValid());
    }
}

TEST_SUITE("WindowCtx - Draw queues")
{
    TEST_CASE("Draw queue runs callbacks once in FIFO order")
    {
        duin::WindowCtx ctx;
        std::vector<int> order;
        ctx.QueueDraw([&]() { order.push_back(1); });
        ctx.QueueDraw([&]() { order.push_back(2); });
        ctx.QueueDraw([&]() { order.push_back(3); });

        ctx.ExecuteDrawQueue();
        REQUIRE(order.size() == 3);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);
        CHECK(order[2] == 3);

        ctx.ExecuteDrawQueue();
        CHECK(order.size() == 3);
    }

    TEST_CASE("UI draw queue runs callbacks once in FIFO order")
    {
        duin::WindowCtx ctx;
        std::vector<int> order;
        ctx.QueueDrawUI([&]() { order.push_back(1); });
        ctx.QueueDrawUI([&]() { order.push_back(2); });

        ctx.ExecuteDrawUIQueue();
        REQUIRE(order.size() == 2);
        CHECK(order[0] == 1);
        CHECK(order[1] == 2);

        ctx.ExecuteDrawUIQueue();
        CHECK(order.size() == 2);
    }

    TEST_CASE("Scene and UI queues are independent")
    {
        duin::WindowCtx ctx;
        int scene = 0, ui = 0;
        ctx.QueueDraw([&]() { scene++; });
        ctx.QueueDrawUI([&]() { ui++; });

        ctx.ExecuteDrawQueue();
        CHECK(scene == 1);
        CHECK(ui == 0);

        ctx.ExecuteDrawUIQueue();
        CHECK(scene == 1);
        CHECK(ui == 1);
    }

    TEST_CASE("Clear drops queued callbacks without running them")
    {
        duin::WindowCtx ctx;
        int scene = 0, ui = 0;
        ctx.QueueDraw([&]() { scene++; });
        ctx.QueueDraw([&]() { scene++; });
        ctx.QueueDrawUI([&]() { ui++; });

        ctx.ClearDrawQueue();
        ctx.ExecuteDrawQueue();
        CHECK(scene == 0);

        ctx.ExecuteDrawUIQueue();
        CHECK(ui == 1);

        ctx.QueueDrawUI([&]() { ui++; });
        ctx.ClearDrawUIQueue();
        ctx.ExecuteDrawUIQueue();
        CHECK(ui == 1);
    }

    TEST_CASE("Clear on an empty queue is safe")
    {
        duin::WindowCtx ctx;
        ctx.ClearDrawQueue();
        ctx.ClearDrawUIQueue();
        ctx.ExecuteDrawQueue();
        ctx.ExecuteDrawUIQueue();
        CHECK_FALSE(ctx.IsWindowValid());
    }

    TEST_CASE("Callback queued during a drain runs in the same drain")
    {
        duin::WindowCtx ctx;
        std::vector<std::string> order;
        ctx.QueueDraw([&]() {
            order.push_back("outer");
            ctx.QueueDraw([&]() { order.push_back("inner"); });
        });

        ctx.ExecuteDrawQueue();
        REQUIRE(order.size() == 2);
        CHECK(order[0] == "outer");
        CHECK(order[1] == "inner");

        ctx.ExecuteDrawQueue();
        CHECK(order.size() == 2);
    }
}
} // namespace TestWindowCtx
