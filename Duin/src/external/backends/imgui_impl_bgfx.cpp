// Derived from this Gist by Richard Gale:
//     https://gist.github.com/RichardGale/6e2b74bc42b3005e08397236e4be0fd0
// Updated for Dear ImGui 1.92+ texture status-machine API.

#include "imgui_impl_bgfx.h"
#include "external/imgui.h"

// BGFX/BX
#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"
#include "bx/timer.h"
#include <cstring>

// Data
static uint8_t g_View = 255;
static bgfx::ProgramHandle g_ShaderHandle = BGFX_INVALID_HANDLE;
static bgfx::UniformHandle g_AttribLocationTex = BGFX_INVALID_HANDLE;
static bgfx::VertexLayout g_VertexLayout;

void ImGui_Implbgfx_RenderDrawLists(ImDrawData *draw_data)
{
    ImGuiIO &io = ImGui::GetIO();
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
    {
        return;
    }

    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Handle texture lifecycle (create/update/destroy) driven by imgui 1.92+
    if (draw_data->Textures != nullptr)
    {
        for (ImTextureData *texData : *draw_data->Textures)
        {
            switch (texData->Status)
            {
            case ImTextureStatus_WantCreate:
            {
                bgfx::TextureHandle handle = bgfx::createTexture2D(
                    (uint16_t)texData->Width, (uint16_t)texData->Height,
                    false, 1, bgfx::TextureFormat::BGRA8, 0);
                bgfx::updateTexture2D(handle, 0, 0, 0, 0,
                    (uint16_t)texData->Width, (uint16_t)texData->Height,
                    bgfx::copy(texData->GetPixels(), texData->GetSizeInBytes()));
                texData->SetTexID((ImTextureID)(intptr_t)handle.idx);
                texData->SetStatus(ImTextureStatus_OK);
                break;
            }
            case ImTextureStatus_WantUpdates:
            {
                bgfx::TextureHandle handle = {(uint16_t)((intptr_t)texData->GetTexID() & 0xffff)};
                for (ImTextureRect &rect : texData->Updates)
                {
                    const uint32_t bpp = texData->BytesPerPixel;
                    const bgfx::Memory *pix = bgfx::alloc(rect.h * rect.w * bpp);
                    bx::gather(pix->data, texData->GetPixelsAt(rect.x, rect.y), texData->GetPitch(), rect.w * bpp, rect.h);
                    bgfx::updateTexture2D(handle, 0, 0, rect.x, rect.y, rect.w, rect.h, pix);
                }
                break;
            }
            case ImTextureStatus_WantDestroy:
            {
                bgfx::TextureHandle handle = {(uint16_t)((intptr_t)texData->GetTexID() & 0xffff)};
                bgfx::destroy(handle);
                texData->SetTexID(ImTextureID_Invalid);
                texData->SetStatus(ImTextureStatus_Destroyed);
                break;
            }
            default:
                break;
            }
        }
    }

    uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
                     BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);

    const bgfx::Caps *caps = bgfx::getCaps();

    float ortho[16];
    bx::mtxOrtho(ortho, 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
    bgfx::setViewTransform(g_View, NULL, ortho);
    bgfx::setViewRect(g_View, 0, 0, (uint16_t)fb_width, (uint16_t)fb_height);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList *cmd_list = draw_data->CmdLists[n];

        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        uint32_t numVertices = (uint32_t)cmd_list->VtxBuffer.size();
        uint32_t numIndices = (uint32_t)cmd_list->IdxBuffer.size();

        if ((numVertices != bgfx::getAvailTransientVertexBuffer(numVertices, g_VertexLayout)) ||
            (numIndices != bgfx::getAvailTransientIndexBuffer(numIndices)))
        {
            break;
        }

        bgfx::allocTransientVertexBuffer(&tvb, numVertices, g_VertexLayout);
        bgfx::allocTransientIndexBuffer(&tib, numIndices);

        ImDrawVert *verts = (ImDrawVert *)tvb.data;
        memcpy(verts, cmd_list->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert));

        ImDrawIdx *indices = (ImDrawIdx *)tib.data;
        memcpy(indices, cmd_list->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];

            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const uint16_t xx = (uint16_t)bx::max(pcmd->ClipRect.x, 0.0f);
                const uint16_t yy = (uint16_t)bx::max(pcmd->ClipRect.y, 0.0f);
                bgfx::setScissor(xx, yy, (uint16_t)bx::min(pcmd->ClipRect.z, 65535.0f) - xx,
                                 (uint16_t)bx::min(pcmd->ClipRect.w, 65535.0f) - yy);

                bgfx::setState(state);
                bgfx::TextureHandle texture = {(uint16_t)((intptr_t)pcmd->GetTexID() & 0xffff)};
                bgfx::setTexture(0, g_AttribLocationTex, texture);
                bgfx::setVertexBuffer(0, &tvb, 0, numVertices);
                bgfx::setIndexBuffer(&tib, pcmd->IdxOffset, pcmd->ElemCount);
                bgfx::submit(g_View, g_ShaderHandle);
            }
        }
    }
}

#include "fs_ocornut_imgui.bin.h"
#include "vs_ocornut_imgui.bin.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER(vs_ocornut_imgui), BGFX_EMBEDDED_SHADER(fs_ocornut_imgui), BGFX_EMBEDDED_SHADER_END()};

bool ImGui_Implbgfx_CreateDeviceObjects()
{
    bgfx::RendererType::Enum type = bgfx::getRendererType();
    g_ShaderHandle = bgfx::createProgram(bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui"),
                                         bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui"), true);

    g_VertexLayout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    g_AttribLocationTex = bgfx::createUniform("g_AttribLocationTex", bgfx::UniformType::Sampler);

    return true;
}

void ImGui_Implbgfx_InvalidateDeviceObjects()
{
    bgfx::destroy(g_AttribLocationTex);
    bgfx::destroy(g_ShaderHandle);
}

void ImGui_Implbgfx_Init(int view)
{
    g_View = (uint8_t)(view & 0xff);

    ImGuiIO &io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_bgfx";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
}

void ImGui_Implbgfx_Shutdown()
{
    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasTextures;
    if (io.BackendRendererName != nullptr &&
        std::strcmp(io.BackendRendererName, "imgui_impl_bgfx") == 0)
    {
        io.BackendRendererName = nullptr;
    }

    ImGui_Implbgfx_InvalidateDeviceObjects();
    g_View = 255;
}

void ImGui_Implbgfx_NewFrame()
{
    if (!bgfx::isValid(g_ShaderHandle))
    {
        ImGui_Implbgfx_CreateDeviceObjects();
    }
}
