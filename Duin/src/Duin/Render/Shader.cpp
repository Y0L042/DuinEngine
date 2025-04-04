#include "dnpch.h"
#include "Shader.h"

namespace duin {
    bgfx::ShaderHandle LoadShader(const char *path)
    {
        const char* shaderPath = "???";

        switch(bgfx::getRendererType()) {
            case bgfx::RendererType::Noop:
            case bgfx::RendererType::Agc:                                       break;
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12: shaderPath = "./Resources/shaders/dx11/";  break;
            case bgfx::RendererType::Gnm:        shaderPath = "./Resources/shaders/pssl/";  break;
            case bgfx::RendererType::Metal:      shaderPath = "./Resources/shaders/metal/"; break;
            case bgfx::RendererType::OpenGL:     shaderPath = "./Resources/shaders/glsl/";  break;
            case bgfx::RendererType::OpenGLES:   shaderPath = "./Resources/shaders/essl/";  break;
            case bgfx::RendererType::Vulkan:     shaderPath = "./Resources/shaders/spirv/"; break;
        }



        FILE *file = fopen(path, "rb");
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
        fread(mem->data, 1, fileSize, file);
        mem->data[mem->size - 1] = '\0';
        fclose(file);

        return bgfx::createShader(mem);
    }
}
