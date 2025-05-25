#pragma once

#include <array>
#include <string_view>

namespace forge
{
	namespace TextureType
	{
		enum TEXTURE_TYPE : u8
		{
			Diffuse,
			Specular,
			Emissive,
			Max,
		};

		inline std::string_view to_string(u8 type)
		{
			switch (type)
			{
				case Diffuse: return std::string_view{"diffuse", 7};
				case Specular: return std::string_view{"specular", 8};
				case Emissive: return std::string_view{"emissive", 8};
				default: return "";
			}
		}
	}

	template<class T>
	using TextureList = std::array<T, TextureType::Max>;

	struct Texture
	{
		float scale = 1;
		float strength = 1.0;
		bool enabled = false;
	};

	struct Material
	{
		glm::vec3 color {1.0};
		TextureList<Texture> textures;
	};
}
