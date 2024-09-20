#include <condition_variable>
#include <iostream>
#include <thread>
#include <typeindex>

#include "core/engine.hpp"
#include "src/util/types.hpp"
#include "core/logging.hpp"
#include "GLFW/glfw3.h"
#include "graphics/ogl_renderer/ogl_renderer.hpp"


class DebugRenderComponent : public forge::BaseComponent
{
public:
	std::mutex m_mutex;
	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		if (glfwGetKey(engine.window.get_handle(), GLFW_KEY_R))
		{
			engine.renderer->toggle_wireframe();
		}
	}
};

int main()
{
	auto &engine = forge::Engine::get_instance();

	auto ok = engine.init(
	{
		.window_title = "ByteForge Engine",
		.window_width = 720,
		.window_height = 480,
	});

	auto *entity = engine.nexus->create_entity();

	entity->add_component<DebugRenderComponent>(true);

	if (!ok)
	{
		return -1;
	}

	engine.run();

	engine.shutdown();
}
