#pragma once
#include <string_view>

namespace forge
{
	class ISubSystem
	{
	public:
		virtual ~ISubSystem() = default;

		// if the returning string is not empty that means an error occurred
		virtual std::string init() = 0;
		virtual void shutdown() = 0;
		virtual void update() = 0;

		virtual bool should_update() { return true; }

		// will run the update method on its own thread
		virtual bool should_be_threaded() { return false; }

		// if true the system init call must succeed or else the engine will fail to init
		virtual bool is_critical() { return false; }
	};
}
