#pragma once
#include <glm/vec4.hpp>

namespace forge
{
	namespace TextureType
	{
		enum TEXTURE_TYPE
		{
			Diffuse,
			Specular,
			Emissive,
			Max,
		};
	}

	template<class T>
	using TextureList = std::array<T, TextureType::Max>;

	struct Texture
	{
		std::string path;
		float scale = 1;
		bool enabled = false;
	};

	struct Material
	{
		glm::vec4 color {1.0};
		TextureList<Texture> textures;
	};
}
