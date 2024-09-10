#pragma once
#include "../core/isub_system.hpp"

namespace forge
{
	class WindowSubSystem : public ISubSystem
	{
	public:
		void init() override;
		void shutdown() override;
		void update() override;
	};
}
