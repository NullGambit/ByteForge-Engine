#pragma once

#include "forge/graphics/mesh.hpp"
#include "forge/graphics/material.hpp"
#include "forge/graphics/image/image.hpp"
#include "forge/container/array.hpp"
#include "../../math/transform.hpp"
#include "forge/graphics/lights.hpp"

#include <optional>

namespace forge
{
	struct RenderObject;

	struct MeshLoadOptions
	{
		float uniform_scale = 1;
	};

	struct MeshLoadPrimitive
	{
		Mesh mesh;
		Material material;
		Image texture;
	};

	struct MeshLoaderNode
	{
		String name;
		Array<MeshLoadPrimitive> primitives;
		std::optional<Light> light;
		Transform transform;
		Array<MeshLoaderNode> children;
	};
}
