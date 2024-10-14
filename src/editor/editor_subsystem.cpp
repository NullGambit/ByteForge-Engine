#include "editor_subsystem.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "gui/imgui_subsystem.hpp"
#include "util/types.hpp"

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
		auto &engine = forge::Engine::get_instance();
		auto render_stats = engine.renderer->get_statistics();

		ImGui::Text("FPS: %d", (int)last_frame);
		ImGui::Text("Tick: %f", last_engine_delta);
		ImGui::Text("Draw calls: %d", render_stats.draw_calls);
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

	SceneOutlineEditorWindow() : IEditorWindow("Scene Outline") {}

	void update(forge::DeltaTime delta) override
	{
		IEditorWindow::update(delta);

		m_show_components_window = show_window;

		if (m_show_components_window && m_selected_entity.has_value())
		{
			ImGui::Begin("Components", &m_show_components_window);

			auto &entity = m_selected_entity.get();

			auto should_delete_entity = ImGui::Button("Delete Entity");

			char name_buffer[256];

			// auto name = entity.get_name();
			//
			// if (!name.empty())
			// {
			// 	strncpy(name_buffer, name.data(), name.size());
			// }

			if (ImGui::InputText("##", name_buffer, sizeof(name_buffer)))
			{
				entity.set_name(name_buffer);
			}

			ImGui::SameLine();

			if (ImGui::Button("+"))
			{
				ImGui::OpenPopup("Add component");
			}

			if (ImGui::BeginPopup("Add component"))
			{
				auto &engine = forge::Engine::get_instance();

				for (auto &[index, ct] : engine.nexus->get_component_table())
				{
					if (ImGui::Selectable(util::type_name(index).data()))
					{
						entity.add_component(index);
					}
				}

				ImGui::EndPopup();
			}

			for (auto &[index, view] : entity.get_components())
			{
				auto *component = (IComponent*)view.pointer;

				auto is_enabled = component->is_enabled();

				ImGui::PushID(index.name());

				if (ImGui::Checkbox("##", &is_enabled))
				{
					component->set_enabled(is_enabled);
				}

				ImGui::PopID();

				ImGui::SameLine();

				if (ImGui::TreeNode(util::type_name(index).data()))
				{
					auto should_delete = ImGui::Button("Delete");

					for (auto &[name, value] : component->export_fields())
					{
						auto formatted = std::string{name};

						if (formatted.starts_with("m_"))
						{
							formatted.erase(formatted.begin(), formatted.begin()+2);
						}

						std::replace(formatted.begin(), formatted.end(), '_', ' ');

						if (value.index() == 0)
						{
							auto *f_value = std::get<0>(value);

							ImGui::DragFloat(formatted.data(), f_value);
						}
					}

					ImGui::TreePop();

					if (should_delete)
					{
						entity.remove_component(index);
					}
				}
			}

			if (should_delete_entity)
			{
				entity.destroy();

				if (m_selected_entity.index > 0)
				{
					m_selected_entity.index--;
				}
			}

			ImGui::End();
		}
	}

protected:

	void show_entities(u32 entity_table_index = 0)
	{
		auto &engine = forge::Engine::get_instance();
		auto &entities_table = engine.nexus->get_all_entities();

		if (entity_table_index >= entities_table.size())
		{
			return;
		}

		auto &entities = entities_table[entity_table_index];

		for (u32 index = 0; auto &entity : entities)
		{
			auto name = std::string{entity.get_name()};

			if (name.empty())
			{
				name += "Entity_" + std::to_string(index);
			}

			auto flags = 0;

			if (m_selected_entity.index == index)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			auto node_enabled = ImGui::TreeNodeEx(name.c_str(), flags);

			if (ImGui::IsItemClicked())
			{
				m_selected_entity = entity.get_view();
			}

			if (node_enabled)
			{
				auto children_index = entity.get_children_index();

				if (children_index > 0)
				{
					show_entities(children_index);
				}

				ImGui::TreePop();
			}

			index++;
		}
	}

	void on_window() override
	{
		show_entities();
	}

private:
	forge::EntityView m_selected_entity;
	bool m_show_components_window = false;
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

	windows_entity->add_components<StatisticsEditorWindow, SettingsEditorWindow, SceneOutlineEditorWindow>(true);

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
