#include "glm/glm.hpp"
#include <condition_variable>
#include <iostream>
#include <thread>

#include "core/engine.hpp"
#include "src/util/types.hpp"
#include "core/logging.hpp"
#include "core/type_def.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"
#include "GLFW/glfw3.h"

class DebugRenderComponent : public forge::IComponent
{
public:
	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		if (glfwGetKey(engine.window.get_handle(), GLFW_KEY_R))
		{
			engine.renderer->toggle_wireframe();
		}
	}
};

class CameraComponent : public forge::IComponent
{
public:
	forge::Camera camera {forge::CameraProjectionMode::Perspective};

	CameraComponent()
	{
		camera.set_position({0, 0, -10});
	}

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		auto runtime = forge::Engine::get_instance().get_engine_runtime();
		auto radius = 10.0f;
		auto camera_x = glm::sin(runtime) * radius;
		auto camera_z = glm::cos(runtime) * radius;

		camera.look_at({camera_x, 0.0f, camera_z}, {});

		engine.renderer->update_pv(camera.pv());
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

	entity->add_components<DebugRenderComponent, CameraComponent>(true);

	if (!ok)
	{
		return -1;
	}

	engine.run();

	engine.shutdown();
}
