#pragma once
#include <glm/vec4.hpp>

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
				case Diffuse: return "diffuse";
				case Specular: return "specular";
				case Emissive: return "emissive";
				default: return "";
			}
		}
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
		TextureList<Texture> textures;
		glm::vec3 specular {0.5};
		glm::vec3 diffuse {1.0};
		float ambient;
		float shininess = 16;
	};
}
