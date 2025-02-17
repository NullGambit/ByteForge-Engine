#pragma once

#include <cstdint>
#include <string_view>

namespace forge
{
	using ResourceID = u32;
	constexpr ResourceID NO_RESOURCE = UINT32_MAX;

	class IResource
	{
	public:
		virtual ~IResource() = default;

		virtual ResourceID load(std::string_view path) = 0;
		virtual bool save(std::string_view path, ResourceID id) = 0;
	};
}