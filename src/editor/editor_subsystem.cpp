#include "editor_subsystem.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "gui/imgui_subsystem.hpp"

class IEditorWindow;

static forge::HashMap<std::string_view, IEditorWindow*> g_window_table;

class IEditorWindow : public forge::IComponent
{
public:

	IEditorWindow(std::string_view name, bool show_window = false) :
		show_window(show_window),
		m_window_name(name)
	{}

	void on_editor_enter() override
	{
		g_window_table[m_window_name] = this;
	}

	void update(forge::DeltaTime) override
	{
		if (show_window)
		{
			ImGui::Begin(m_window_name.data(), &show_window);
			on_window();
			ImGui::End();
		}
	}

	bool show_window = false;

protected:
	std::string_view m_window_name;

	virtual void on_window() {}
	virtual void on_register_window() {}
};

class StatisticsEditorWindow final : public IEditorWindow
{
public:
	float last_frame {};
	float last_engine_delta {};
	forge::DeltaTime elapsed_time {};

	StatisticsEditorWindow() : IEditorWindow("Statistics") {}

	void update(forge::DeltaTime delta) override
	{
		IEditorWindow::update(delta);

		elapsed_time += delta;

		auto &engine = forge::Engine::get_instance();

		if (elapsed_time >= 0.3)
		{
			last_frame = engine.get_fps();
			last_engine_delta = engine.get_delta();

			elapsed_time = 0;
		}
	}

protected:
	void on_window() override
	{
		ImGui::Text("FPS: %d", (int)last_frame);
		ImGui::Text("Tick: %f", last_engine_delta);
	}
};

class SettingsEditorWindow final : public IEditorWindow
{
public:
	bool toggle_wireframe = false;

	SettingsEditorWindow() : IEditorWindow("Settings") {}

protected:

	void on_window() override
	{
		auto &engine = forge::Engine::get_instance();

		if (ImGui::BeginTabBar("Rendering"))
		{
			if (ImGui::BeginTabItem("Rendering"))
			{
				if (ImGui::Checkbox("Wireframe", &toggle_wireframe))
				{
					engine.renderer->set_wireframe(toggle_wireframe);
				}

				auto clear_color = engine.renderer->get_clear_color();

				if (ImGui::ColorEdit3("Clear color", glm::value_ptr(clear_color)))
				{
					engine.renderer->set_clear_color(clear_color);
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
};

class SceneOutlineEditorWindow final : public IEditorWindow
{
public:
	bool toggle_wireframe = false;

	SceneOutlineEditorWindow() : IEditorWindow("Scene Outline") {}

protected:

	void on_window() override
	{
		auto &engine = forge::Engine::get_instance();

	}
};

class TopBarEditorComponent final : public forge::IComponent
{
public:
	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close"))
				{
					engine.quit();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				for (auto &[name, window] : g_window_table)
				{
					ImGui::Checkbox(name.data(), &window->show_window);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
};

std::string forge::EditorSubsystem::init()
{
	auto result = m_nexus.init();

	if (!result.empty())
	{
		return result;
	}

	// entity for all editor windows
	auto *windows_entity = m_nexus.create_entity("windows");

	windows_entity->add_components<StatisticsEditorWindow, SettingsEditorWindow>(true);

	windows_entity->on_editor_enter();

	// entity for anything related to view config such as the top bar
	auto *config_entity = m_nexus.create_entity("config");

	config_entity->add_components<TopBarEditorComponent>(true);

	return {};
}

void forge::EditorSubsystem::shutdown()
{
	m_nexus.shutdown();
}

void forge::EditorSubsystem::update()
{
	m_nexus.update();
}

std::vector<std::type_index> forge::EditorSubsystem::get_dependencies()
{
	return { typeid(ImGuiSubsystem) };
}
