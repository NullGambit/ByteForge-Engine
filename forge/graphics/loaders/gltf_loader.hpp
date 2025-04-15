#pragma once

#include <optional>
#include <string_view>

#include "mesh_loader.hpp"

namespace forge
{
	struct GltfLoader
	{
		std::optional<MeshLoaderNode> load_mesh(std::string_view filepath, MeshLoadOptions options);
	};
}