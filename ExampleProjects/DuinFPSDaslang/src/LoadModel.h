#pragma once

#include <Duin/Assets/Model.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <optional>
#include <string>

/** @brief Loads the mesh at @p path, returning nullopt if it could not be loaded. */
std::optional<duin::Model> LoadMesh(const std::string& path);

/** @brief Queues @p model for rendering, doing nothing if it is empty.
 *  @p scale is applied on top of the model's baseScale. */
void DrawMesh(
    const std::optional<duin::Model>& model, duin::Vector3 position, float scale = 1.0f,
    duin::Quaternion rotation = duin::QuaternionIdentity());
