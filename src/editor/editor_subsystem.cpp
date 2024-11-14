#include "editor_subsystem.hpp"

#include <imgui.h>
#include "misc/cpp/imgui_stdlib.h"
#include <imgui_internal.h>
#include <ranges>
#include <bits/ranges_algo.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/engine.hpp"
#include "core/logging.hpp"
#include "framework/input.hpp"
#include "gui/imgui_subsystem.hpp"
#include "util/types.hpp"

class IEditorWindow;

static forge::HashMap<std::string_view, IEditorWindow*> g_window_table;

struct ScopedWindow
{
	ScopedWindow(std::string_view name, bool *is_open = nullptr)
	{
		ImGui::Begin(name.data(), is_open);
	}

	~ScopedWindow()
	{
		ImGui::End();
	}
};

struct ContextMenuItem
{
	std::string_view name;
	std::function<void()> callback;
	std::optional<ImVec4> color = std::nullopt;
	std::function<bool()> predicate = []{ return true; };
	bool on_hover = false;
};

bool check_context_menu(std::string_view name)
{
	if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered())
	{
		ImGui::OpenPopup(name.data());
		return true;
	}

	return false;
}

void open_context_menu(std::string_view name, const std::vector<ContextMenuItem> &items)
{
	if (!ImGui::IsPopupOpen(name.data()))
	{
		check_context_menu(name);
	}

	if (ImGui::BeginPopup(name.data()))
	{
		for (const auto &item : items)
		{
			if (item.predicate() && ImGui::Selectable(item.name.data()) || (item.on_hover && ImGui::IsItemHovered()))
			{
				item.callback();
			}
		}

		ImGui::EndPopup();
	}
}

class IEditorWindow : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

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

	REGISTER_UPDATE_FUNC

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
		ImGui::Text("Entity tables: %d", engine.nexus->get_all_entities().size());
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

			if (ImGui::BeginTabItem("Engine"))
			{
				ImGui::DragFloat("Time scale", &engine.time_scale, 0.1);

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
};

class Dialog
{
public:
	Dialog(std::string_view name) :
		m_name(name)
	{}

	void open()
	{
		ImGui::OpenPopup(m_name.data());
		m_show_modal = true;
	}

	void take_text_input(std::function<void(std::string&)> &&callback)
	{
		if (ImGui::BeginPopupModal(m_name.data(), &m_show_modal, ImGuiChildFlags_AlwaysAutoResize))
		{
			ImGui::InputText("Input", &m_input_buffer);

			if (should_confirm())
			{
				callback(m_input_buffer);

				m_show_modal = false;

				m_input_buffer.clear();

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

private:
	std::string_view m_name;
	bool m_show_modal = false;
	std::string m_input_buffer;

	bool should_confirm() const
	{
		return ImGui::Button("Confirm") || ImGui::IsKeyPressed(ImGuiKey_Enter) && !m_input_buffer.empty();
	}
};

class SceneOutlineEditorWindow final : public IEditorWindow
{
public:

	SceneOutlineEditorWindow() : IEditorWindow("Scene Outline") {}

protected:

	void vec_drag_control(std::string_view label, float *values, int components, bool *uniform = nullptr,
		float speed = 1, float min_steps = 0, float max_steps = 0)
	{
		static const char *component_table[] = {"X", "Y", "Z", "W"};

		ImGui::Text(label.data());

		if (uniform != nullptr)
		{
			ImGui::SameLine();
			ImGui::PushID(uniform);
			ImGui::Checkbox("##", uniform);
			ImGui::PopID();
		}

		ImGui::PushItemWidth(50);

		for (auto i = 0; i < components; i++)
		{
			ImGui::PushID(values + i);

			ImGui::DragFloat(component_table[i], &values[i], speed, min_steps, max_steps);

			ImGui::PopID();

			if (i != components-1)
			{
				ImGui::SameLine();
			}
		}

		ImGui::PopItemWidth();
	}

	template<class T>
	void vec_drag_control(std::string_view label, T &values, bool *uniform = nullptr,
		float speed = 0.01, float min_steps = 0, float max_steps = 0)
	{
		auto *ptr = glm::value_ptr(values);
		vec_drag_control(label, ptr, sizeof(T) / sizeof(ptr[0]), uniform, speed, min_steps, max_steps);
	}

	static u32 find_last_member_index(std::string_view name)
	{
		for (auto i = name.size(); auto c : std::ranges::reverse_view(name))
		{
			if (c == '.' || c == '>')
			{
				return i;
			}

			i--;
		}

		return 0;
	}

	bool render_field(std::string_view name, forge::FieldVar &value, u32 id)
	{
		auto format_field = [](std::string_view name)
		{
			name = name.substr(find_last_member_index(name));
			// will likely start with an & because of taking the member address
			auto offset = name.starts_with('&');
			auto formatted = std::string{name.substr(offset)};

			if (formatted.starts_with("m_"))
			{
				formatted.erase(formatted.begin(), formatted.begin()+2);
			}

			return formatted;
		};

		auto formatted = format_field(name);

		std::replace(formatted.begin(), formatted.end(), '_', ' ');

		ImGui::PushID(id);

		auto out = std::visit(util::overload
		{
			[&formatted](float *value)
			{
				return ImGui::DragFloat(formatted.data(), value);
			},
			[&formatted](double *value)
			{
				return ImGui::DragFloat(formatted.data(), (float*)value);
			},
			[&formatted](int *value)
			{
				return ImGui::DragInt(formatted.data(), value);
			},
			[&formatted](bool *value)
			{
				return ImGui::Checkbox(formatted.data(), value);
			},
			[&formatted](std::string *value)
			{
				return ImGui::InputText(formatted.data(), value);
			},
			[&formatted](forge::ButtonField &value)
			{
				auto button_name = value.name.empty() ? formatted : value.name;

				if (ImGui::Button(button_name.data()))
				{
					value.callback();
					return true;
				}

				return false;
			},
			[](forge::FieldSeperator value)
			{
				if (value.name.empty())
				{
					ImGui::Separator();
				}
				else
				{
					ImGui::SeparatorText(value.name.data());
				}

				return false;
			},
			[&formatted](glm::vec4 *value)
			{
				return ImGui::DragFloat4(formatted.data(), glm::value_ptr(*value));
			},
			[&formatted](glm::vec3 *value)
			{
				return ImGui::DragFloat3(formatted.data(), glm::value_ptr(*value));
			},
			[&formatted](glm::vec2 *value)
			{
				return ImGui::DragFloat2(formatted.data(), glm::value_ptr(*value));
			},
			[&formatted](glm::quat *value)
			{
				return ImGui::DragFloat4(formatted.data(), glm::value_ptr(*value));
			},
			[&format_field](forge::ColorField value)
			{
				if (value.color_value.index() == 0)
				{
					return ImGui::ColorEdit3(format_field(value.name).data(), glm::value_ptr(*std::get<0>(value.color_value)));
				}
				else
				{
					return ImGui::ColorEdit4(format_field(value.name).data(), glm::value_ptr(*std::get<1>(value.color_value)));
				}
			},
			[&](forge::WatchedField *value)
			{
				auto field_changed = render_field(formatted, value->field, id++);

				if (field_changed)
				{
					value->on_changed(value->field);
				}

				return field_changed;
			},
		}, value);

		ImGui::PopID();

		return out;
	}

	void draw_right_side()
	{
		if (show_window && m_selected_entity.has_value())
		{
			ScopedWindow components_window {"Components"};

			if (!m_selected_entity.is_entity_valid())
			{
				return;
			}

			auto &entity = m_selected_entity.get_entity();

			char name_buffer[256] {};

			auto name = entity.get_name();

			if (!name.empty())
			{
				strcpy(name_buffer, name.data());
			}

			if (ImGui::InputText("##", name_buffer, sizeof(name_buffer)))
			{
				entity.set_name(name_buffer);
			}

			ImGui::SameLine();

			ImGui::Text("(%d)", entity.get_id());

			if (ImGui::CollapsingHeader("Transform"))
			{
				static bool uniform_scale = true;

				auto scale = entity.get_local_scale();

				vec_drag_control("Scale", scale, &uniform_scale);

				entity.set_local_scale(uniform_scale ? glm::vec3{scale[0]} : scale);

				auto position = entity.get_local_position();

				vec_drag_control("Position", position);

				entity.set_local_position(position);

				auto rotation = entity.get_local_euler_rotation();

				vec_drag_control("Rotation", rotation);

				entity.set_local_rotation(rotation);
			}

			ImGui::Separator();

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

			for (auto i = 0; auto &[index, view] : entity.get_components())
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
						render_field(name, value, i++);
					}

					ImGui::TreePop();

					if (should_delete)
					{
						entity.remove_component(index);
					}
				}
			}
		}
	}

	void draw_left_side()
	{
		if (ImGui::BeginTabBar("LeftSide"))
		{
			if (ImGui::BeginTabItem("Scene"))
			{
				m_is_in_group_tab = false;

				show_entities(0);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Groups"))
			{
				m_is_in_group_tab = true;

				draw_groups();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void draw_groups()
	{
		if (ImGui::Button("Create group"))
		{
			m_create_group_dialog.open();
		}

		m_create_group_dialog.take_text_input([](std::string &input)
		{
			forge::Engine::get_instance().nexus->create_group(input);
		});

		auto &engine = forge::Engine::get_instance();

		for (auto &[name, entities] : engine.nexus->get_all_groups())
		{
			if (ImGui::IsItemHovered())
			{
				m_last_hovered_group_name = name;
			}

			if (ImGui::TreeNode(name.c_str()))
			{
				open_context_menu("GroupContext",
				{
					{
						"Delete", [&name]
						{
							forge::Engine::get_instance().nexus->remove_group(name);
						},
						ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
					},
				});

				for (auto &view : entities)
				{
					if (!view->is_entity_valid())
					{
						continue;
					}

					draw_entity(view->get_entity(), name);
				}

				ImGui::TreePop();
			}
		}
	}

	bool does_filter_fail(std::string_view name)
	{
		return !m_filter.empty() && name.find(m_filter) == std::string::npos;
	}

	void draw_entity(forge::Entity &entity, std::string_view from_group = "")
	{
		auto name = entity.get_name();

		if (name.empty())
		{
			name = "Entity";
		}

		auto flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (m_selected_entity == *entity.get_view())
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (does_filter_fail(name))
		{
			return;
		}

		ImGui::PushID(entity.get_id());

		auto node_enabled = ImGui::TreeNodeEx(name.data(), flags);

		ImGui::PopID();

		if (ImGui::IsItemClicked())
		{
			m_selected_entity = *entity.get_view();
		}

		if (check_context_menu("EntityContext"))
		{
			m_selected_context_entity = *entity.get_view();
		}

		if (*entity.get_view() == m_selected_context_entity && ImGui::BeginPopup("EntityContext"))
		{
			if (ImGui::Selectable("Add child"))
			{
				m_selected_entity = *entity.emplace_child().get_view();
			}
			if (ImGui::BeginMenu("Add to group"))
			{
				auto &engine = forge::Engine::get_instance();

				auto &groups = engine.nexus->get_all_groups();

				for (auto &[name, _] : groups)
				{
					if (ImGui::MenuItem(name.data()))
					{
						engine.nexus->add_to_group(name, m_selected_context_entity.get_entity());
					}
				}

				ImGui::EndMenu();
			}
			if (m_is_in_group_tab && ImGui::Selectable("Remove from group"))
			{
				forge::Engine::get_instance().nexus->remove_from_group(from_group, m_selected_context_entity.get_entity());
			}

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

			if (ImGui::Selectable("Delete"))
			{
				m_selected_context_entity.get_entity().destroy();
			}

			ImGui::PopStyleColor(1);

			ImGui::EndPopup();
		}

		if (node_enabled)
		{
			auto children_index = entity.get_children_index();

			if (children_index > 0)
			{
				show_entities(children_index, from_group);
			}

			ImGui::TreePop();
		}
	}

	void show_entities(u32 entity_table_index = 0, std::string_view from_group = "")
	{
		auto &engine = forge::Engine::get_instance();
		auto &entities_table = engine.nexus->get_all_entities();

		if (entity_table_index >= entities_table.size())
		{
			return;
		}

		auto &entities = entities_table[entity_table_index].entities;

		for (auto &[entity, _] : entities)
		{
			draw_entity(entity, from_group);
		}
	}

	void on_window() override
	{
		if (ImGui::Button("+"))
		{
			auto &engine = forge::Engine::get_instance();

			engine.nexus->create_entity();
		}

		ImGui::SameLine();

		static char name_buffer[256] {};

		if (ImGui::InputText("Filter", name_buffer, sizeof(name_buffer)))
		{
			m_filter = name_buffer;
		}

		draw_left_side();
		draw_right_side();
	}

private:
	u32 m_entity_counter = 0;
	forge::EntityView m_selected_entity;
	forge::EntityView m_selected_context_entity;
	std::string_view m_filter;
	bool m_is_in_group_tab = false;
	std::string_view m_last_hovered_group_name;
	Dialog m_create_group_dialog {"Create group"};
};

class TopBarEditorComponent final : public forge::IComponent
{
public:

	REGISTER_UPDATE_FUNC

	void update(forge::DeltaTime delta) override
	{
		auto &engine = forge::Engine::get_instance();

		if (m_show_demo_window)
		{
			ImGui::ShowDemoWindow();
		}

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
				ImGui::Checkbox("Demo", &m_show_demo_window);

				for (auto &[name, window] : g_window_table)
				{
					ImGui::Checkbox(name.data(), &window->show_window);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

private:
	bool m_show_demo_window;
};

std::string forge::EditorSubsystem::init()
{
	auto result = m_nexus.init();

	if (!result.empty())
	{
		return result;
	}

	// entity for all editor windows
	auto &windows_entity = m_nexus.create_entity("windows");

	windows_entity.add_components<StatisticsEditorWindow, SettingsEditorWindow, SceneOutlineEditorWindow>();

	windows_entity.on_editor_enter();

	// entity for anything related to view config such as the top bar
	auto &config_entity = m_nexus.create_entity("config");

	config_entity.add_components<TopBarEditorComponent>();

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
