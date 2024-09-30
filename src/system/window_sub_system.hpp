#pragma once
#include "../core/isub_system.hpp"

namespace forge
{
	class WindowSubSystem : public ISubSystem
	{
	public:
		std::string init() override;
		void shutdown() override;
		void update() override;
		bool is_critical() override { return true; }

		[[nodiscard]]
		float get_runtime() const;
	};
}
