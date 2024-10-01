#pragma once
#include <string_view>
#include <glm/vec2.hpp>

#include "input_keys.hpp"
#include "events/signal.hpp"

class GLFWwindow;

namespace forge
{
	struct WindowKeyEventData
	{
		bool called;
		int action;
		int mods;
	};

	struct WindowInputEventData
	{
		glm::vec2	mouse_cords;
		glm::vec2	scroll_offsets;
		bool		mouse_called;
		bool		scroll_called;

		std::array<WindowKeyEventData, 119> key_cache;
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

		bool is_key_pressed(Key key, Modifier mod = Modifier::None);
		bool is_key_released(Key key, Modifier mod = Modifier::None);
		bool is_key_held(Key key);

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

		bool get_key(int key, int action, int mod);
	};

}
