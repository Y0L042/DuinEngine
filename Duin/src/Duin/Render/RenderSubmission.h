#pragma once

#include "RHI.h"
namespace duin
{
struct MeshData
{
	RHIVertexLayout vertexLayout;
	size_t bufferSize;
	RHIVertexBufferHandle vertexBufferHandle;
	RHIIndexBufferHandle indexBufferHandle;
};

struct RenderSubmission
{
	MeshData meshData;
	RHIShaderHandle shaderHandle; // If empty uses default shader
};

} // namespace duin