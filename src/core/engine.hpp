#pragma once
#include <memory>
#include <vector>

#include "isub_system.hpp"
#include "system/window.hpp"

namespace forge
{
	class OglRenderSubSystem;
}

namespace forge
{
	class Engine
	{
	public:

		Engine() = default;

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		static inline Engine& get_instance()
		{
			static Engine instance;
			return instance;
		}

		void quit();

		void init();

		void run();

		void shutdown();

		template<class T>
		inline T* add_subsystem() requires std::derived_from<T, ISubSystem>
		{
			return (T*)m_subsystems.emplace_back(std::make_unique<T>()).get();
		}

		Window window;
		OglRenderSubSystem *renderer;
	private:
		std::vector<std::unique_ptr<ISubSystem>> m_subsystems;
	};
}
