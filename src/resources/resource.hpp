#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace forge
{
	using ResourceID = uint32_t;

	class IResource
	{
	public:
		virtual ~IResource() = default;

		virtual std::optional<ResourceID> load(std::string_view path) = 0;
		virtual bool save(std::string_view path, ResourceID id) = 0;
	};
}