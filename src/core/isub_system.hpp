#pragma once
#include <string_view>

namespace forge
{
	class ISubSystem
	{
	public:
		virtual ~ISubSystem() = default;

		virtual void init() = 0;
		virtual void shutdown() = 0;
		virtual void update() = 0;

		virtual std::string_view error_message() const { return {}; };
		virtual bool is_ok() const { return true;};
	};
}
