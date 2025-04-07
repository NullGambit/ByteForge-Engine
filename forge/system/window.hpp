#pragma once
#include <bitset>
#include <string_view>
#include <glm/vec2.hpp>

#include "input_keys.hpp"
#include "forge/events/signal.hpp"

class GLFWwindow;

namespace forge
{
	struct WindowInputEventData
	{
		glm::vec2 mouse_cords;
		glm::vec2 scroll_offsets;

		std::bitset<119> key_down;
		std::bitset<119> key_up;
		std::bitset<7> mouse_button_down;
		std::bitset<7> mouse_button_up;
	};

	class Window
	{
	public:
		~Window();

		void close();

		bool open(std::string_view name, int width, int height);

		[[nodiscard]]
		bool should_close() const;

		[[nodiscard]]
		inline bool is_open() const
		{
			return m_handle != nullptr;
		}

		[[nodiscard]]
		bool should_stay_open() const;

		void set_should_close(bool value);

		void swap_buffers() const;

		void set_context();

		void set_cursor_mode(CursorMode mode) const;
		CursorMode get_cursor_mode() const;

		bool is_key_pressed(Key key, KeyModifier mod = KeyModifier::None) const;
		bool is_key_released(Key key, KeyModifier mod = KeyModifier::None) const;
		bool is_key_held(Key key) const;
		bool is_mouse_button_pressed(MouseButton key, KeyModifier mod = KeyModifier::None) const;
		bool is_mouse_button_released(MouseButton key, KeyModifier mod = KeyModifier::None) const;
		bool is_mouse_button_held(MouseButton key) const;
		glm::vec2 get_mouse_coords() const;

		// gets a reference to a struct that stores data related to input events
		// should not be used unless you know what you're doing.
		WindowInputEventData& get_input_event_data()
		{
			return m_input_event_data;
		}

		void reset_input();

		GLFWwindow* get_handle() const;

		[[nodiscard]]
		glm::ivec2 get_size() const;

		Signal<void(int, int)> on_resize;

	private:
		GLFWwindow *m_handle = nullptr;
		WindowInputEventData m_input_event_data;

		bool get_mouse_button(int button, int action, int mod) const;
		bool get_key(int key, int action, int mod) const;
	};

}
