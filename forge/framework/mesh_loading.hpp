#pragma once
#include <string_view>

#include "forge/graphics/loaders/mesh_loader.hpp"

namespace forge
{
	void load_meshes_hierarchy(std::string_view filepath, MeshLoadOptions options = {});
}
