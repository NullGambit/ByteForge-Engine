#include <imgui.h>
#include <numeric>

#include "glm/glm.hpp"
#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "src/util/types.hpp"
#include "graphics/ogl_renderer/ogl_renderer.hpp"

class DebugRenderComponent : public forge::IComponent
{
public:
	float last_frame {};
	float last_engine_delta {};
	float last_delta {};
	forge::DeltaTime elapsed_time {};

	void update(forge::DeltaTime delta) override
	{
		elapsed_time += delta;

		auto &engine = forge::Engine::get_instance();

		if (elapsed_time >= 0.3)
		{
			last_frame = engine.get_fps();
			last_engine_delta = engine.get_delta();
			last_delta = delta;

			elapsed_time = 0;
		}

		ImGui::Begin("debug window");

		if (forge::is_key_pressed(forge::Key::R))
		{
			engine.renderer->toggle_wireframe();
		}

		ImGui::Text("FPS: %d", (int)last_frame);
		ImGui::Text("Tick: %f", last_engine_delta);
		ImGui::Text("ECS: %f", last_delta);

		ImGui::End();
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

	if (!ok)
	{
		return -1;
	}

	auto *entity = engine.nexus->create_entity();

	entity->add_components<DebugRenderComponent, CameraComponent>(true);

	engine.run();

	engine.shutdown();
}
