#pragma once
#include <string_view>

#include "events/signal.hpp"

class GLFWwindow;

namespace forge
{
	class Window
	{
	public:
		~Window();

		void close();

		bool open(std::string_view name, int width, int height);

		bool should_close() const;

		bool should_stay_open() const;

		void set_should_close(bool value);

		void swap_buffers() const;

		Signal<void(int, int)> on_resize;

	private:
		GLFWwindow *m_handle;
	};
}
